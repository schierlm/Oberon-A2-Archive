#include "StdAfx.h"
#include "Contexts.h"
#include "Common.h"

Context::Context(CALdevice hDev, CALdeviceinfo* devInfo, CALdeviceattribs* devAttribs, ArrayPool* arrs, Kernel** kernels, CALresult* err)
{		
	long i;	

	isUsed = FALSE;
	modules = NULL;	
	expr = NULL;
	result = NULL;
	resultTemp = NULL;

	idleCounter = 0;
	cacheHitCounter = 0;

	*err = calCtxCreate(&ctx,hDev);
	if(*err != CAL_RESULT_OK)
	{
		ctx = 0; 
		return;
	}	

	counterExtSupported = InitCounterExtension();

	if(counterExtSupported)
	{
		*err = calCtxCreateCounterExt(&idleCounter,ctx,CAL_COUNTER_IDLE);
		if(*err != CAL_RESULT_OK)
			idleCounter = 0;

		*err = calCtxCreateCounterExt(&cacheHitCounter,ctx,CAL_COUNTER_INPUT_CACHE_HIT_RATE);
		if(*err != CAL_RESULT_OK)		
			cacheHitCounter = 0;	
	}	

	modules = new Module*[NKernels];
	for(i = 0; i < NKernels; i++)
		modules[i] = NULL;
	
	this->hDev = hDev;
	this->kernels = kernels;	
	this->arrs = arrs;
	this->devInfo = devInfo;
	this->devAttribs = devAttribs;		
}

Context::~Context(void)
{
	long i;

	if(modules)
	{
		for(i = 0; i < NKernels; i++)
		{
			if(modules[i])
				delete modules[i];
		}
		delete modules;
	}

	if(expr)
		delete expr;

	if(idleCounter)
		calCtxDestroyCounterExt(ctx,idleCounter);

	if(cacheHitCounter)
		calCtxDestroyCounterExt(ctx,cacheHitCounter);	

	calCtxDestroy(ctx);
}

BOOL Context::InitCounterExtension(void)
{		
    if (calExtSupported((CALextid)CAL_EXT_COUNTERS) != CAL_RESULT_OK)    
        return FALSE;    
        
    if (calExtGetProc((CALextproc*)&calCtxCreateCounterExt, (CALextid)CAL_EXT_COUNTERS, "calCtxCreateCounter"))    
        return FALSE;    

    if (calExtGetProc((CALextproc*)&calCtxDestroyCounterExt, (CALextid)CAL_EXT_COUNTERS, "calCtxDestroyCounter"))
		return FALSE;
    
    if (calExtGetProc((CALextproc*)&calCtxBeginCounterExt, (CALextid)CAL_EXT_COUNTERS, "calCtxBeginCounter"))
		return FALSE;
    
    if (calExtGetProc((CALextproc*)&calCtxEndCounterExt, (CALextid)CAL_EXT_COUNTERS, "calCtxEndCounter"))
		return FALSE;

    if (calExtGetProc((CALextproc*)&calCtxGetCounterExt, (CALextid)CAL_EXT_COUNTERS, "calCtxGetCounter"))
		return FALSE;

	return TRUE;	
}

// set computation
CALresult Context::SetComputation(ArrayExpression* expr, Array* result, long priority, long flags)
{	
	CALresult err;	
	BOOL isReservedForGet0;
	long i;	

	// increment use counters beforehand!	
	for(i = 0; (i < 3) && expr->args[i]; i++){expr->args[i]->useCounter++;}	
	result->useCounter++;
	isReservedForGet0 = result->isReservedForGet;
	result->isReservedForGet = TRUE;

	switch(expr->op)
	{
		case OpIdentic:
			err = SetIdentic(expr,result);
			break;

		case OpAdd:
			err = SetElementwise(expr,result);
			break;

		case OpSub:
			err = SetElementwise(expr,result);
			break;

		case OpEwMul:
			err = SetElementwise(expr,result);
			break;

		case OpEwDiv:
			err = SetElementwise(expr,result);
			break;		

		case OpMul:
			if( (expr->args[0]->nDims == 2) && (expr->args[1]->nDims == 1) )			
				err = SetMatVecMul(expr,result);
			else if( (expr->args[0]->nDims == 2) && (expr->args[1]->nDims == 2) )	// matrix multiplication
			{
				err = SetMatMul(expr,result);
			}
			else	
				err = CAL_RESULT_NOT_SUPPORTED;
			break;

		case OpReshape:
			err = SetReshape(expr,result);
			break;

		case OpTranspose:
			err = SetTranspose(expr,result);
			break;

		default:
			err = CAL_RESULT_INVALID_PARAMETER;
	}

	if(err == CAL_RESULT_OK)		
	{
		if(this->expr)
			delete this->expr;

		this->expr = expr;
		this->result = result;
	}
	else	// in case of an error set use counters to their previous values		
	{			
		for(i = 0; (i < 3) && expr->args[i]; i++){expr->args[i]->useCounter--;}	
		result->useCounter--;
		result->isReservedForGet = isReservedForGet0;			
	}

	return err;
}

// start Idle counter
CALresult Context::StartIdleCounter(void)
{
	if(idleCounter)
		return calCtxBeginCounterExt(ctx,idleCounter);
	else
		return CAL_RESULT_NOT_SUPPORTED;
	
}

