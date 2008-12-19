#pragma once
#include "ObjectPool.h"

class Array
{
public:	
	Array(CALdevice hDev, CALdeviceinfo* devInfo, CALdeviceattribs* devAttribs, long arrID, long dType, long numComponents, long nDims, long* size);	
	~Array(void);

	// free array resource
	void Free(void);
	// allocate array resource
	CALresult Allocate(CALuint flags);
	// copy data from one resource to another
	CALresult Copy(CALcontext ctx, CALresource dstRes, CALresource srcRes);
	// sets data to GPU memory
	CALresult SetData(CALcontext ctx, void* cpuData);
	// gets data from GPU memory
	CALresult GetData(CALcontext ctx, void* cpuData);
	// Set data to a resource
	CALresult SetDataToRes(CALresource res, void* cpuData);
	// Set data part to a resource
	CALresult SetDataPartToRes(CALresource res, void* cpuData, long iPart);
	// Get data from a resource
	CALresult GetDataFromRes(CALresource res, void* cpuData);	
	// Get data part from a resource
	CALresult GetDataPartFromRes(CALresource res, void* cpuData, long iPart);

	// get named local memory handle for given context
	CALresult GetNamedLocalMem(CALcontext ctx, CALname name, CALmem* mem);

	CALdevice hDev;			// handle of device on which the array exists	
	CALdeviceinfo* devInfo;
	CALdeviceattribs* devAttribs;

	long arrID;				// array ID		
	long dType;				// data type code		
	long nDims;				// number of dimensions
	long* size;				// array size

	long numElements;		// total number of elements
	long elemSize;			// element size in bytes
	long dataSize;			// total data size in bytes
	
	CALformat dFormat;		// data format on the GPU		
	long physNumComponents;	// number of components in each element on the GPU	
	long physNumElements;	// total number of physical multicomponent elements
	long physElemSize;		// physical element size (here an element can be multicomponent)	
	long physDataSize;		// physical data size in bytes
	long physPitch;			// GPU alignment pitch for data row

	long physSize[2];		// physical size on the GPU with account of padding to multiple of physNumComponents

	void* cpuData;			// CPU data pointer
	
	BOOL isGlobalBuf;		// TRUE when array is allocated as a global buffer
	BOOL isVirtualized;		// memory virtualization is used to fit to the hardware requirements
	long useCounter;		// counter for storing number of contextes which use the array
	BOOL isReservedForGet;	// the object is reserved for further getting (for example a return argument)

	CALresource res;		// CAL resource	

	long numParts;			// number of parts (in case of matrices)
	Array** parts;			// matrix parts (to speed up matrix multiplication)
};

class ArrayPool :
	public ObjectPool
{
public:
	ArrayPool(void);
	~ArrayPool(void);

	Array* Get(long ind);
	long Find(long arrID);
	void Remove(long ind);
	// allocate an array
	CALresult AllocateArray(Array* arr, CALuint flags);	
	// allocated a matrix splitted in given number of parts parts
	CALresult AllocateSplittedMatrix(Array* arr, long numParts, CALuint flags);
};


// array expression for internal use
class ArrayExpression
{
public:
	ArrayExpression(long op, long dType, long nDims, long* size, long* transpDims);
	~ArrayExpression(void);

	long op;			// operation code
	long dType;			// data type code
	long nDims;			// number of dimensions of expression result
	long* size;			// size of expression result
	long* transpDims;	// transposed dimensions in case of transposition operation
	Array* args[3];		// expression arguments
};
