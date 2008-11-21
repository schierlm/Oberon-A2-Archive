// ATIGPU.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATIGPU.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

//    CAL_RESULT_OK                = 0, /**< No error */
//    CAL_RESULT_ERROR             = 1, /**< Operational error */
//    CAL_RESULT_INVALID_PARAMETER = 2, /**< Parameter passed in is invalid */
//    CAL_RESULT_NOT_SUPPORTED     = 3, /**< Function used properly but currently not supported */
//    CAL_RESULT_ALREADY           = 4, /**< Stateful operation requested has already been performed */
//    CAL_RESULT_NOT_INITIALIZED   = 5, /**< CAL function was called without CAL being initialized */
//    CAL_RESULT_BAD_HANDLE        = 6, /**< A handle parameter is invalid */
//    CAL_RESULT_BAD_NAME_TYPE     = 7, /**< A name parameter is invalid */
//    CAL_RESULT_PENDING           = 8, /**< An asynchronous operation is still pending */
//    CAL_RESULT_BUSY              = 9,  /**< The resource in question is still in use */
//    CAL_RESULT_WARNING           = 10, /**< Compiler generated a warning */

// TRUE when library is loaded by a process
BOOL isLoaded = FALSE;

// TRUE when the library is successfully initialized
BOOL isInitialized = FALSE;

// GPU devices
DevicePool* devs = NULL;

// context pools for each device
ContextPool** ctxPools = NULL;

// argument pool
ArgumentPool** argPools = NULL;

#define ARG1	0
#define ARG2	1
#define RETARG	2


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	CALresult err;	
	CALuint count;
	Device* dev;
	long i;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		// only one process can load the library!
		if(!isLoaded) 
			isLoaded = TRUE; 
		else 
			return FALSE;

		// initialize CAL interface
		err = calInit(); isInitialized = (err == CAL_RESULT_OK);

		if(isInitialized)
		{	
			count = 0;
			err = calDeviceGetCount(&count); // get number of available devices
			if(count > 0)
			{
				devs = new DevicePool;

				for(i = 0; i < (long)count; i++)
				{
					dev = new Device(i);
					if(dev->hDev)
						devs->Add(dev);
					else
						delete dev;
				}
			}
		}

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:				

		if(isInitialized)
		{	
			if(devs)
			{				
				delete devs;
				devs = NULL;
			}
			err = calShutdown();
			isInitialized = FALSE;
		}

		isLoaded = FALSE;
		
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


/*
	Get amount of accessible ATI GPUs

	devCount[var] - number of accessible device

	returns error code
*/
ATIGPU_API long GetDevCount(long* devCount)
{
	if(!isInitialized) 
		return CAL_RESULT_NOT_INITIALIZED;	

	*devCount = devs->Length();
	return CAL_RESULT_OK;
}

/*
	Create a new CPU computing context

	devNum - device number
	ctxNum[var] - context number

	returns error code
*/
ATIGPU_API long CreateContext(long devNum, long* ctx)
{
	CALresult err;	
	ContextPool* ctxs;

	if(!isInitialized) 
		return CAL_RESULT_NOT_INITIALIZED;
	if( (devNum < 0) || (devNum >= devs->Length()) ) 
		return CAL_RESULT_INVALID_PARAMETER;	

	ctxs = devs->Get(devNum)->ctxs;

	ctxs->Lock();
	err = devs->Get(devNum)->NewContext(ctx);
	ctxs->Unlock();	

	return err;
}

/*
	Destroy already existing GPU context

	devNum - device number
	ctxNum - context number

	returns error code
*/
ATIGPU_API long DestroyContext(long devNum, long ctx)
{	
	long ind;
	ContextPool* ctxs;

	if(!isInitialized) 
		return CAL_RESULT_NOT_INITIALIZED;
	if( (devNum < 0) || (devNum >= devs->Length()) ) 
		return CAL_RESULT_INVALID_PARAMETER;	
	
	ctxs = devs->Get(devNum)->ctxs;
	
	// find context in the pool
	ctxs->Lock();
	ind = ctxs->Find(ctx);
	ctxs->Remove(ind);
	ctxs->Unlock();	

	return CAL_RESULT_OK;
}