// start cache hit counter
CALresult Context::StartCacheHitCounter(void)
{
	if(cacheHitCounter)
		return calCtxBeginCounterExt(ctx,cacheHitCounter);
	else
		return CAL_RESULT_NOT_SUPPORTED;
}

// stop idle counter
CALresult Context::StopIdleCounter(void)
{
	if(idleCounter)
		return calCtxEndCounterExt(ctx,idleCounter);
	else
		return CAL_RESULT_NOT_SUPPORTED;
}

// stop cache hit counter
CALresult Context::StopCacheHitCounter(void)
{
	if(cacheHitCounter)
		return calCtxEndCounterExt(ctx,cacheHitCounter);
	else
		return CAL_RESULT_NOT_SUPPORTED;
}

// get idle counter value
CALresult Context::GetIdleCounter(float* counterVal)
{
	if(idleCounter)
		return calCtxGetCounterExt(counterVal,ctx,idleCounter);
	else
		return CAL_RESULT_NOT_SUPPORTED;
}

// get cache hit counter value
CALresult Context::GetCacheHitCounter(float* counterVal)
{
	if(cacheHitCounter)
		return calCtxGetCounterExt(counterVal,ctx,cacheHitCounter);
	else
		return CAL_RESULT_NOT_SUPPORTED;
}

ContextPool::ContextPool(void)
{
}

ContextPool::~ContextPool(void)
{
	RemoveAll();
}

Context* ContextPool::Get(long ind)
{
	return (Context*)ObjectPool::Get(ind);
}

void ContextPool::Remove(long ind)
{
	Context* context = Get(ind);
	if(context)
		delete context;
	
	ObjectPool::Remove(ind);
}

long ContextPool::FindUnused(void)
{		
	long i;	
	for(i = 0; (i < nObjs) && Get(i)->isUsed; i++);

	if(i < nObjs) 
		return i; 
	else 
		return -1;
}

// set an elementwise computation
CALresult Context::SetElementwise(ArrayExpression* expr, Array* result)
{
	CALresult err;	
	Array* arr, *arr1;
	long i, numParts;

	err = CAL_RESULT_OK;

	// check for the case when array is located on another device
	for(i = 0; (err == CAL_RESULT_OK) && (i < 2); i++)
	{
		if(expr->args[i]->hDev != hDev)
		{
			arr1 = expr->args[i];
			
			// create array copy on the local device
			arr = arrs->NewArray(arr1->arrID,arr1->dType,arr1->nDims,arr1->size,arr1->cpuData);						

			if(!arr1->parts)			
				err = arrs->AllocateArray(arr,0);			
			else
				err = arrs->AllocateSplittedMatrix(arr,arr1->numParts,0);

			if(err == CAL_RESULT_OK)
			{
				if( (err = arr1->Copy(ctx,arr)) == CAL_RESULT_OK )
				{
					expr->args[i] = arr;

					// add to the local pool as a copy
					arr->isCopy = TRUE;
					arrs->Add(arr);
				}
				else
					delete arr;
			}
			else
				delete arr;
		}
	}

	if(err != CAL_RESULT_OK)
		return err;

	numParts = max(expr->args[0]->numParts,expr->args[1]->numParts);

	for(i = 0; (err == CAL_RESULT_OK) && (i < 2) && expr->args[i]; i++)
	{				
		if(!numParts) // no splitted matrices within the arguments
		{
			if(!expr->args[i]->res)	// array does not reside in the memory
			{
				// allocate array and set data
				if( (err = arrs->AllocateArray(expr->args[i],0)) == CAL_RESULT_OK )
					err = expr->args[i]->SetData(ctx,expr->args[i]->cpuData);
			}			
		}
		else
		{
			if(!expr->args[i]->res && !expr->args[i]->parts)	// array does not reside in the memory
			{
				// allocate splitted matrix and set data
				if( (err = arrs->AllocateSplittedMatrix(expr->args[i],numParts,0)) == CAL_RESULT_OK )
					err = expr->args[i]->SetData(ctx,expr->args[i]->cpuData);
			}
			else if(expr->args[i]->res)	// if array resides in the memory as a solid 2D piece
			{
				if( (err = arrs->AllocateSplittedMatrix(expr->args[i],numParts,0)) == CAL_RESULT_OK )
				{
					if( (err = SplitMatrix(expr->args[i],numParts,expr->args[i]->parts)) == CAL_RESULT_OK )
					{
						calResFree(expr->args[i]->res);
						expr->args[i]->res = 0;
					}
				}
			}
		}
	}

	if(err != CAL_RESULT_OK)
		return err;
	
	if(!numParts) // no splitted matrices within the arguments
	{			
		if(!result->res)	
		{
			result->Free();
			err = ((ArrayPool*)result->pool)->AllocateArray(result,0);			
		}
	}
	else if(!result->parts)	// if array resides in the memory as a solid 2D memory piece
	{
		result->Free();
		err = ((ArrayPool*)result->pool)->AllocateSplittedMatrix(result,numParts,0);
	}

	if(err != CAL_RESULT_OK)
		return err;

	if(result->hDev != hDev)	// if array resides on another device
	{
		resultTemp = arrs->NewArray(result->arrID,result->dType,result->nDims,result->size,NULL);
		if(!result->parts)
			err = arrs->AllocateArray(resultTemp,0);
		else
			err = arrs->AllocateSplittedMatrix(resultTemp,result->numParts,0);		

		if(err != CAL_RESULT_OK)		
		{
			delete resultTemp;
			resultTemp = NULL;
		}
	}
	
	return err;
}

// split a matrix into given number of parts, convenient for matrix multiplication
CALresult Context::SplitMatrix(Array* arr, long numParts, Array** parts)
{	
	CALresult err;
	CALdomain domain;	
	KernelCode iKernel;		

	switch(numParts)
	{
		case 4: iKernel = KernSplitMatrixTo4Parts_PS;
			break;

		case 8: iKernel = KernSplitMatrixTo8Parts_PS;
			break;
		
		default:
			return CAL_RESULT_INVALID_PARAMETER;
	}	

	err = CAL_RESULT_OK;
	
	// get suited module
	if(!modules[iKernel])
	{
		modules[iKernel] = new Module(hDev,ctx,kernels[iKernel],&err);	
		if(err != CAL_RESULT_OK)
		{
			delete modules[iKernel];
			modules[iKernel] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{
		// set the domain of execution
		domain.x = 0;
		domain.y = 0;		
		domain.width = parts[0]->physSize[1];
		domain.height = parts[0]->physSize[0];		

		// run the program				
		err = modules[iKernel]->RunPixelShader(&arr,parts,NULL,&domain);
	}

	return err;
}

// perform a computation already set by SetComputation
CALresult Context::DoComputation(void)
{
	CALresult err;
	long i;

	if(!expr)	
		return CAL_RESULT_ERROR;

	switch(expr->op)
	{
		case OpIdentic:
			err = DoIdentic();
			break;

		case OpAdd:
			err = DoElementwise();			
			break;

		case OpSub:
			err = DoElementwise();
			break;

		case OpEwMul:
			err = DoElementwise();
			break;

		case OpEwDiv:
			err = DoElementwise();
			break;

		case OpMul:
			if( (expr->args[0]->nDims == 2) && (expr->args[1]->nDims == 1) )
			{
				if(expr->args[0]->res)
					err = DoMatVecMul();
				else
					err = DoMatVecMulSplitted();
			}			
			else if( (expr->args[0]->nDims == 2) && (expr->args[1]->nDims == 2) )
				err = DoMatMul();

			break;

		case OpReshape:
			err = DoReshape();
			break;

		case OpTranspose:
			err = DoTranspose();
			break;

		default:
			err = CAL_RESULT_INVALID_PARAMETER;
	}
	
	// decrement use counters
	if(err == CAL_RESULT_OK)
	{		
		for(i = 0; (i < 3) && (expr->args[i]); i++){expr->args[i]->useCounter--;}	
		result->useCounter--;
	}

	return err;
}

// perform an elementwise operation
CALresult Context::DoElementwise(void)
{	
	CALresult err;
	Module* module;
	CALdomain domain;	
	Array* parts[2];
	Array* arr;

	long i;
	KernelCode iKernel;
	
	err = CAL_RESULT_OK;

	switch(expr->dType)
	{
		case TREAL:
		{
			switch(expr->op)
			{
				case OpAdd: iKernel = KernAddR_PS; break;
				case OpSub: iKernel = KernSubR_PS; break;
				case OpEwMul: iKernel = KernEwMulR_PS; break;
				case OpEwDiv: iKernel = KernEwDivR_PS; break;

				default:
					return CAL_RESULT_INVALID_PARAMETER;
			}			

		}break;

		case TLONGREAL:
		{
			switch(expr->op)
			{
				case OpAdd: iKernel = KernAddLR_PS; break;
				case OpSub: iKernel = KernSubLR_PS; break;
				case OpEwMul: iKernel = KernEwMulLR_PS; break;
				case OpEwDiv: iKernel = KernEwDivLR_PS; break;

				default:
					return CAL_RESULT_INVALID_PARAMETER;
			}			

		}break;
		
		default:
			return CAL_RESULT_INVALID_PARAMETER;
	}	
	
	// get suited module
	if(!modules[iKernel])
	{
		modules[iKernel] = new Module(hDev,ctx,kernels[iKernel],&err);		
		if(err != CAL_RESULT_OK)
		{
			delete modules[iKernel];
			modules[iKernel] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{				
		module = modules[iKernel];

		// set the domain of execution
		domain.x = 0;
		domain.y = 0;

		if(!resultTemp)
			arr = result;
		else
			arr = resultTemp;

		if(!arr->parts)
		{
			domain.width = arr->physSize[1];
			domain.height = arr->physSize[0];

			// run the program			
			err = module->RunPixelShader(expr->args,&arr,NULL,&domain);		
		}
		else
		{
			domain.width = arr->parts[0]->physSize[1];
			domain.height = arr->parts[0]->physSize[0];			
			
			// run the program for each part separately
			for(i = 0; i < arr->numParts; i++)
			{
				parts[0] = expr->args[0]->parts[i];
				if(expr->args[1])
					parts[1] = expr->args[1]->parts[i];

				err = module->RunPixelShader(parts,&arr->parts[i],NULL,&domain);
			}			
		}

		if(resultTemp)
		{
			if( (err == CAL_RESULT_OK) && ((err = resultTemp->Copy(ctx,result)) == CAL_RESULT_OK) )
			{
				// add temp result as a copy of actual result
				resultTemp->isCopy = TRUE;
				arrs->Add(resultTemp);				
				resultTemp = NULL;
			}							
		}

	}	

	if(resultTemp)
	{
		delete resultTemp;
		resultTemp = NULL;
	}

	return err;
}

// set a matrix vector multiply computation
CALresult Context::SetMatVecMul(ArrayExpression* expr, Array* result)
{
	return CAL_RESULT_NOT_SUPPORTED;
/*
	CALresult err;		

	err = CAL_RESULT_OK;	

	if(!expr->args[0]->res && !expr->args[0]->parts)
	{
		// allocate array and set data
		if( (err = arrs->AllocateArray(expr->args[0],0)) == CAL_RESULT_OK )
			err = expr->args[0]->SetData(ctx,expr->args[0]->cpuData);
	}

	if(err != CAL_RESULT_OK)
		return err;

	if(!expr->args[1]->res)
	{
		// allocate array and set data
		if( (err = arrs->AllocateArray(expr->args[1],0)) == CAL_RESULT_OK )
			err = expr->args[1]->SetData(ctx,expr->args[1]->cpuData);
	}
	
	if(err != CAL_RESULT_OK)
		return err;
	
	if(result == expr->args[1])
	{
		// result is within input arguments - create temporary result array
		_ASSERT(!resultTemp);

		resultTemp = new Array(result->hDev,result->devInfo,result->devAttribs,result->arrID,result->dType,result->nDims,result->size);
		if( (err = arrs->AllocateArray(resultTemp,0)) != CAL_RESULT_OK )
		{
			delete resultTemp;
			resultTemp = NULL;
		}
		
	}
	else if(!result->res || !EqualSizes(result->nDims,result->size,expr->nDims,expr->size))
	{
		result->Free();
		err = arrs->AllocateArray(result,0);
	}

	return err;
*/
}

// perform a matrix vector multiplication
CALresult Context::DoMatVecMul(void)
{
	return CAL_RESULT_NOT_SUPPORTED;
/*
	CALresult err;
	Module* module;
	CALdomain domain;
	KernelCode iKernel;	
	float constData[4];	
	
	err = CAL_RESULT_OK;

	switch(expr->dType)
	{
		case TREAL:
		{
			iKernel = KernMatVecR_PS; break;
		}break;
		
		default:
			return CAL_RESULT_INVALID_PARAMETER;
	}	
	
	// get suited module
	if(!modules[iKernel])
	{
		modules[iKernel] = new Module(hDev,ctx,kernels[iKernel],&err);		
		if(err != CAL_RESULT_OK)
		{
			delete modules[iKernel];
			modules[iKernel] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{		
		module = modules[iKernel];

		constData[0] = (float)(expr->args[0]->physSize[1]);	// matrix width		

		err = module->constants[0]->SetData(&constData);		
		if(err == CAL_RESULT_OK)
		{
			err = module->SetConstantsToContext();
			if(err == CAL_RESULT_OK)
			{
				// set the domain of execution
				domain.x = 0;
				domain.y = 0;		
				domain.width = result->physSize[1];
				domain.height = 1;								

				if(!resultTemp)
					err = module->RunPixelShader(expr->args,&result,NULL,&domain);
				else
				{
					if( (err = module->RunPixelShader(expr->args,&resultTemp,NULL,&domain)) == CAL_RESULT_OK )
					{							
						delete result;
						arrs->Set(arrs->Find(resultTemp->arrID),resultTemp);						
						result = resultTemp;
						resultTemp = NULL;
					}
				}								

				module->ReleaseConstantsFromContext();
			}
		}		
	}

	if(resultTemp)
	{
		delete resultTemp;
		resultTemp = NULL;
	}

	return err;
*/
}

// perform matrix vector multiplication for the case when matrix is splitted into parts
CALresult Context::DoMatVecMulSplitted(void)
{
	return CAL_RESULT_NOT_SUPPORTED;
/*
	CALresult err;
	Module* module;
	CALdomain domain;
	KernelCode iKernel;	
	Array* inputs[9];
	float constData[4];	
	long i;
	
	err = CAL_RESULT_OK;

	switch(expr->args[0]->numParts)
	{
		case 4:

			switch(expr->dType)
			{
				case TREAL:
				{
					iKernel = KernMatVec4PartsR_PS; break;
				}break;
				
				default:
					return CAL_RESULT_INVALID_PARAMETER;
			}
			break;

		case 8:

			switch(expr->dType)
			{
				case TREAL:
				{
					iKernel = KernMatVec8PartsR_PS; break;
				}break;
				
				default:
					return CAL_RESULT_INVALID_PARAMETER;
			}
			break;
	}
	
	// get suited module
	if(!modules[iKernel])
	{
		modules[iKernel] = new Module(hDev,ctx,kernels[iKernel],&err);		
		if(err != CAL_RESULT_OK)
		{
			delete modules[iKernel];
			modules[iKernel] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{		
		module = modules[iKernel];

		constData[0] = (float)(expr->args[0]->physSize[1]);	// matrix width		

		err = module->constants[0]->SetData(&constData);		
		if(err == CAL_RESULT_OK)
		{
			err = module->SetConstantsToContext();
			if(err == CAL_RESULT_OK)
			{
				// set the domain of execution
				domain.x = 0;
				domain.y = 0;		
				domain.width = result->physSize[1];
				domain.height = 1;	

				for(i = 0; i < expr->args[0]->numParts; i++)
					inputs[i] = expr->args[0]->parts[i];
				inputs[i] = expr->args[1];

				if(!resultTemp)
				{
					err = module->RunPixelShader(inputs,&result,NULL,&domain);
				}
				else
				{
					if( (err = module->RunPixelShader(inputs,&resultTemp,NULL,&domain)) == CAL_RESULT_OK )
					{						
						delete result;
						arrs->Set(arrs->Find(resultTemp->arrID),resultTemp);						
						result = resultTemp;
						resultTemp = NULL;
					}
				}								

				module->ReleaseConstantsFromContext();
			}
		}		
	}	

	if(resultTemp)
	{
		delete resultTemp;
		resultTemp = NULL;
	}

	return err;
*/
}

// set a matrix multiplication computation
CALresult Context::SetMatMul(ArrayExpression* expr, Array* result)
{
	CALresult err;
	Array* arr, *arr1;
	long i, j;

	err = CAL_RESULT_OK;

	if(result->isVirtualized || expr->args[0]->isVirtualized || expr->args[1]->isVirtualized)
		return CAL_RESULT_NOT_SUPPORTED;

	// check for the case when array is located on another device
	for(i = 0; (err == CAL_RESULT_OK) && (i < 2); i++)
	{
		if(expr->args[i]->hDev != hDev)
		{
			arr1 = expr->args[i];
			
			// create array copy on the local device
			arr = arrs->NewArray(arr1->arrID,arr1->dType,arr1->nDims,arr1->size,arr1->cpuData);						

			if(!arr1->parts)			
				err = arrs->AllocateArray(arr,0);			
			else
				err = arrs->AllocateSplittedMatrix(arr,arr1->numParts,0);

			if(err == CAL_RESULT_OK)
			{
				if( (err = arr1->Copy(ctx,arr)) == CAL_RESULT_OK )
				{
					expr->args[i] = arr;

					// add to the local pool as a copy
					arr->isCopy = TRUE;
					arrs->Add(arr);
				}
				else
					delete arr;
			}
			else
				delete arr;
		}
	}

	if(err != CAL_RESULT_OK)
		return err;

	// set array data if necessary
	for(i = 0; (i < 2) && (err == CAL_RESULT_OK); i++)
	{
		if(!expr->args[i]->res && !expr->args[i]->parts)
		{
			// allocate array and set data
			if(expr->args[i]->size[0] >= 8)		
				err = arrs->AllocateSplittedMatrix(expr->args[i],8,0);						
			else
				err = arrs->AllocateSplittedMatrix(expr->args[i],4,0);
	
			if(err == CAL_RESULT_OK)
				err = expr->args[i]->SetData(ctx,expr->args[i]->cpuData);	
		}
		else if(expr->args[i]->res)	// requires splitting
		{
			if(expr->args[i]->size[0] >= 8)		
				err = arrs->AllocateSplittedMatrix(expr->args[i],8,0);						
			else
				err = arrs->AllocateSplittedMatrix(expr->args[i],4,0);
	
			if(err == CAL_RESULT_OK)
			{
				err = SplitMatrix(expr->args[i],8,expr->args[i]->parts);
				if(err == CAL_RESULT_OK)
				{
					calResFree(expr->args[i]->res);
					expr->args[i]->res = 0;
				}
				else // do cleanup
				{
					for(j = 0; j < expr->args[i]->numParts; j++)
						delete expr->args[i]->parts[j];

					expr->args[i]->parts = NULL;
					expr->args[i]->numParts = 0;
				}
			}
		}
	}

	if(err != CAL_RESULT_OK)
		return err;

	if( (result == expr->args[0]) || (result == expr->args[1]) )
	{
		// result is within input arguments -> create temporary result array
		resultTemp = arrs->NewArray(result->arrID,result->dType,result->nDims,result->size,result->cpuData);
		err = arrs->AllocateSplittedMatrix(resultTemp,expr->args[0]->numParts,0);

		if(err != CAL_RESULT_OK)
		{
			delete resultTemp;
			resultTemp = NULL;
		}
	}
	else if(!result->parts)
	{
		result->Free();
		err = ((ArrayPool*)result->pool)->AllocateSplittedMatrix(result,expr->args[0]->numParts,0);
	}

	if(result->hDev != hDev)	// if array resides on another device
	{
		resultTemp = arrs->NewArray(result->arrID,result->dType,result->nDims,result->size,NULL);		
		err = arrs->AllocateSplittedMatrix(resultTemp,result->numParts,0);		

		if(err != CAL_RESULT_OK)		
		{
			delete resultTemp;
			resultTemp = NULL;
		}
	}

	return err;
}

// perform a matrix multiplication computation
CALresult Context::DoMatMul(void)
{
	CALresult err;
	Module* module;
	CALdomain domain;
	KernelCode iKernel;	
	Array* inputs[16];
	Array* arr;
	float constData[4];	
	long i;
	
	err = CAL_RESULT_OK;

	if( (expr->args[0]->numParts == 8) && (expr->args[1]->numParts == 8) )
	{
		switch(expr->args[0]->dType)
		{
			case TREAL: iKernel = KernMatMul88Parts8x4by4x4R_PS; break;

			default:
				return CAL_RESULT_NOT_SUPPORTED;
		}
	}
	else
		return CAL_RESULT_NOT_SUPPORTED;
	
	// get suited module
	if(!modules[iKernel])
	{
		modules[iKernel] = new Module(hDev,ctx,kernels[iKernel],&err);		
		if(err != CAL_RESULT_OK)
		{
			delete modules[iKernel];
			modules[iKernel] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{		
		module = modules[iKernel];

		constData[0] = (float)(expr->args[0]->physSize[1]);	// matrix width		

		err = module->constants[0]->SetData(&constData);		
		if(err == CAL_RESULT_OK)
		{
			err = module->SetConstantsToContext();
			if(err == CAL_RESULT_OK)
			{
				if(!resultTemp)
					arr = result;
				else
					arr = resultTemp;

				// set the domain of execution
				domain.x = 0;
				domain.y = 0;		
				domain.width = arr->parts[0]->physSize[1];
				domain.height = arr->parts[0]->physSize[0];	
				
				for(i = 0; i < expr->args[0]->numParts; i++)				
					inputs[i] = expr->args[0]->parts[i];
				for(i = 0; i < expr->args[1]->numParts; i++)
					inputs[i+expr->args[0]->numParts] = expr->args[1]->parts[i];
				
				err = module->RunPixelShader(inputs,arr->parts,NULL,&domain);

				if(resultTemp)
				{
					if(err == CAL_RESULT_OK)
					{
						if(result->hDev == hDev)
						{							
							arrs->Set(arrs->Find(result->arrID),resultTemp);
							delete result;
							result = resultTemp;
							resultTemp = NULL;

							// do not forget about the flags!
							result->useCounter++;
							result->isReservedForGet = TRUE;
						}
						else if( (err = resultTemp->Copy(ctx,result)) == CAL_RESULT_OK )
						{
							// add temp result as a copy of actual result
							resultTemp->isCopy = TRUE;
							arrs->Add(resultTemp);				
							resultTemp = NULL;
						}						
					}					
				}

				module->ReleaseConstantsFromContext();
			}
		}		
	}	

	if(resultTemp)
	{
		delete resultTemp;
		resultTemp = NULL;
	}

	return err;
}

// set a reshape computation
CALresult Context::SetReshape(ArrayExpression* expr, Array* result)
{
	return CAL_RESULT_NOT_SUPPORTED;
/*
	CALresult err;		

	err = CAL_RESULT_OK;
		
	if(!expr->args[0]->res && !expr->args[0]->parts)
	{
		if( (err = arrs->AllocateArray(expr->args[0],0)) == CAL_RESULT_OK )
			err = expr->args[0]->SetData(ctx,expr->args[0]->cpuData);
	}
	else if(expr->args[0]->parts)
	{
		err = expr->args[0]->SetData(ctx,expr->args[0]->cpuData);
	}

	if(err != CAL_RESULT_OK)
		return err;	

	if(result == expr->args[0])
	{
		resultTemp = new Array(result->hDev,result->devInfo,result->devAttribs,result->arrID,result->dType,expr->nDims,expr->size);
		if(!resultTemp->isVirtualized)
			err = arrs->AllocateArray(resultTemp,0);
	}
	else if( !result->res || result->parts || !EqualSizes(result->nDims,result->size,expr->nDims,expr->size) )
	{
		result->Free();
		err = arrs->AllocateArray(result,0);
	}

	return err;
*/
}

// perform a reshape computation
CALresult Context::DoReshape(void)
{
	return CAL_RESULT_NOT_SUPPORTED;
/*
	CALresult err;
	KernelCode iKernel;
	Module* module;
	CALdomain domain;		
	long constData[4];
	Array* arr;
	Array** inputs;

	err = CAL_RESULT_OK;

	if(result != expr->args[0])
		arr = result;
	else
		arr = resultTemp;

	inputs = expr->args;

	if(expr->args[0]->isVirtualized && arr->isVirtualized)
	{
		// no explicit reshape is required!

		if(!resultTemp)
			err = ResCopy(ctx,result->res,expr->args[0]->res);
		else
		{				
			resultTemp->res = result->res;
			result->res = 0;
			delete result;

			arrs->Set(arrs->Find(resultTemp->arrID),resultTemp);			
			result = resultTemp;
			resultTemp = NULL;
		}

		return err;
	}
	else if(!expr->args[0]->isVirtualized && !arr->isVirtualized)
	{
		if( !(expr->args[0]->size[1] % expr->args[0]->physNumComponents) && !(arr->size[arr->nDims-1] % arr->physNumComponents) )
		{			
			iKernel = KernReshapeMatToMatNoBounds_PS;
			constData[0] = expr->args[0]->physSize[1];	// A.physWidth
			constData[1] = arr->physSize[1];			// C.physWidth						
		}
		else
			return CAL_RESULT_NOT_SUPPORTED;
	}
	else if(expr->args[0]->isVirtualized && !arr->isVirtualized)
	{
		if(expr->args[0]->elemSize == 4)
		{
			iKernel = KernReshapeArr1DWToMat4DW_PS;
			constData[0] = expr->args[0]->physSize[1];	// A.physWidth					
			constData[1] = arr->physSize[1];			// C.physWidth
			constData[2] = arr->size[1];				// C.width			
		}
		else
			return CAL_RESULT_NOT_SUPPORTED;
	}
	else if(!expr->args[0]->isVirtualized && arr->isVirtualized)
	{		
		if(arr->elemSize == 4)
		{			
			if(expr->args[0]->numParts == 0)		
				iKernel = KernReshapeMat4DWToArr1DW_PS;			
			else if(expr->args[0]->numParts == 4)
			{				
				iKernel = KernReshapeMat4Parts4DWToArr1DW_PS;
				inputs = expr->args[0]->parts;
			}
			else if(expr->args[0]->numParts == 8)
			{
				iKernel = KernReshapeMat8Parts4DWToArr1DW_PS;
				inputs = expr->args[0]->parts;
			}
			else
				return CAL_RESULT_ERROR;

			constData[0] = arr->physSize[1];		// C.physWidth
			constData[1] = expr->args[0]->size[1];	// A.Width
		}
		else
			return CAL_RESULT_NOT_SUPPORTED;		
	}
	else
		return CAL_RESULT_NOT_SUPPORTED;

	// get suited module
	if(!modules[iKernel])
	{
		modules[iKernel] = new Module(hDev,ctx,kernels[iKernel],&err);		
		if(err != CAL_RESULT_OK)
		{
			delete modules[iKernel];
			modules[iKernel] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{		
		module = modules[iKernel];		

		err = module->constants[0]->SetData(&constData);		
		if(err == CAL_RESULT_OK)
		{
			err = module->SetConstantsToContext();
			if(err == CAL_RESULT_OK)
			{	
				// set the domain of execution
				domain.x = 0;
				domain.y = 0;		
				domain.width = arr->physSize[1];
				domain.height = arr->physSize[0];

				err = module->RunPixelShader(inputs,&arr,NULL,&domain);

				if( (err == CAL_RESULT_OK) && resultTemp )
				{					
					resultTemp->res = result->res;
					result->res = 0;
					delete result;

					arrs->Set(arrs->Find(resultTemp->arrID),resultTemp);					
					result = resultTemp;
					resultTemp = NULL;
				}
				
				module->ReleaseConstantsFromContext();
			}
		}		
	}	

	if(resultTemp)
	{
		delete resultTemp;
		resultTemp = NULL;
	}		

	return err;
*/
}


// zero array memory
CALresult Context::ZeroArrayMemory(Array* arr, CALdomain* domain)
{
	CALresult err;	
	
	err = CAL_RESULT_OK;	
	
	// get suited module
	if(!modules[KernZeroMemory_PS])
	{
		modules[KernZeroMemory_PS] = new Module(hDev,ctx,kernels[KernZeroMemory_PS],&err);		
		if(err != CAL_RESULT_OK)
		{
			delete modules[KernZeroMemory_PS];
			modules[KernZeroMemory_PS] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{
		// run the program			
		err = modules[KernZeroMemory_PS]->RunPixelShader(NULL,&arr,NULL,domain);				
	}	

	return err;
}

// set a transpose computation
CALresult Context::SetTranspose(ArrayExpression* expr, Array* result)
{
	return CAL_RESULT_NOT_SUPPORTED;
/*
	CALresult err;	

	err = CAL_RESULT_OK;

	if(expr->args[0]->parts || result->parts)
		return CAL_RESULT_NOT_SUPPORTED;

		
	if(!expr->args[0]->res)
	{
		if( (err = arrs->AllocateArray(expr->args[0],0)) == CAL_RESULT_OK )
			err = expr->args[0]->SetData(ctx,expr->args[0]->cpuData);
	}	

	if(err != CAL_RESULT_OK)
		return err;	

	if(result == expr->args[0])
	{
		resultTemp = new Array(result->hDev,result->devInfo,result->devAttribs,result->arrID,result->dType,expr->nDims,expr->size);		
		err = arrs->AllocateArray(resultTemp,0);
	}
	else if(!result->res || !EqualSizes(result->nDims,result->size,expr->nDims,expr->size) )
	{
		result->Free();
		err = arrs->AllocateArray(result,0);
	}

	return err;
*/
}
// perform a transpose computation
CALresult Context::DoTranspose(void)
{
	return CAL_RESULT_NOT_SUPPORTED;
/*
	CALresult err;
	KernelCode iKernel;
	Module* module;
	CALdomain domain;		
	long i, constData[12];
	Array* arr;	

	err = CAL_RESULT_OK;

	if(result != expr->args[0])
		arr = result;
	else
		arr = resultTemp;

	if(expr->args[0]->isVirtualized && arr->isVirtualized)
	{		
		if(result->nDims == 3)
		{
			iKernel = KernTranspose3D_PS;

			constData[0] = result->physSize[1];				// physWidth
			constData[1] = result->size[1]*result->size[2];	// C.Ny*C.Nx
			constData[2] = result->size[2];					// C.Nx
			constData[3] = 1;

			constData[4] = expr->transpDims[0];
			constData[5] = expr->transpDims[1];
			constData[6] = expr->transpDims[2];
			constData[7] = 0;

			constData[8] = expr->args[0]->size[1]*expr->args[0]->size[2];	// A.Ny*A.Nx
			constData[9] = expr->args[0]->size[2];							// A.Nx			
			constData[10] = 1;												// 1
		}
		else if(result->nDims == 4)
		{
			iKernel = KernTranspose4D_PS;

			constData[0] = result->physSize[1];								// physWidth
			constData[1] = result->size[1]*result->size[2]*result->size[3];	// C.Nz*C.Ny*C.Nx
			constData[2] = result->size[2]*result->size[3];					// C.Ny*C.Nx
			constData[3] = result->size[3];									// C.Nx			

			constData[4] = expr->transpDims[0];
			constData[5] = expr->transpDims[1];
			constData[6] = expr->transpDims[2];
			constData[7] = expr->transpDims[3];;
			
			constData[8] = expr->args[0]->size[1]*expr->args[0]->size[2]*expr->args[0]->size[3];	// A.Nz*A.Ny*A.Nx
			constData[9] = expr->args[0]->size[2]*expr->args[0]->size[3];							// A.Ny*A.Nx
			constData[10] = expr->args[0]->size[3];													// A.Nx			
			constData[11] = 1;																		// 1
		}
		else
			return CAL_RESULT_NOT_SUPPORTED;		
	}
	else if(!expr->args[0]->isVirtualized && !arr->isVirtualized)
	{	
		if( (expr->transpDims[0] == 1) && (expr->transpDims[1] == 0) )
		{
			if(!expr->args[0]->parts && !arr->parts)
				iKernel = KernTransposeMat4DW_PS;
			else
				return CAL_RESULT_NOT_SUPPORTED;
		}
		else	// just copy the data
		{
			if(!expr->args[0]->parts && !arr->parts)
				err = ResCopy(ctx,arr->res,expr->args[0]->res);
			else if(expr->args[0]->parts && arr->parts)
			{				
				for(i = 0; (i < arr->numParts) && (err == CAL_RESULT_OK); i++)
					err = ResCopy(ctx,arr->parts[i]->res,expr->args[0]->parts[i]->res);
			}
			else
				return CAL_RESULT_NOT_SUPPORTED;

			return err;
		}
	}	
	else
		return CAL_RESULT_INVALID_PARAMETER;

	// get suited module
	if(!modules[iKernel])
	{
		modules[iKernel] = new Module(hDev,ctx,kernels[iKernel],&err);		
		if(err != CAL_RESULT_OK)
		{
			delete modules[iKernel];
			modules[iKernel] = NULL;
		}
	}
	
	if(err == CAL_RESULT_OK)
	{		
		module = modules[iKernel];		
		
		if(module->nConstants)
			err = module->constants[0]->SetData(&constData);		
		if(err == CAL_RESULT_OK)
		{
			if(module->nConstants)
				err = module->SetConstantsToContext();
			if(err == CAL_RESULT_OK)
			{	
				// set the domain of execution
				domain.x = 0;
				domain.y = 0;		
				domain.width = arr->physSize[1];
				domain.height = arr->physSize[0];

				err = module->RunPixelShader(expr->args,&arr,NULL,&domain);

				if( (err == CAL_RESULT_OK) && resultTemp )
				{					
					resultTemp->res = result->res;
					result->res = 0;
					delete result;

					arrs->Set(arrs->Find(resultTemp->arrID),resultTemp);					
					result = resultTemp;
					resultTemp = NULL;
				}
				
				module->ReleaseConstantsFromContext();
			}
		}		
	}	

	if(resultTemp)
	{
		delete resultTemp;
		resultTemp = NULL;
	}		

	return err;
*/
}


// set an identic operation
CALresult Context::SetIdentic(ArrayExpression* expr, Array* result)
{
	CALresult err;

	err = CAL_RESULT_OK;

	if(!expr->args[0]->IsScalar())
	{
		if(expr->args[0] != result)
		{
			if(!expr->args[0]->parts) // src is not a splitted matrix
			{
				if(!result->res)
				{
					result->Free();
					err = ((ArrayPool*)result->pool)->AllocateArray(result,0);
				}
			}
			else // src is a splitted matrix
			{
				if(!result->parts)
				{
					result->Free();
					err = ((ArrayPool*)result->pool)->AllocateSplittedMatrix(result,expr->args[0]->numParts,0);
				}
			}
		}
	}
	else
		return CAL_RESULT_NOT_SUPPORTED;

	return err;
}

// do an identic operation
CALresult Context::DoIdentic(void)
{
	CALresult err;	

	err = CAL_RESULT_OK;
	
	if(expr->args[0] != result)
	{
		if(!expr->args[0]->IsScalar())
		{
			if(expr->args[0]->res || expr->args[0]->parts)
				err = expr->args[0]->Copy(ctx,result);
			else
				err = result->SetData(ctx,expr->args[0]->cpuData);
		}
		else
		{
			return CAL_RESULT_NOT_SUPPORTED;
		}			
	}

	return err;
}