/*
	Set argument:

	devNum - used device number
	ctx - used context
	argID - argument ID 
	dType - data type code
	nDims - number of dimensions
	size - size for each dimensions	
	data - data to set	
	bSetData - TRUE when data has to be set
	argType - can be ARG1, ARG2, RETARG

	returns error code	
*/
long SetArg(long devNum, long ctx, long argID, long dType, long nDims, long* size, void* data, BOOL bSetData, long argType)
{
	long i, ind;	
	Context* context;
	Argument* arg;
	CALresult err = CAL_RESULT_OK;	

	Device* dev;	
	ArgumentPool* args;

	if(!isInitialized) 
		return CAL_RESULT_NOT_INITIALIZED;
	if( (devNum < 0) || (devNum >= devs->Length()) ) 
		return CAL_RESULT_INVALID_PARAMETER;	
	if(nDims <= 0) 
		return CAL_RESULT_INVALID_PARAMETER;
	if(argID == 0) 
		return CAL_RESULT_INVALID_PARAMETER;
	if(data == NULL) 
		return CAL_RESULT_INVALID_PARAMETER;
	
	// lock all context and argument pools
	for(i = 0; i < devs->Length(); i++)
	{
		devs->Get(i)->ctxs->Lock();
		devs->Get(i)->args->Lock();
	}

	ind = devs->Get(devNum)->ctxs->Find(ctx);
	if(ind >= 0) 
		context = devs->Get(devNum)->ctxs->Get(ind);
	else
	{
		// unlock all context and argument pools
		for(i = 0; i < devs->Length(); i++)
		{
			devs->Get(i)->ctxs->Unlock();
			devs->Get(i)->args->Unlock();
		}
		return CAL_RESULT_INVALID_PARAMETER;
	}	
	
	// look for already existing argument
	i = 0;
	while((i < devs->Length()) && ((ind = devs->Get(i)->args->Find(argID)) == -1) ){i++;}

	if(ind == -1) // create a new argument on current device
	{
		dev = devs->Get(devNum);
		args = dev->args;

		err = args->NewArgument(dev->hDev,context->ctx,argID,dType,nDims,size,data);		
		if( err == CAL_RESULT_OK )
		{
			arg = (Argument*)args->GetLast();
			// if necessary set data to the local GPU memory
			if(bSetData) err = arg->SetDataToLocal(context->ctx);
		}
	}
	else // use already existing argument
	{		
		err = CAL_RESULT_NOT_SUPPORTED;
	}

	if(err == CAL_RESULT_OK)
	{
		arg->isInUse = TRUE;	// set flag that the object is in use!
		switch(argType)
		{
			case ARG1: context->arg1 = arg; break;
			case ARG2: context->arg2 = arg; break;
			case RETARG: context->retArg = arg; break;
		}
	}
	
	// unlock all context and argument pools
	for(i = 0; i < devs->Length(); i++)
	{
		devs->Get(i)->ctxs->Unlock();
		devs->Get(i)->args->Unlock();
	}			
	
	return err;
}

/*
	Set first argument of an array expression

	devNum - used device number
	ctx - used context
	argID - argument ID 
	dType - data type code
	nDims - number of dimensions
	size - size for each dimensions	
	data - data to set

	returns error code	
*/
ATIGPU_API long SetArg1(long devNum, long ctx, long argID, long dType, long nDims, long* size, void* data)
{	
	return SetArg(devNum,ctx,argID,dType,nDims,size,data,TRUE,ARG1);
}

/*
	Set second argument of an array expression

	devNum - used device number
	ctx - used context
	argID - argument ID 
	dType - data type code
	nDims - number of dimensions
	size - size for each dimensions	
	data - data to set

	returns error code	
*/
ATIGPU_API long SetArg2(long devNum, long ctx, long argID, long dType, long nDims, long* size, void* data)
{
	return SetArg(devNum,ctx,argID,dType,nDims,size,data,TRUE,ARG2);
}

/*
	Set return argument of an array expression

	devNum - used device number
	ctx - used context
	argID - argument ID 
	dType - data type code
	nDims - number of dimensions
	size - size for each dimensions	
	data - data to set
	bSetData - if 0 the data will not be set

	returns error code	
*/
ATIGPU_API long SetReturnArg(long devNum, long ctx, long argID, long dType, long nDims, long* size, void* data, long bSetData)
{
	return SetArg(devNum,ctx,argID,dType,nDims,size,data,bSetData,RETARG);
}


/*
	Get return argument

	devNum - used device number
	ctx - used context	

	returns error code

	Copies return argument data from local/remote GPU memory to CPU memory
*/
ATIGPU_API long GetReturnArg(long devNum, long ctx)
{
	long ind;	
	Context* context;
	CALresult err = CAL_RESULT_OK;	

	ContextPool* ctxs;
	ArgumentPool* args;

	if(!isInitialized) 
		return CAL_RESULT_NOT_INITIALIZED;
	if( (devNum < 0) || (devNum >= devs->Length()) ) 
		return CAL_RESULT_INVALID_PARAMETER;	
	
	ctxs = devs->Get(devNum)->ctxs;
	args = devs->Get(devNum)->args;
	
	ctxs->Lock();
	args->Lock();		
	
	ind = ctxs->Find(ctx);
	if(ind >= 0) 
		context = ctxs->Get(ind);
	else
	{
		ctxs->Unlock();
		args->Unlock();
		return CAL_RESULT_INVALID_PARAMETER;
	}

	if(context->retArg)
	{
		_ASSERT( (context->retArg->remoteRes) || (context->retArg->localRes) );

		if(context->retArg->remoteRes) 
			err = context->retArg->GetDataFromRemote(context->ctx);
		else 
		{
			err = context->retArg->GetDataFromLocal(context->ctx);		
			// FIXME: what to do if it can not get from local because of failure of remote allocation?!
		}
	}
	else
	{
		err = CAL_RESULT_INVALID_PARAMETER;
	}		

	return err;
}


/*
	Compute an op operation using already set Arg1, Arg2, RetArg

	devNum - used device number
	ctx - used context	
	op - operation code

	returns error code
*/
ATIGPU_API long Do(long devNum, long ctx, long op)
{
	long ind;	
	Context* context;
	CALresult err = CAL_RESULT_OK;	

	ContextPool* ctxs;
	ArgumentPool* args;

	if(!isInitialized) 
		return CAL_RESULT_NOT_INITIALIZED;
	if( (devNum < 0) || (devNum >= devs->Length()) ) 
		return CAL_RESULT_INVALID_PARAMETER;	
	
	ctxs = devs->Get(devNum)->ctxs;
	args = devs->Get(devNum)->args;

	ctxs->Lock();

	ind = ctxs->Find(ctx);
	if(ind >= 0) 
		context = ctxs->Get(ind);
	else
	{
		ctxs->Unlock();		
		return CAL_RESULT_INVALID_PARAMETER;
	}

	ctxs->Unlock();

	err = context->Do(op);

	return err;
}
