#pragma once
#include "ObjectPool.h"

#define GBufBurstSize						64	// global buffer burst size in bytes
#define MinFillPhysNumElements_PS			64	// minimal data size in bytes used for filling memory using pixel shader kernels

enum ShaderType
{
	CS,	// compute shader
	PS	// pixel shader
};

/*
 REAL kernels
*/

// data filling kernels
enum KernelCode
{
KernFill_PS,	// pixel shader for filling memory by physNumComponents elements per running kernel
KernFill1_PS,	// the same as KernFill_PS but with account of that the size is not multiple of physNumComponents

// addition
KernAddR_PS,
KernAddLR_PS,

// subtraction
KernSubR_PS,
KernSubLR_PS,

// elementwise multiply
KernEwMulR_PS,
KernEwMulLR_PS,

// elementwise divide
KernEwDivR_PS,
KernEwDivLR_PS,

// matrix vector multiply
KernMatVecR_PS,

// divide a matrix into parts
KernDivideMatrixTo4Parts_PS,
KernDivideMatrixTo8Parts_PS,

KernMatMul4x8x4by4x4x4R_CS,
KernMatMul8x4by4x4R_CS,
KernMatMul4x4by4x4R_CS,
KernMatMul4x8x4by4x4x4R_PS,
KernMatMul4x4by4x4R_PS,
KernMatMul8x4by4x4R_PS,

NKernels	// total number of kernels
};

// simplest pixel shader for filling multicomponent memory 
// by a given value passed as a constant 
const char kernelFill_PS[] =
"il_ps_2_0\n"
"dcl_cb cb0[1]\n"
"dcl_output_generic o0\n"
"mov o0, cb0[0]\n"
"end\n";

// the same as KernFill_PS but with account of that the data 
// size is not multiple of physNumComponents
const char kernelFill1_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[2]\n"
"dcl_cb cb1[1]\n"
"dcl_output_generic o0\n"

"lt r0.y, vWinCoord0.x, cb1[0].x\n"
"if_logicalnz r0.y\n"	// if elemIndex <= lastIndex which is not multiple of physNumComponents
"	mov o0, cb0[0]\n"
"else\n"
"	mov o0, cb0[1]\n"
"endif\n"

"end\n";

const char kernelAddR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"add o0, r0, r1\n"
"end\n";

const char kernelAddLR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"dadd o0.xy, r0.xy, r1.xy\n"
"dadd o0.zw, r0.zw, r1.zw\n"
"end\n";


// subtract
const char kernelSubR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"sub o0, r0, r1\n"
"end\n";

const char kernelSubLR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"dcl_literal l0, 0x00000000, 0xBFF00000, 0x00000000, 0xBFF00000\n" // [-1.0D0, -1.0D0]
"dmad o0.xy, r1.xy, l0.xy, r0.xy\n"	// o0 = r1*(-1) + r0 == r0 - r1
"dmad o0.zw, r1.zw, l0.zw, r0.zw\n"	// o0 = r1*(-1) + r0 == r0 - r1
"end\n";

// elementwise multiply
const char kernelEwMulR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"mul o0, r0, r1\n"
"end\n";

const char kernelEwMulLR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"dmul o0.xy, r0.xy, r1.xy\n"
"dmul o0.zw, r0.zw, r1.zw\n"
"end\n";

// elementwise divide
const char kernelEwDivR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"div_zeroop(zero) o0, r0, r1\n"
"end\n";

const char kernelEwDivLR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
"ddiv_zeroop(zero) o0.xy, r0.xy, r1.xy\n"
"ddiv_zeroop(zero) o0.zw, r0.zw, r1.zw\n"
"end\n";


const char kernelMatVecR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[1]\n"
"dcl_literal l0, 0, 0, 0, 0\n"
"dcl_literal l2, 0.0f, 0.0f, 0.0f, 4.0f\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"mov r0, r0.0000\n"	// r0.xy is index of the first rhs element

// r0.zw -> 2D position of first input row
"flr r0.___w, vWinCoord0.x\n"
"mul r0.w, r0.w, l2.w\n"

"mov r1.0yzw, r0.w\n"
"add r1.y, r1.y, r1.1\n"		// 2D position of second input row
"add r1.z, r1.y, r1.1\n"		// 2D position of third input row
"add r1.w, r1.z, r1.1\n"		// 2D position of fourth input row

"mov r20, r20.0000\n"
"mov r21, r21.0000\n"
"mov r22, r22.0000\n"
"mov r23, r23.0000\n"

"mov r10, r10.0000\n"	// loop counter
"mov r11, cb0[0]\n"

"whileloop\n"
"    ge r10.y, r10.x, r11.x\n"	// while(loop counter < cb0[0].x)
"    break_logicalnz r10.y\n"

	// load next part of rhs
"	sample_resource(1)_sampler(1) r5, r0.xy\n"

	// load rows parts
"	sample_resource(0)_sampler(0) r6, r0.zw\n"
"	sample_resource(0)_sampler(0) r7, r1.xy\n"
"	sample_resource(0)_sampler(0) r8, r1.xz\n"
"	sample_resource(0)_sampler(0) r9, r1.xw\n"

	// do element wise multiply	
"	mad r20, r5, r6, r20\n"
"	mad r21, r5, r7, r21\n"
"	mad r22, r5, r8, r22\n"
"	mad r23, r5, r9, r23\n"

"	add r0.xz, r0.xz, r0.11\n"
"	add r1.x, r1.x, r1.1\n"

"	add r10.x, r10.x, r10.1\n"	// loop counter ++
"endloop\n"

// now do final horizontal add
"dp4 r30.x, r20, r20.1111\n"	// r +* ones == r.x+r.y+r.z+r.w
"dp4 r30.y, r21, r21.1111\n"
"dp4 r30.z, r22, r22.1111\n"
"dp4 r30.w, r23, r23.1111\n"

"dcl_output_generic o0\n"
"mov o0, r30\n"

"end\n";


/*
	Matrix multiplication C := A*B for the case
	when A has size [8*M,4*N] and B has size [4*N,K]
	where M, N, K any integer numbers
*/
const char kernelMatMul8x4by4x4R_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[1]\n"	// [A.width, C.pitch]
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_literal l0, 0.0f, 8.0f, 1.0f, 2.0f\n"
"dcl_literal l1, 3.0f, 3.0f, 3.0f, 3.0f\n"
"dcl_literal l2, 0.0f, 4.0f, 4.0f, 4.0f\n"

// 2D index of first row in first block of A
"flr r0.0y, vWinCoord0.y\n"		// [0,y] in the execution domain
"mul r0.y, r0.y, l0.y\n"		// multiply y coordinate by 8 to get real position in A data

// 2D index of first column of block of B
"flr r1.x000, vWinCoord0.x\n"	// [x,0] in the B data

// clear float4 accumulators for 8x4 * 4x4 matrix multiply result
"mov r34, r34.0000\n"	
"mov r35, r35.0000\n"
"mov r36, r36.0000\n"
"mov r37, r37.0000\n"
"mov r38, r38.0000\n"	
"mov r39, r39.0000\n"
"mov r40, r40.0000\n"
"mov r41, r41.0000\n"

// initialize sample counters for B
"mov r1.zw, l0\n"			// r1 := [x,0,1,2]
"add r3, r1, l1.0yzw\n"		// r3 := [x,3,4,5]

// initialize sample counters for A
"add r0.__zw, r0.00yy, l0.00zw\n"	// r0 := [0,y,y+1,y+2]
"add r8.0yzw, r0.0yzw, l1.0yzw\n"	// r8 := [0,y+3,y+4,y+5]
"add r9.0yzw, r8.0yzw, l1.0yzw\n"	// r9 := [0,y+6,y+7,y+8]

"mov r2.0y00, cb0[0].x\n"	// r2.x is the loop counter, r2.y := A.width

"whileloop\n"
"    ge r2.z, r2.x, r2.y\n"	// while(loop counter < width)
"    break_logicalnz r2.z\n"

	// load 4 next 4x4 blocks of B
"	sample_resource(1)_sampler(1) r10, r1.xy\n"
"	sample_resource(1)_sampler(1) r11, r1.xz\n"
"	sample_resource(1)_sampler(1) r12, r1.xw\n"
"	sample_resource(1)_sampler(1) r13, r3.xy\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk

	// row 1
"	mad r42, r26.x, r10, r34\n"	// r42 := Ablk[0,0]*Bblk[0,*] + Cblk[0,*]
"	mad r42, r26.y, r11, r42\n"	// r42 := Ablk[0,1]*Bblk[1,*] + r42
"	mad r42, r26.z, r12, r42\n"	// r42 := Ablk[0,2]*Bblk[2,*] + r42
"	mad r34, r26.w, r13, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk[3,*] + r42
	// row 2
"	mad r42, r27.x, r10, r35\n"
"	mad r42, r27.y, r11, r42\n"
"	mad r42, r27.z, r12, r42\n"
"	mad r35, r27.w, r13, r42\n"
	// row 3
"	mad r42, r28.x, r10, r36\n"
"	mad r42, r28.y, r11, r42\n"
"	mad r42, r28.z, r12, r42\n"
"	mad r36, r28.w, r13, r42\n"
	// row 4
"	mad r42, r29.x, r10, r37\n"
"	mad r42, r29.y, r11, r42\n"
"	mad r42, r29.z, r12, r42\n"
"	mad r37, r29.w, r13, r42\n"
	// row 5
"	mad r42, r30.x, r10, r38\n"
"	mad r42, r30.y, r11, r42\n"
"	mad r42, r30.z, r12, r42\n"
"	mad r38, r30.w, r13, r42\n"
	// row 6
"	mad r42, r31.x, r10, r39\n"
"	mad r42, r31.y, r11, r42\n"
"	mad r42, r31.z, r12, r42\n"
"	mad r39, r31.w, r13, r42\n"
	// row 7
"	mad r42, r32.x, r10, r40\n"
"	mad r42, r32.y, r11, r42\n"
"	mad r42, r32.z, r12, r42\n"
"	mad r40, r32.w, r13, r42\n"
	// row 8
"	mad r42, r33.x, r10, r41\n"
"	mad r42, r33.y, r11, r42\n"
"	mad r42, r33.z, r12, r42\n"
"	mad r41, r33.w, r13, r42\n"

	// increment sample counters of B
"	add r1, r1, l2\n"
"	add r3, r3, l2\n"

"	add r2.x, r2.x, r2.1\n"	// loop counter ++
"endloop\n"

// convert [x,y] index to equivalent linear index
"flr r0.xy, vWinCoord0\n"			// [x,y] in the execution domain
"mul r0.y, r0.y, l0.y\n"			// multiply y coordinate by 8 to get real position in A data
"ftoi r1.x, cb0[0].y\n"				// r1.x := int(pitch);
"ftoi r0, r0\n"						// r0 := int(r0);
"imad r0.x, r0.y, r1.x, r0.x\n"		// index := y*pitch + x -> index with account of the alignment pitch

// store the result

"mov g[r0.x], r34\n"
"iadd r0.x, r0.x, r1.x\n"			// index += pitch -> switch to the next row

"mov g[r0.x], r35\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r36\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r37\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r38\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r39\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r40\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r41\n"

"end\n";

/*
	Matrix multiplication C := A*B for the case
	when A has size [8*M,4*N] and B has size [4*N,K]
	where M, N, K any integer numbers
*/
const char kernelMatMul8x4by4x4R_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_cb cb0[2]\n"	// [C.width, C.pitch, 1/C.wieght, C.nTotal8x4], [A.width,...]
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_literal l0, 4.0f, 8.0f, 1.0f, 2.0f\n"
"dcl_literal l1, 3.0f, 3.0f, 3.0f, 3.0f\n"
"dcl_literal l2, 0.0f, 4.0f, 4.0f, 4.0f\n"

"itof r0.z, vaTid0.x\n"				// r0.z := thread_index

"lt r0.y, r0.z, cb0[0].w\n"
"if_logicalnz r0.y\n"				// if thread_index < total number of 8x4 elements in the result

"mul r0.y, r0.z, cb0[0].z\n"		// r0.y := thread_index*(1/(result.width)) == y index
"mod r0.x, r0.z, cb0[0].x\n"		// r0.x := thread_index % result.width == x index
"flr r43.xy, r0.xy\n"				// [x,y] := floor(r0.xy) 2D position in the result

// 2D index of first row in first block of A
"flr r0.0y, r43.y\n"				// [0,y] in the execution domain
"mul r0.y, r0.y, l0.y\n"		// multiply y coordinate by 8 to get real position in A data

// 2D index of first column of block of B
"flr r1.x000, r43.x\n"			// [x,0] in the B data

// clear float4 accumulators for 8x4 * 4x4 matrix multiply result
"mov r34, r34.0000\n"	
"mov r35, r35.0000\n"
"mov r36, r36.0000\n"
"mov r37, r37.0000\n"
"mov r38, r38.0000\n"	
"mov r39, r39.0000\n"
"mov r40, r40.0000\n"
"mov r41, r41.0000\n"

// initialize sample counters for B
"mov r1.zw, l0\n"			// r1 := [x,0,1,2]
"add r3, r1, l1.0yzw\n"		// r3 := [x,3,4,5]

// initialize sample counters for A
"add r0.__zw, r0.00yy, l0.00zw\n"	// r0 := [0,y,y+1,y+2]
"add r8.0yzw, r0.0yzw, l1.0yzw\n"	// r8 := [0,y+3,y+4,y+5]
"add r9.0yzw, r8.0yzw, l1.0yzw\n"	// r9 := [0,y+6,y+7,y+8]

"mov r2.0y00, cb0[1].x\n"	// r2.x is the loop counter, r2.y := A.width

"whileloop\n"
"    ge r2.z, r2.x, r2.y\n"	// while(loop counter < width)
"    break_logicalnz r2.z\n"

	// load 4 next 4x4 blocks of B
"	sample_resource(1)_sampler(1) r10, r1.xy\n"
"	sample_resource(1)_sampler(1) r11, r1.xz\n"
"	sample_resource(1)_sampler(1) r12, r1.xw\n"
"	sample_resource(1)_sampler(1) r13, r3.xy\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk

	// row 1
"	mad r42, r26.x, r10, r34\n"	// r42 := Ablk[0,0]*Bblk[0,*] + Cblk[0,*]
"	mad r42, r26.y, r11, r42\n"	// r42 := Ablk[0,1]*Bblk[1,*] + r42
"	mad r42, r26.z, r12, r42\n"	// r42 := Ablk[0,2]*Bblk[2,*] + r42
"	mad r34, r26.w, r13, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk[3,*] + r42
	// row 2
"	mad r42, r27.x, r10, r35\n"
"	mad r42, r27.y, r11, r42\n"
"	mad r42, r27.z, r12, r42\n"
"	mad r35, r27.w, r13, r42\n"
	// row 3
"	mad r42, r28.x, r10, r36\n"
"	mad r42, r28.y, r11, r42\n"
"	mad r42, r28.z, r12, r42\n"
"	mad r36, r28.w, r13, r42\n"
	// row 4
"	mad r42, r29.x, r10, r37\n"
"	mad r42, r29.y, r11, r42\n"
"	mad r42, r29.z, r12, r42\n"
"	mad r37, r29.w, r13, r42\n"
	// row 5
"	mad r42, r30.x, r10, r38\n"
"	mad r42, r30.y, r11, r42\n"
"	mad r42, r30.z, r12, r42\n"
"	mad r38, r30.w, r13, r42\n"
	// row 6
"	mad r42, r31.x, r10, r39\n"
"	mad r42, r31.y, r11, r42\n"
"	mad r42, r31.z, r12, r42\n"
"	mad r39, r31.w, r13, r42\n"
	// row 7
"	mad r42, r32.x, r10, r40\n"
"	mad r42, r32.y, r11, r42\n"
"	mad r42, r32.z, r12, r42\n"
"	mad r40, r32.w, r13, r42\n"
	// row 8
"	mad r42, r33.x, r10, r41\n"
"	mad r42, r33.y, r11, r42\n"
"	mad r42, r33.z, r12, r42\n"
"	mad r41, r33.w, r13, r42\n"

	// increment sample counters of B
"	add r1, r1, l2\n"
"	add r3, r3, l2\n"

"	add r2.x, r2.x, r2.1\n"	// loop counter ++
"endloop\n"

// convert [x,y] index to equivalent linear index
"mul r0.xy, r43.xy, l0.1y\n"		// multiply y coordinate by 8 to get real position in A data
"ftoi r1.x, cb0[0].y\n"				// r1.x := int(pitch);
"ftoi r0, r0\n"						// r0 := int(r0);
"imad r0.x, r0.y, r1.x, r0.x\n"		// index := y*pitch + x -> index with account of the alignment pitch

// store the result

"mov g[r0.x], r34\n"
"iadd r0.x, r0.x, r1.x\n"			// index += pitch -> switch to the next row

"mov g[r0.x], r35\n"
"iadd r0.x, r0.x, r1.x\n"

"mov g[r0.x], r36\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r37\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r38\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r39\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r40\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r41\n"

"endif\n"

"end\n";

/*
	Matrix multiplication C := A*B for the case
	when A has size [8*M,16*N] and B has size [16*N,K]
	where M, N, K any integer numbers
*/
const char kernelMatMul4x8x4by4x4x4R_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[1]\n"	// [A.width, C.pitch]
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_literal l0, 4.0f, 8.0f, 1.0f, 2.0f\n"
"dcl_literal l1, 3.0f, 3.0f, 3.0f, 3.0f\n"
"dcl_literal l2, 0.0f, 16.0f, 16.0f, 16.0f\n"

// 2D index of first row in first block of A
"flr r0.0y, vWinCoord0.y\n"		// [0,y] in the execution domain
"mul r0.y, r0.y, l0.y\n"		// multiply y coordinate by 8 to get real position in A data

// 2D index of first column of block of B
"flr r1.x000, vWinCoord0.x\n"	// [x,0] in the B data

// clear float4 accumulators for 8x4 * 4x4 matrix multiply result
"mov r34, r34.0000\n"	
"mov r35, r35.0000\n"
"mov r36, r36.0000\n"
"mov r37, r37.0000\n"
"mov r38, r38.0000\n"	
"mov r39, r39.0000\n"
"mov r40, r40.0000\n"
"mov r41, r41.0000\n"

// initialize sample counters for B
"mov r1.zw, l0\n"			// r1 := [x,0,1,2]
"add r3, r1, l1.0yzw\n"		// r3 := [x,3,4,5]
"add r4, r3, l1.0yzw\n"		// r4 := [x,6,7,8]
"add r5, r4, l1.0yzw\n"		// r5 := [x,9,10,11]
"add r6, r5, l1.0yzw\n"		// r6 := [x,12,13,14]
"add r7, r6, l1.0yzw\n"		// r7 := [x,15,16,17]

// initialize sample counters for A
"add r0.__zw, r0.00yy, l0.00zw\n"	// r0 := [0,y,y+1,y+2]
"add r8.0yzw, r0.0yzw, l1.0yzw\n"	// r8 := [0,y+3,y+4,y+5]
"add r9.0yzw, r8.0yzw, l1.0yzw\n"	// r9 := [0,y+6,y+7,y+8]

"mov r2.0y00, cb0[0].x\n"	// r2.x is the loop counter, r2.y := A.width

"whileloop\n"
"    ge r2.z, r2.x, r2.y\n"	// while(loop counter < width)
"    break_logicalnz r2.z\n"

	// load 4 next 4x4 blocks of B
"	sample_resource(1)_sampler(1) r10, r1.xy\n"
"	sample_resource(1)_sampler(1) r11, r1.xz\n"
"	sample_resource(1)_sampler(1) r12, r1.xw\n"
"	sample_resource(1)_sampler(1) r13, r3.xy\n"

"	sample_resource(1)_sampler(1) r14, r3.xz\n"
"	sample_resource(1)_sampler(1) r15, r3.xw\n"
"	sample_resource(1)_sampler(1) r16, r4.xy\n"
"	sample_resource(1)_sampler(1) r17, r4.xz\n"

"	sample_resource(1)_sampler(1) r18, r4.xw\n"
"	sample_resource(1)_sampler(1) r19, r5.xy\n"
"	sample_resource(1)_sampler(1) r20, r5.xz\n"
"	sample_resource(1)_sampler(1) r21, r5.xw\n"

"	sample_resource(1)_sampler(1) r22, r6.xy\n"
"	sample_resource(1)_sampler(1) r23, r6.xz\n"
"	sample_resource(1)_sampler(1) r24, r6.xw\n"
"	sample_resource(1)_sampler(1) r25, r7.xy\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk0

	// row 1
"	mad r42, r26.x, r10, r34\n"	// r42 := Ablk[0,0]*Bblk0[0,*] + Cblk[0,*]
"	mad r42, r26.y, r11, r42\n"	// r42 := Ablk[0,1]*Bblk0[1,*] + r42
"	mad r42, r26.z, r12, r42\n"	// r42 := Ablk[0,2]*Bblk0[2,*] + r42
"	mad r34, r26.w, r13, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk0[3,*] + r42
	// row 2
"	mad r42, r27.x, r10, r35\n"
"	mad r42, r27.y, r11, r42\n"
"	mad r42, r27.z, r12, r42\n"
"	mad r35, r27.w, r13, r42\n"
	// row 3
"	mad r42, r28.x, r10, r36\n"
"	mad r42, r28.y, r11, r42\n"
"	mad r42, r28.z, r12, r42\n"
"	mad r36, r28.w, r13, r42\n"
	// row 4
"	mad r42, r29.x, r10, r37\n"
"	mad r42, r29.y, r11, r42\n"
"	mad r42, r29.z, r12, r42\n"
"	mad r37, r29.w, r13, r42\n"
	// row 5
"	mad r42, r30.x, r10, r38\n"
"	mad r42, r30.y, r11, r42\n"
"	mad r42, r30.z, r12, r42\n"
"	mad r38, r30.w, r13, r42\n"
	// row 6
"	mad r42, r31.x, r10, r39\n"
"	mad r42, r31.y, r11, r42\n"
"	mad r42, r31.z, r12, r42\n"
"	mad r39, r31.w, r13, r42\n"
	// row 7
"	mad r42, r32.x, r10, r40\n"
"	mad r42, r32.y, r11, r42\n"
"	mad r42, r32.z, r12, r42\n"
"	mad r40, r32.w, r13, r42\n"
	// row 8
"	mad r42, r33.x, r10, r41\n"
"	mad r42, r33.y, r11, r42\n"
"	mad r42, r33.z, r12, r42\n"
"	mad r41, r33.w, r13, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk1

	// row 1
"	mad r42, r26.x, r14, r34\n"	// r42 := Ablk[0,0]*Bblk1[0,*] + Cblk[0,*]
"	mad r42, r26.y, r15, r42\n"	// r42 := Ablk[0,1]*Bblk1[1,*] + r42
"	mad r42, r26.z, r16, r42\n"	// r42 := Ablk[0,2]*Bblk1[2,*] + r42
"	mad r34, r26.w, r17, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk1[3,*] + r42
	// row 2
"	mad r42, r27.x, r14, r35\n"
"	mad r42, r27.y, r15, r42\n"
"	mad r42, r27.z, r16, r42\n"
"	mad r35, r27.w, r17, r42\n"
	// row 3
"	mad r42, r28.x, r14, r36\n"
"	mad r42, r28.y, r15, r42\n"
"	mad r42, r28.z, r16, r42\n"
"	mad r36, r28.w, r17, r42\n"
	// row 4
"	mad r42, r29.x, r14, r37\n"
"	mad r42, r29.y, r15, r42\n"
"	mad r42, r29.z, r16, r42\n"
"	mad r37, r29.w, r17, r42\n"
	// row 5
"	mad r42, r30.x, r14, r38\n"
"	mad r42, r30.y, r15, r42\n"
"	mad r42, r30.z, r16, r42\n"
"	mad r38, r30.w, r17, r42\n"
	// row 6
"	mad r42, r31.x, r14, r39\n"
"	mad r42, r31.y, r15, r42\n"
"	mad r42, r31.z, r16, r42\n"
"	mad r39, r31.w, r17, r42\n"
	// row 7
"	mad r42, r32.x, r14, r40\n"
"	mad r42, r32.y, r15, r42\n"
"	mad r42, r32.z, r16, r42\n"
"	mad r40, r32.w, r17, r42\n"
	// row 8
"	mad r42, r33.x, r14, r41\n"
"	mad r42, r33.y, r15, r42\n"
"	mad r42, r33.z, r16, r42\n"
"	mad r41, r33.w, r17, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk2

	// row 1
"	mad r42, r26.x, r18, r34\n"	// r42 := Ablk[0,0]*Bblk2[0,*] + Cblk[0,*]
"	mad r42, r26.y, r19, r42\n"	// r42 := Ablk[0,1]*Bblk2[1,*] + r42
"	mad r42, r26.z, r20, r42\n"	// r42 := Ablk[0,2]*Bblk2[2,*] + r42
"	mad r34, r26.w, r21, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk2[3,*] + r42
	// row 2
"	mad r42, r27.x, r18, r35\n"
"	mad r42, r27.y, r19, r42\n"
"	mad r42, r27.z, r20, r42\n"
"	mad r35, r27.w, r21, r42\n"
	// row 3
"	mad r42, r28.x, r18, r36\n"
"	mad r42, r28.y, r19, r42\n"
"	mad r42, r28.z, r20, r42\n"
"	mad r36, r28.w, r21, r42\n"
	// row 4
"	mad r42, r29.x, r18, r37\n"
"	mad r42, r29.y, r19, r42\n"
"	mad r42, r29.z, r20, r42\n"
"	mad r37, r29.w, r21, r42\n"
	// row 5
"	mad r42, r30.x, r18, r38\n"
"	mad r42, r30.y, r19, r42\n"
"	mad r42, r30.z, r20, r42\n"
"	mad r38, r30.w, r21, r42\n"
	// row 6
"	mad r42, r31.x, r18, r39\n"
"	mad r42, r31.y, r19, r42\n"
"	mad r42, r31.z, r20, r42\n"
"	mad r39, r31.w, r21, r42\n"
	// row 7
"	mad r42, r32.x, r18, r40\n"
"	mad r42, r32.y, r19, r42\n"
"	mad r42, r32.z, r20, r42\n"
"	mad r40, r32.w, r21, r42\n"
	// row 8
"	mad r42, r33.x, r18, r41\n"
"	mad r42, r33.y, r19, r42\n"
"	mad r42, r33.z, r20, r42\n"
"	mad r41, r33.w, r21, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk3

	// row 1
"	mad r42, r26.x, r22, r34\n"	// r42 := Ablk[0,0]*Bblk3[0,*] + Cblk[0,*]
"	mad r42, r26.y, r23, r42\n"	// r42 := Ablk[0,1]*Bblk3[1,*] + r42
"	mad r42, r26.z, r24, r42\n"	// r42 := Ablk[0,2]*Bblk3[2,*] + r42
"	mad r34, r26.w, r25, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk3[3,*] + r42
	// row 2
"	mad r42, r27.x, r22, r35\n"
"	mad r42, r27.y, r23, r42\n"
"	mad r42, r27.z, r24, r42\n"
"	mad r35, r27.w, r25, r42\n"
	// row 3
"	mad r42, r28.x, r22, r36\n"
"	mad r42, r28.y, r23, r42\n"
"	mad r42, r28.z, r24, r42\n"
"	mad r36, r28.w, r25, r42\n"
	// row 4
"	mad r42, r29.x, r22, r37\n"
"	mad r42, r29.y, r23, r42\n"
"	mad r42, r29.z, r24, r42\n"
"	mad r37, r29.w, r25, r42\n"
	// row 5
"	mad r42, r30.x, r22, r38\n"
"	mad r42, r30.y, r23, r42\n"
"	mad r42, r30.z, r24, r42\n"
"	mad r38, r30.w, r25, r42\n"
	// row 6
"	mad r42, r31.x, r22, r39\n"
"	mad r42, r31.y, r23, r42\n"
"	mad r42, r31.z, r24, r42\n"
"	mad r39, r31.w, r25, r42\n"
	// row 7
"	mad r42, r32.x, r22, r40\n"
"	mad r42, r32.y, r23, r42\n"
"	mad r42, r32.z, r24, r42\n"
"	mad r40, r32.w, r25, r42\n"
	// row 8
"	mad r42, r33.x, r22, r41\n"
"	mad r42, r33.y, r23, r42\n"
"	mad r42, r33.z, r24, r42\n"
"	mad r41, r33.w, r25, r42\n"

	// increment counters of B
"	add r1, r1, l2\n"
"	add r3, r3, l2\n"
"	add r4, r4, l2\n"
"	add r5, r5, l2\n"
"	add r6, r6, l2\n"
"	add r7, r7, l2\n"

"	add r2.x, r2.x, l0.x\n"	// loop counter ++
"endloop\n"

// convert [x,y] index to equivalent linear index
"flr r0.xy, vWinCoord0\n"		// [x,y] in the execution domain
"mul r0.y, r0.y, l0.y\n"			// multiply y coordinate by 8 to get real position in A data
"ftoi r1.x, cb0[0].y\n"				// r1.x := int(pitch);
"ftoi r0, r0\n"						// r0 := int(r0);
"imad r0.x, r0.y, r1.x, r0.x\n"		// index := y*pitch + x -> index with account of the alignment pitch

// store the result

"mov g[r0.x], r34\n"
"iadd r0.x, r0.x, r1.x\n"			// index += pitch -> switch to the next row

"mov g[r0.x], r35\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r36\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r37\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r38\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r39\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r40\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r41\n"

"end\n";

/*
	Matrix multiplication C := A*B for the case
	when A has size [8*M,16*N] and B has size [16*N,K]
	where M, N, K any integer numbers
*/
const char kernelMatMul4x8x4by4x4x4R_CS[] = 
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_cb cb0[2]\n"	// [C.width, C.pitch, 1/C.wieght, C.nTotal8x4], [A.width,...]
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_literal l0, 4.0f, 8.0f, 1.0f, 2.0f\n"
"dcl_literal l1, 3.0f, 3.0f, 3.0f, 3.0f\n"
"dcl_literal l2, 0.0f, 16.0f, 16.0f, 16.0f\n"


"itof r0.z, vaTid0.x\n"				// r0.z := thread_index

"lt r0.y, r0.z, cb0[0].w\n"
"if_logicalnz r0.y\n"				// if thread_index < total number of 8x4 elements in the result

"mul r0.y, r0.z, cb0[0].z\n"		// r0.y := thread_index*(1/(result.width)) == y index
"mod r0.x, r0.z, cb0[0].x\n"		// r0.x := thread_index % result.width == x index
"flr r43.xy, r0.xy\n"				// [x,y] := floor(r0.xy) 2D position in the result

// 2D index of first row in first block of A
"flr r0.0y, r43.y\n"				// [0,y] in the execution domain
"mul r0.y, r0.y, l0.y\n"		// multiply y coordinate by 8 to get real position in A data

// 2D index of first column of block of B
"flr r1.x000, r43.x\n"			// [x,0] in the B data

// clear float4 accumulators for 8x4 * 4x4 matrix multiply result
"mov r34, r34.0000\n"	
"mov r35, r35.0000\n"
"mov r36, r36.0000\n"
"mov r37, r37.0000\n"
"mov r38, r38.0000\n"	
"mov r39, r39.0000\n"
"mov r40, r40.0000\n"
"mov r41, r41.0000\n"

// initialize sample counters for B
"mov r1.zw,	l0\n"			// r1 := [x,0,1,2]
"add r3, r1, l1.0yzw\n"		// r3 := [x,3,4,5]
"add r4, r3, l1.0yzw\n"		// r4 := [x,6,7,8]
"add r5, r4, l1.0yzw\n"		// r5 := [x,9,10,11]
"add r6, r5, l1.0yzw\n"		// r6 := [x,12,13,14]
"add r7, r6, l1.0yzw\n"		// r7 := [x,15,16,17]

// initialize sample counters for A
"add r0.__zw, r0.00yy, l0.00zw\n"	// r0 := [0,y,y+1,y+2]
"add r8.0yzw, r0.0yzw, l1.0yzw\n"	// r8 := [0,y+3,y+4,y+5]
"add r9.0yzw, r8.0yzw, l1.0yzw\n"	// r9 := [0,y+6,y+7,y+8]

"mov r2.0y00, cb0[1].x\n"	// r2.x is the loop counter, r2.y := A.width

"whileloop\n"
"    ge r2.z, r2.x, r2.y\n"	// while(loop counter < width)
"    break_logicalnz r2.z\n"

	// load 4 next 4x4 blocks of B
"	sample_resource(1)_sampler(1) r10, r1.xy\n"
"	sample_resource(1)_sampler(1) r11, r1.xz\n"
"	sample_resource(1)_sampler(1) r12, r1.xw\n"
"	sample_resource(1)_sampler(1) r13, r3.xy\n"

"	sample_resource(1)_sampler(1) r14, r3.xz\n"
"	sample_resource(1)_sampler(1) r15, r3.xw\n"
"	sample_resource(1)_sampler(1) r16, r4.xy\n"
"	sample_resource(1)_sampler(1) r17, r4.xz\n"

"	sample_resource(1)_sampler(1) r18, r4.xw\n"
"	sample_resource(1)_sampler(1) r19, r5.xy\n"
"	sample_resource(1)_sampler(1) r20, r5.xz\n"
"	sample_resource(1)_sampler(1) r21, r5.xw\n"

"	sample_resource(1)_sampler(1) r22, r6.xy\n"
"	sample_resource(1)_sampler(1) r23, r6.xz\n"
"	sample_resource(1)_sampler(1) r24, r6.xw\n"
"	sample_resource(1)_sampler(1) r25, r7.xy\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk0

	// row 1
"	mad r42, r26.x, r10, r34\n"	// r42 := Ablk[0,0]*Bblk0[0,*] + Cblk[0,*]
"	mad r42, r26.y, r11, r42\n"	// r42 := Ablk[0,1]*Bblk0[1,*] + r42
"	mad r42, r26.z, r12, r42\n"	// r42 := Ablk[0,2]*Bblk0[2,*] + r42
"	mad r34, r26.w, r13, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk0[3,*] + r42
	// row 2
"	mad r42, r27.x, r10, r35\n"
"	mad r42, r27.y, r11, r42\n"
"	mad r42, r27.z, r12, r42\n"
"	mad r35, r27.w, r13, r42\n"
	// row 3
"	mad r42, r28.x, r10, r36\n"
"	mad r42, r28.y, r11, r42\n"
"	mad r42, r28.z, r12, r42\n"
"	mad r36, r28.w, r13, r42\n"
	// row 4
"	mad r42, r29.x, r10, r37\n"
"	mad r42, r29.y, r11, r42\n"
"	mad r42, r29.z, r12, r42\n"
"	mad r37, r29.w, r13, r42\n"
	// row 5
"	mad r42, r30.x, r10, r38\n"
"	mad r42, r30.y, r11, r42\n"
"	mad r42, r30.z, r12, r42\n"
"	mad r38, r30.w, r13, r42\n"
	// row 6
"	mad r42, r31.x, r10, r39\n"
"	mad r42, r31.y, r11, r42\n"
"	mad r42, r31.z, r12, r42\n"
"	mad r39, r31.w, r13, r42\n"
	// row 7
"	mad r42, r32.x, r10, r40\n"
"	mad r42, r32.y, r11, r42\n"
"	mad r42, r32.z, r12, r42\n"
"	mad r40, r32.w, r13, r42\n"
	// row 8
"	mad r42, r33.x, r10, r41\n"
"	mad r42, r33.y, r11, r42\n"
"	mad r42, r33.z, r12, r42\n"
"	mad r41, r33.w, r13, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk1

	// row 1
"	mad r42, r26.x, r14, r34\n"	// r42 := Ablk[0,0]*Bblk1[0,*] + Cblk[0,*]
"	mad r42, r26.y, r15, r42\n"	// r42 := Ablk[0,1]*Bblk1[1,*] + r42
"	mad r42, r26.z, r16, r42\n"	// r42 := Ablk[0,2]*Bblk1[2,*] + r42
"	mad r34, r26.w, r17, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk1[3,*] + r42
	// row 2
"	mad r42, r27.x, r14, r35\n"
"	mad r42, r27.y, r15, r42\n"
"	mad r42, r27.z, r16, r42\n"
"	mad r35, r27.w, r17, r42\n"
	// row 3
"	mad r42, r28.x, r14, r36\n"
"	mad r42, r28.y, r15, r42\n"
"	mad r42, r28.z, r16, r42\n"
"	mad r36, r28.w, r17, r42\n"
	// row 4
"	mad r42, r29.x, r14, r37\n"
"	mad r42, r29.y, r15, r42\n"
"	mad r42, r29.z, r16, r42\n"
"	mad r37, r29.w, r17, r42\n"
	// row 5
"	mad r42, r30.x, r14, r38\n"
"	mad r42, r30.y, r15, r42\n"
"	mad r42, r30.z, r16, r42\n"
"	mad r38, r30.w, r17, r42\n"
	// row 6
"	mad r42, r31.x, r14, r39\n"
"	mad r42, r31.y, r15, r42\n"
"	mad r42, r31.z, r16, r42\n"
"	mad r39, r31.w, r17, r42\n"
	// row 7
"	mad r42, r32.x, r14, r40\n"
"	mad r42, r32.y, r15, r42\n"
"	mad r42, r32.z, r16, r42\n"
"	mad r40, r32.w, r17, r42\n"
	// row 8
"	mad r42, r33.x, r14, r41\n"
"	mad r42, r33.y, r15, r42\n"
"	mad r42, r33.z, r16, r42\n"
"	mad r41, r33.w, r17, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk2

	// row 1
"	mad r42, r26.x, r18, r34\n"	// r42 := Ablk[0,0]*Bblk2[0,*] + Cblk[0,*]
"	mad r42, r26.y, r19, r42\n"	// r42 := Ablk[0,1]*Bblk2[1,*] + r42
"	mad r42, r26.z, r20, r42\n"	// r42 := Ablk[0,2]*Bblk2[2,*] + r42
"	mad r34, r26.w, r21, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk2[3,*] + r42
	// row 2
"	mad r42, r27.x, r18, r35\n"
"	mad r42, r27.y, r19, r42\n"
"	mad r42, r27.z, r20, r42\n"
"	mad r35, r27.w, r21, r42\n"
	// row 3
"	mad r42, r28.x, r18, r36\n"
"	mad r42, r28.y, r19, r42\n"
"	mad r42, r28.z, r20, r42\n"
"	mad r36, r28.w, r21, r42\n"
	// row 4
"	mad r42, r29.x, r18, r37\n"
"	mad r42, r29.y, r19, r42\n"
"	mad r42, r29.z, r20, r42\n"
"	mad r37, r29.w, r21, r42\n"
	// row 5
"	mad r42, r30.x, r18, r38\n"
"	mad r42, r30.y, r19, r42\n"
"	mad r42, r30.z, r20, r42\n"
"	mad r38, r30.w, r21, r42\n"
	// row 6
"	mad r42, r31.x, r18, r39\n"
"	mad r42, r31.y, r19, r42\n"
"	mad r42, r31.z, r20, r42\n"
"	mad r39, r31.w, r21, r42\n"
	// row 7
"	mad r42, r32.x, r18, r40\n"
"	mad r42, r32.y, r19, r42\n"
"	mad r42, r32.z, r20, r42\n"
"	mad r40, r32.w, r21, r42\n"
	// row 8
"	mad r42, r33.x, r18, r41\n"
"	mad r42, r33.y, r19, r42\n"
"	mad r42, r33.z, r20, r42\n"
"	mad r41, r33.w, r21, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(0)_sampler(0) r27, r0.xz\n"
"	sample_resource(0)_sampler(0) r28, r0.xw\n"
"	sample_resource(0)_sampler(0) r29, r8.xy\n"
"	sample_resource(0)_sampler(0) r30, r8.xz\n"
"	sample_resource(0)_sampler(0) r31, r8.xw\n"
"	sample_resource(0)_sampler(0) r32, r9.xy\n"
"	sample_resource(0)_sampler(0) r33, r9.xz\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"
"	add r8.x, r8.x, r8.1\n"
"	add r9.x, r9.x, r9.1\n"

	// compute Ablk * Bblk3

	// row 1
"	mad r42, r26.x, r22, r34\n"	// r42 := Ablk[0,0]*Bblk3[0,*] + Cblk[0,*]
"	mad r42, r26.y, r23, r42\n"	// r42 := Ablk[0,1]*Bblk3[1,*] + r42
"	mad r42, r26.z, r24, r42\n"	// r42 := Ablk[0,2]*Bblk3[2,*] + r42
"	mad r34, r26.w, r25, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk3[3,*] + r42
	// row 2
"	mad r42, r27.x, r22, r35\n"
"	mad r42, r27.y, r23, r42\n"
"	mad r42, r27.z, r24, r42\n"
"	mad r35, r27.w, r25, r42\n"
	// row 3
"	mad r42, r28.x, r22, r36\n"
"	mad r42, r28.y, r23, r42\n"
"	mad r42, r28.z, r24, r42\n"
"	mad r36, r28.w, r25, r42\n"
	// row 4
"	mad r42, r29.x, r22, r37\n"
"	mad r42, r29.y, r23, r42\n"
"	mad r42, r29.z, r24, r42\n"
"	mad r37, r29.w, r25, r42\n"
	// row 5
"	mad r42, r30.x, r22, r38\n"
"	mad r42, r30.y, r23, r42\n"
"	mad r42, r30.z, r24, r42\n"
"	mad r38, r30.w, r25, r42\n"
	// row 6
"	mad r42, r31.x, r22, r39\n"
"	mad r42, r31.y, r23, r42\n"
"	mad r42, r31.z, r24, r42\n"
"	mad r39, r31.w, r25, r42\n"
	// row 7
"	mad r42, r32.x, r22, r40\n"
"	mad r42, r32.y, r23, r42\n"
"	mad r42, r32.z, r24, r42\n"
"	mad r40, r32.w, r25, r42\n"
	// row 8
"	mad r42, r33.x, r22, r41\n"
"	mad r42, r33.y, r23, r42\n"
"	mad r42, r33.z, r24, r42\n"
"	mad r41, r33.w, r25, r42\n"

	// increment counters of B
"	add r1, r1, l2\n"
"	add r3, r3, l2\n"
"	add r4, r4, l2\n"
"	add r5, r5, l2\n"
"	add r6, r6, l2\n"
"	add r7, r7, l2\n"

"	add r2.x, r2.x, l0.x\n"	// loop counter ++
"endloop\n"

// convert [x,y] index to equivalent linear index
"mul r0.xy, r43.xy, l0.1y\n"		// multiply y coordinate by 8 to get real position in A data
"ftoi r1.x, cb0[0].y\n"				// r1.x := int(pitch);
"ftoi r0, r0\n"						// r0 := int(r0);
"imad r0.x, r0.y, r1.x, r0.x\n"		// index := y*pitch + x -> index with account of the alignment pitch

// store the result

"mov g[r0.x], r34\n"
"iadd r0.x, r0.x, r1.x\n"			// index += pitch -> switch to the next row

"mov g[r0.x], r35\n"
"iadd r0.x, r0.x, r1.x\n"

"mov g[r0.x], r36\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r37\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r38\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r39\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r40\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r41\n"

"endif\n"

"end\n";

/*
	Matrix multiplication C := A*B for the case
	when A has size [4*M,4*N] and B has size [4*N,K]
	where M, N, K any integer numbers	

	compute shader
*/
const char kernelMatMul4x4by4x4R_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_cb cb0[2]\n"
"dcl_literal l0, 4.0f, 4.0f, 4.0f, 4.0f\n"

"ftoi r0.x, cb0[0].w"		// total number of 4x4 elements in the result
"ilt r0.y, vaTid0.x, r0.x\n"

"if_logicalnz r0.y\n"		// if thread_index < total number of 4x4 elements in the result

"	itof r0.z, vaTid0.x\n"				// r0.z := thread_index
"	mul r0.y, r0.z, cb0[0].z\n"			// r0.y := thread_index*(1/(result.width)) == y index
"	mod r0.x, r0.z, cb0[0].x\n"			// r0.x := thread_index % result.width == x index
"	flr r3.xy, r0.xy\n"					// [x,y] := floor(r0.xy) 2D position in the result

	// 2D index of first row in first block of A
"	flr r0.0yz0, r3.y\n"		// [0,y]
"	mul r0.yz, r0.yz, l0.yz\n"	// multiply y coordinate by 4

	// 2D index of first column of block of B
"	flr r1.x000, r3.x\n"	// [x,0]

	// clear float4 accumulators for small matrix multiply result
"	mov r12, r12.0000\n"	
"	mov r13, r13.0000\n"
"	mov r14, r14.0000\n"
"	mov r15, r15.0000\n"

"	mov r2.0y00, cb0[1].x\n"	// r2.x is the loop counter

"	whileloop\n"
"		ge r2.z, r2.x, r2.y\n"	// while(loop counter < lhs.width)
"		break_logicalnz r2.z\n"

		// load the next block of the rhs
"		sample_resource(1)_sampler(1) r4, r1.xy\n"
"		add r1.y, r1.y, r1.1\n"
"		sample_resource(1)_sampler(1) r5, r1.xy\n"
"		add r1.y, r1.y, r1.1\n"
"		sample_resource(1)_sampler(1) r6, r1.xy\n"
"		add r1.y, r1.y, r1.1\n"
"		sample_resource(1)_sampler(1) r7, r1.xy\n"
"		add r1.y, r1.y, r1.1\n"
	
		// load the next block of the lhs
"		sample_resource(0)_sampler(0) r8, r0.xy\n"
"		add r0.y, r0.y, r0.1\n"
"		sample_resource(0)_sampler(0) r9, r0.xy\n"
"		add r0.y, r0.y, r0.1\n"
"		sample_resource(0)_sampler(0) r10, r0.xy\n"
"		add r0.y, r0.y, r0.1\n"
"		sample_resource(0)_sampler(0) r11, r0.xy\n"
"		add r0.x, r0.x, r0.1\n"
"		mov r0._y, r0.z\n"

		// compute small matrix multiplication

		// first row
"		mad r16, r8.x, r4, r12\n"
"		mad r16, r8.y, r5, r16\n"
"		mad r16, r8.z, r6, r16\n"
"		mad r12, r8.w, r7, r16\n"

		// second row
"		mad r16, r9.x, r4, r13\n"
"		mad r16, r9.y, r5, r16\n"
"		mad r16, r9.z, r6, r16\n"
"		mad r13, r9.w, r7, r16\n"

		// third row
"		mad r16, r10.x, r4, r14\n"
"		mad r16, r10.y, r5, r16\n"
"		mad r16, r10.z, r6, r16\n"
"		mad r14, r10.w, r7, r16\n"

		// fourth row
"		mad r16, r11.x, r4, r15\n"
"		mad r16, r11.y, r5, r16\n"
"		mad r16, r11.z, r6, r16\n"
"		mad r15, r11.w, r7, r16\n"

"		add r2.x, r2.x, r2.1\n"	// loop counter ++
"	endloop\n"

	// convert [x,y] index to the linear form	
"	mul r3.y, r3.y, l0.y\n"			// multiply y coordinate by 4 to get real position in the result
"	ftoi r3, r3\n"
"	ftoi r2, cb0[0]\n"
"	imad r0.x, r3.y, r2.y, r3.x\n"	// index := y*result.pitch + x -> index with account of the alignment pitch

	// store the result

"	mov g[r0.x], r12\n"
"	iadd r0.x, r0.x, r2.y\n"		// index := index + pitch

"	mov g[r0.x], r13\n"
"	iadd r0.x, r0.x, r2.y\n"		// index := index + pitch

"	mov g[r0.x], r14\n"
"	iadd r0.x, r0.x, r2.y\n"		// index := index + pitch

"	mov g[r0.x], r15\n"

"endif\n"

"end\n";

/*
	Divide a 2D array into 8 parts
*/
const char kernelDivideMatrixTo8Parts_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 1.0f, 8.0f, 0.0f, 0.0f\n"

"flr r0.xy, vWinCoord0.xy\n"
"mul r0.y, r0.y, l0.y\n"	// r1.xy := [x,y*8] - 2D position of the first row to copy

"dcl_output_generic o0\n"
"dcl_output_generic o1\n"
"dcl_output_generic o2\n"
"dcl_output_generic o3\n"
"dcl_output_generic o4\n"
"dcl_output_generic o5\n"
"dcl_output_generic o6\n"
"dcl_output_generic o7\n"

"sample_resource(0)_sampler(0) o0, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o1, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o2, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o3, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o4, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o5, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o6, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o7, r0.xy\n"

"end\n";

/*
	Divide a 2D array into 4 parts
*/
const char kernelDivideMatrixTo4Parts_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 1.0f, 4.0f, 0.0f, 0.0f\n"

"flr r0.xy, vWinCoord0.xy\n"
"mul r0.y, r0.y, l0.y\n"	// r1.xy := [x,y*4] - 2D position of the first row to copy

"dcl_output_generic o0\n"
"dcl_output_generic o1\n"
"dcl_output_generic o2\n"
"dcl_output_generic o3\n"

"sample_resource(0)_sampler(0) o0, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o1, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o2, r0.xy\n"
"add r0.y, r0.y, r0.1\n"
"sample_resource(0)_sampler(0) o3, r0.xy\n"

"end\n";

/*
	Gather a 2D array from 8 parts
*/
const char kernelGatherMatrixFrom8Parts_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[1]\n" // [pitch,...]

"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(2)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(3)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(4)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(5)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(6)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(7)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 1.0f, 8.0f, 0.0f, 0.0f\n"

"flr r0.xy, vWinCoord0.xy\n"
"mul r1.xy, r0.xy, l0.xy\n"			// r1.xy := [x,y*8] - 2D position of the first row to copy

"mad r1.x, r1.y, cb0[0].x, r1.x\n"	// index := y*A.pitch + x -> index with account of the alignment pitch
"ftoi r1.x, r1.x\n"
"ftoi r2.x, cb0[0].x\n"

"sample_resource(0)_sampler(0) r3, r0.xy\n"
"sample_resource(1)_sampler(1) r4, r0.xy\n"
"sample_resource(2)_sampler(2) r5, r0.xy\n"
"sample_resource(3)_sampler(3) r6, r0.xy\n"
"sample_resource(4)_sampler(4) r7, r0.xy\n"
"sample_resource(5)_sampler(5) r8, r0.xy\n"
"sample_resource(6)_sampler(6) r9, r0.xy\n"
"sample_resource(7)_sampler(7) r10, r0.xy\n"

"mov g[r1.x], r3\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r4\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r5\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r6\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r7\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r8\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r9\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r10\n"

"end\n";

/*
	Gather a 2D array from 4 parts
*/
const char kernelGatherMatrixFrom4Parts_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[1]\n" // [pitch,...]

"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(2)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(3)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 1.0f, 4.0f, 0.0f, 0.0f\n"

"flr r0.xy, vWinCoord0.xy\n"
"mul r1.xy, r0.xy, l0.xy\n"			// r1.xy := [x,y*4] - 2D position of the first row to copy

"mad r1.x, r1.y, cb0[0].x, r1.x\n"	// index := y*A.pitch + x -> index with account of the alignment pitch
"ftoi r1.x, r1.x\n"
"ftoi r2.x, cb0[0].x\n"

"sample_resource(0)_sampler(0) r3, r0.xy\n"
"sample_resource(1)_sampler(1) r4, r0.xy\n"
"sample_resource(2)_sampler(2) r5, r0.xy\n"
"sample_resource(3)_sampler(3) r6, r0.xy\n"

"mov g[r1.x], r3\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r4\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r5\n"
"iadd r1.x, r1.x, r2.x\n"
"mov g[r1.x], r6\n"

"end\n";

const char kernelMatMulByParts4x8x4by4x4x4R_CS[] = 
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_cb cb0[2]\n"	// [C.width, C.pitch, 1/C.width, C.nTotal8x4], [A.width,...]

"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(2)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(3)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(4)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(5)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(6)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(7)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(8)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(9)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(10)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(11)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 4.0f, 8.0f, 1.0f, 2.0f\n"
"dcl_literal l1, 3.0f, 3.0f, 3.0f, 3.0f\n"
"dcl_literal l2, 0.0f, 4.0f, 4.0f, 4.0f\n"

"itof r0.z, vaTid0.x\n"				// r0.z := thread_index

"lt r0.y, r0.z, cb0[0].w\n"
"if_logicalnz r0.y\n"				// if thread_index < total number of 8x4 elements in the result

"mul r0.y, r0.z, cb0[0].z\n"		// r0.y := thread_index*(1/(result.width)) == y index
"mod r0.x, r0.z, cb0[0].x\n"		// r0.x := thread_index % result.width == x index
"flr r4.xy, r0.xy\n"				// [x,y] := floor(r0.xy) 2D position in the result

// 2D index of first row in first block of A
"flr r0.0y, r4.y\n"				// [0,y] in the execution domain

// 2D index of first column of block of B
"flr r1.x000, r4.x\n"			// [x,0] in the B data

// clear float4 accumulators for 8x4 * 4x4 matrix multiply result
"mov r34, r34.0000\n"	
"mov r35, r35.0000\n"
"mov r36, r36.0000\n"
"mov r37, r37.0000\n"
"mov r38, r38.0000\n"	
"mov r39, r39.0000\n"
"mov r40, r40.0000\n"
"mov r41, r41.0000\n"

// initialize sample counters for B
"mov r1.zw,	l0\n"			// r1 := [x,0,1,2]
"add r3, r1, l1.0yzw\n"		// r3 := [x,3,4,5]

"mov r2.0y00, cb0[1].x\n"	// r2.x is the loop counter, r2.y := A.width

"whileloop\n"
"    ge r2.z, r2.x, r2.y\n"	// while(loop counter < A.width)
"    break_logicalnz r2.z\n"

	// load 4 next 4x4 blocks of B
"	sample_resource(8)_sampler(8) r10, r1.xy\n"
"	sample_resource(9)_sampler(9) r11, r1.xy\n"
"	sample_resource(10)_sampler(10) r12, r1.xy\n"
"	sample_resource(11)_sampler(11) r13, r1.xy\n"

"	sample_resource(8)_sampler(8) r14, r1.xz\n"
"	sample_resource(9)_sampler(9) r15, r1.xz\n"
"	sample_resource(10)_sampler(10) r16, r1.xz\n"
"	sample_resource(11)_sampler(11) r17, r1.xz\n"

"	sample_resource(8)_sampler(8) r18, r1.xw\n"
"	sample_resource(9)_sampler(9) r19, r1.xw\n"
"	sample_resource(10)_sampler(10) r20, r1.xw\n"
"	sample_resource(11)_sampler(11) r21, r1.xw\n"

"	sample_resource(8)_sampler(8) r22, r3.xy\n"
"	sample_resource(9)_sampler(9) r23, r3.xy\n"
"	sample_resource(10)_sampler(10) r24, r3.xy\n"
"	sample_resource(11)_sampler(11) r25, r3.xy\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment counters of B
"	add r1, r1, l2\n"
"	add r3, r3, l2\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk0

	// row 1
"	mad r42, r26.x, r10, r34\n"	// r42 := Ablk[0,0]*Bblk0[0,*] + Cblk[0,*]
"	mad r42, r26.y, r11, r42\n"	// r42 := Ablk[0,1]*Bblk0[1,*] + r42
"	mad r42, r26.z, r12, r42\n"	// r42 := Ablk[0,2]*Bblk0[2,*] + r42
"	mad r34, r26.w, r13, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk0[3,*] + r42
	// row 2
"	mad r42, r27.x, r10, r35\n"
"	mad r42, r27.y, r11, r42\n"
"	mad r42, r27.z, r12, r42\n"
"	mad r35, r27.w, r13, r42\n"
	// row 3
"	mad r42, r28.x, r10, r36\n"
"	mad r42, r28.y, r11, r42\n"
"	mad r42, r28.z, r12, r42\n"
"	mad r36, r28.w, r13, r42\n"
	// row 4
"	mad r42, r29.x, r10, r37\n"
"	mad r42, r29.y, r11, r42\n"
"	mad r42, r29.z, r12, r42\n"
"	mad r37, r29.w, r13, r42\n"
	// row 5
"	mad r42, r30.x, r10, r38\n"
"	mad r42, r30.y, r11, r42\n"
"	mad r42, r30.z, r12, r42\n"
"	mad r38, r30.w, r13, r42\n"
	// row 6
"	mad r42, r31.x, r10, r39\n"
"	mad r42, r31.y, r11, r42\n"
"	mad r42, r31.z, r12, r42\n"
"	mad r39, r31.w, r13, r42\n"
	// row 7
"	mad r42, r32.x, r10, r40\n"
"	mad r42, r32.y, r11, r42\n"
"	mad r42, r32.z, r12, r42\n"
"	mad r40, r32.w, r13, r42\n"
	// row 8
"	mad r42, r33.x, r10, r41\n"
"	mad r42, r33.y, r11, r42\n"
"	mad r42, r33.z, r12, r42\n"
"	mad r41, r33.w, r13, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk1

	// row 1
"	mad r42, r26.x, r14, r34\n"	// r42 := Ablk[0,0]*Bblk1[0,*] + Cblk[0,*]
"	mad r42, r26.y, r15, r42\n"	// r42 := Ablk[0,1]*Bblk1[1,*] + r42
"	mad r42, r26.z, r16, r42\n"	// r42 := Ablk[0,2]*Bblk1[2,*] + r42
"	mad r34, r26.w, r17, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk1[3,*] + r42
	// row 2
"	mad r42, r27.x, r14, r35\n"
"	mad r42, r27.y, r15, r42\n"
"	mad r42, r27.z, r16, r42\n"
"	mad r35, r27.w, r17, r42\n"
	// row 3
"	mad r42, r28.x, r14, r36\n"
"	mad r42, r28.y, r15, r42\n"
"	mad r42, r28.z, r16, r42\n"
"	mad r36, r28.w, r17, r42\n"
	// row 4
"	mad r42, r29.x, r14, r37\n"
"	mad r42, r29.y, r15, r42\n"
"	mad r42, r29.z, r16, r42\n"
"	mad r37, r29.w, r17, r42\n"
	// row 5
"	mad r42, r30.x, r14, r38\n"
"	mad r42, r30.y, r15, r42\n"
"	mad r42, r30.z, r16, r42\n"
"	mad r38, r30.w, r17, r42\n"
	// row 6
"	mad r42, r31.x, r14, r39\n"
"	mad r42, r31.y, r15, r42\n"
"	mad r42, r31.z, r16, r42\n"
"	mad r39, r31.w, r17, r42\n"
	// row 7
"	mad r42, r32.x, r14, r40\n"
"	mad r42, r32.y, r15, r42\n"
"	mad r42, r32.z, r16, r42\n"
"	mad r40, r32.w, r17, r42\n"
	// row 8
"	mad r42, r33.x, r14, r41\n"
"	mad r42, r33.y, r15, r42\n"
"	mad r42, r33.z, r16, r42\n"
"	mad r41, r33.w, r17, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk2

	// row 1
"	mad r42, r26.x, r18, r34\n"	// r42 := Ablk[0,0]*Bblk2[0,*] + Cblk[0,*]
"	mad r42, r26.y, r19, r42\n"	// r42 := Ablk[0,1]*Bblk2[1,*] + r42
"	mad r42, r26.z, r20, r42\n"	// r42 := Ablk[0,2]*Bblk2[2,*] + r42
"	mad r34, r26.w, r21, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk2[3,*] + r42
	// row 2
"	mad r42, r27.x, r18, r35\n"
"	mad r42, r27.y, r19, r42\n"
"	mad r42, r27.z, r20, r42\n"
"	mad r35, r27.w, r21, r42\n"
	// row 3
"	mad r42, r28.x, r18, r36\n"
"	mad r42, r28.y, r19, r42\n"
"	mad r42, r28.z, r20, r42\n"
"	mad r36, r28.w, r21, r42\n"
	// row 4
"	mad r42, r29.x, r18, r37\n"
"	mad r42, r29.y, r19, r42\n"
"	mad r42, r29.z, r20, r42\n"
"	mad r37, r29.w, r21, r42\n"
	// row 5
"	mad r42, r30.x, r18, r38\n"
"	mad r42, r30.y, r19, r42\n"
"	mad r42, r30.z, r20, r42\n"
"	mad r38, r30.w, r21, r42\n"
	// row 6
"	mad r42, r31.x, r18, r39\n"
"	mad r42, r31.y, r19, r42\n"
"	mad r42, r31.z, r20, r42\n"
"	mad r39, r31.w, r21, r42\n"
	// row 7
"	mad r42, r32.x, r18, r40\n"
"	mad r42, r32.y, r19, r42\n"
"	mad r42, r32.z, r20, r42\n"
"	mad r40, r32.w, r21, r42\n"
	// row 8
"	mad r42, r33.x, r18, r41\n"
"	mad r42, r33.y, r19, r42\n"
"	mad r42, r33.z, r20, r42\n"
"	mad r41, r33.w, r21, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk3

	// row 1
"	mad r42, r26.x, r22, r34\n"	// r42 := Ablk[0,0]*Bblk3[0,*] + Cblk[0,*]
"	mad r42, r26.y, r23, r42\n"	// r42 := Ablk[0,1]*Bblk3[1,*] + r42
"	mad r42, r26.z, r24, r42\n"	// r42 := Ablk[0,2]*Bblk3[2,*] + r42
"	mad r34, r26.w, r25, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk3[3,*] + r42
	// row 2
"	mad r42, r27.x, r22, r35\n"
"	mad r42, r27.y, r23, r42\n"
"	mad r42, r27.z, r24, r42\n"
"	mad r35, r27.w, r25, r42\n"
	// row 3
"	mad r42, r28.x, r22, r36\n"
"	mad r42, r28.y, r23, r42\n"
"	mad r42, r28.z, r24, r42\n"
"	mad r36, r28.w, r25, r42\n"
	// row 4
"	mad r42, r29.x, r22, r37\n"
"	mad r42, r29.y, r23, r42\n"
"	mad r42, r29.z, r24, r42\n"
"	mad r37, r29.w, r25, r42\n"
	// row 5
"	mad r42, r30.x, r22, r38\n"
"	mad r42, r30.y, r23, r42\n"
"	mad r42, r30.z, r24, r42\n"
"	mad r38, r30.w, r25, r42\n"
	// row 6
"	mad r42, r31.x, r22, r39\n"
"	mad r42, r31.y, r23, r42\n"
"	mad r42, r31.z, r24, r42\n"
"	mad r39, r31.w, r25, r42\n"
	// row 7
"	mad r42, r32.x, r22, r40\n"
"	mad r42, r32.y, r23, r42\n"
"	mad r42, r32.z, r24, r42\n"
"	mad r40, r32.w, r25, r42\n"
	// row 8
"	mad r42, r33.x, r22, r41\n"
"	mad r42, r33.y, r23, r42\n"
"	mad r42, r33.z, r24, r42\n"
"	mad r41, r33.w, r25, r42\n"

"	add r2.x, r2.x, l0.x\n"	// loop counter ++
"endloop\n"

// convert [x,y] index to equivalent linear index in the result data
"mul r4.y, r4.y, l0.y\n"			// multiply y coordinate by 8 to get real position in result data
"ftoi r1.x, cb0[0].y\n"				// r1.x := int(pitch);
"ftoi r4.xy, r4.xy\n"				
"imad r0.x, r4.y, r1.x, r4.x\n"		// index := y*pitch + x -> index with account of the alignment pitch

// store the result

"mov g[r0.x], r34\n"
"iadd r0.x, r0.x, r1.x\n"			// index += pitch -> switch to the next row

"mov g[r0.x], r35\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r36\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r37\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r38\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r39\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r40\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r41\n"

"endif\n"

"end\n";

const char kernelMatMulByParts4x8x4by4x4x4R_PS[] = 
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[2]\n"	// [C.width, C.pitch, 1/C.width, C.nTotal4x4], [A.width,...]
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(2)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(3)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(4)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(5)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(6)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(7)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(8)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(9)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(10)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(11)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 4.0f, 8.0f, 1.0f, 2.0f\n"
"dcl_literal l1, 3.0f, 3.0f, 3.0f, 3.0f\n"
"dcl_literal l2, 0.0f, 4.0f, 4.0f, 4.0f\n"

// 2D index of first row in first block of A
"flr r0.0y, vWinCoord0.y\n"				// [0,y] in the execution domain

// 2D index of first column of block of B
"flr r1.x000, vWinCoord0.x\n"			// [x,0] in the B data

// clear float4 accumulators for 8x4 * 4x4 matrix multiply result
"mov r34, r34.0000\n"	
"mov r35, r35.0000\n"
"mov r36, r36.0000\n"
"mov r37, r37.0000\n"
"mov r38, r38.0000\n"	
"mov r39, r39.0000\n"
"mov r40, r40.0000\n"
"mov r41, r41.0000\n"

// initialize sample counters for B
"mov r1.zw,	l0\n"			// r1 := [x,0,1,2]
"add r3, r1, l1.0yzw\n"		// r3 := [x,3,4,5]

"mov r2.0y00, cb0[1].x\n"	// r2.x is the loop counter, r2.y := A.width

"whileloop\n"
"    ge r2.z, r2.x, r2.y\n"	// while(loop counter < A.width)
"    break_logicalnz r2.z\n"

	// load 4 next 4x4 blocks of B
"	sample_resource(8)_sampler(8) r10, r1.xy\n"
"	sample_resource(9)_sampler(9) r11, r1.xy\n"
"	sample_resource(10)_sampler(10) r12, r1.xy\n"
"	sample_resource(11)_sampler(11) r13, r1.xy\n"

"	sample_resource(8)_sampler(8) r14, r1.xz\n"
"	sample_resource(9)_sampler(9) r15, r1.xz\n"
"	sample_resource(10)_sampler(10) r16, r1.xz\n"
"	sample_resource(11)_sampler(11) r17, r1.xz\n"

"	sample_resource(8)_sampler(8) r18, r1.xw\n"
"	sample_resource(9)_sampler(9) r19, r1.xw\n"
"	sample_resource(10)_sampler(10) r20, r1.xw\n"
"	sample_resource(11)_sampler(11) r21, r1.xw\n"

"	sample_resource(8)_sampler(8) r22, r3.xy\n"
"	sample_resource(9)_sampler(9) r23, r3.xy\n"
"	sample_resource(10)_sampler(10) r24, r3.xy\n"
"	sample_resource(11)_sampler(11) r25, r3.xy\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment counters of B
"	add r1, r1, l2\n"
"	add r3, r3, l2\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk0

	// row 1
"	mad r42, r26.x, r10, r34\n"	// r42 := Ablk[0,0]*Bblk0[0,*] + Cblk[0,*]
"	mad r42, r26.y, r11, r42\n"	// r42 := Ablk[0,1]*Bblk0[1,*] + r42
"	mad r42, r26.z, r12, r42\n"	// r42 := Ablk[0,2]*Bblk0[2,*] + r42
"	mad r34, r26.w, r13, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk0[3,*] + r42
	// row 2
"	mad r42, r27.x, r10, r35\n"
"	mad r42, r27.y, r11, r42\n"
"	mad r42, r27.z, r12, r42\n"
"	mad r35, r27.w, r13, r42\n"
	// row 3
"	mad r42, r28.x, r10, r36\n"
"	mad r42, r28.y, r11, r42\n"
"	mad r42, r28.z, r12, r42\n"
"	mad r36, r28.w, r13, r42\n"
	// row 4
"	mad r42, r29.x, r10, r37\n"
"	mad r42, r29.y, r11, r42\n"
"	mad r42, r29.z, r12, r42\n"
"	mad r37, r29.w, r13, r42\n"
	// row 5
"	mad r42, r30.x, r10, r38\n"
"	mad r42, r30.y, r11, r42\n"
"	mad r42, r30.z, r12, r42\n"
"	mad r38, r30.w, r13, r42\n"
	// row 6
"	mad r42, r31.x, r10, r39\n"
"	mad r42, r31.y, r11, r42\n"
"	mad r42, r31.z, r12, r42\n"
"	mad r39, r31.w, r13, r42\n"
	// row 7
"	mad r42, r32.x, r10, r40\n"
"	mad r42, r32.y, r11, r42\n"
"	mad r42, r32.z, r12, r42\n"
"	mad r40, r32.w, r13, r42\n"
	// row 8
"	mad r42, r33.x, r10, r41\n"
"	mad r42, r33.y, r11, r42\n"
"	mad r42, r33.z, r12, r42\n"
"	mad r41, r33.w, r13, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk1

	// row 1
"	mad r42, r26.x, r14, r34\n"	// r42 := Ablk[0,0]*Bblk1[0,*] + Cblk[0,*]
"	mad r42, r26.y, r15, r42\n"	// r42 := Ablk[0,1]*Bblk1[1,*] + r42
"	mad r42, r26.z, r16, r42\n"	// r42 := Ablk[0,2]*Bblk1[2,*] + r42
"	mad r34, r26.w, r17, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk1[3,*] + r42
	// row 2
"	mad r42, r27.x, r14, r35\n"
"	mad r42, r27.y, r15, r42\n"
"	mad r42, r27.z, r16, r42\n"
"	mad r35, r27.w, r17, r42\n"
	// row 3
"	mad r42, r28.x, r14, r36\n"
"	mad r42, r28.y, r15, r42\n"
"	mad r42, r28.z, r16, r42\n"
"	mad r36, r28.w, r17, r42\n"
	// row 4
"	mad r42, r29.x, r14, r37\n"
"	mad r42, r29.y, r15, r42\n"
"	mad r42, r29.z, r16, r42\n"
"	mad r37, r29.w, r17, r42\n"
	// row 5
"	mad r42, r30.x, r14, r38\n"
"	mad r42, r30.y, r15, r42\n"
"	mad r42, r30.z, r16, r42\n"
"	mad r38, r30.w, r17, r42\n"
	// row 6
"	mad r42, r31.x, r14, r39\n"
"	mad r42, r31.y, r15, r42\n"
"	mad r42, r31.z, r16, r42\n"
"	mad r39, r31.w, r17, r42\n"
	// row 7
"	mad r42, r32.x, r14, r40\n"
"	mad r42, r32.y, r15, r42\n"
"	mad r42, r32.z, r16, r42\n"
"	mad r40, r32.w, r17, r42\n"
	// row 8
"	mad r42, r33.x, r14, r41\n"
"	mad r42, r33.y, r15, r42\n"
"	mad r42, r33.z, r16, r42\n"
"	mad r41, r33.w, r17, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk2

	// row 1
"	mad r42, r26.x, r18, r34\n"	// r42 := Ablk[0,0]*Bblk2[0,*] + Cblk[0,*]
"	mad r42, r26.y, r19, r42\n"	// r42 := Ablk[0,1]*Bblk2[1,*] + r42
"	mad r42, r26.z, r20, r42\n"	// r42 := Ablk[0,2]*Bblk2[2,*] + r42
"	mad r34, r26.w, r21, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk2[3,*] + r42
	// row 2
"	mad r42, r27.x, r18, r35\n"
"	mad r42, r27.y, r19, r42\n"
"	mad r42, r27.z, r20, r42\n"
"	mad r35, r27.w, r21, r42\n"
	// row 3
"	mad r42, r28.x, r18, r36\n"
"	mad r42, r28.y, r19, r42\n"
"	mad r42, r28.z, r20, r42\n"
"	mad r36, r28.w, r21, r42\n"
	// row 4
"	mad r42, r29.x, r18, r37\n"
"	mad r42, r29.y, r19, r42\n"
"	mad r42, r29.z, r20, r42\n"
"	mad r37, r29.w, r21, r42\n"
	// row 5
"	mad r42, r30.x, r18, r38\n"
"	mad r42, r30.y, r19, r42\n"
"	mad r42, r30.z, r20, r42\n"
"	mad r38, r30.w, r21, r42\n"
	// row 6
"	mad r42, r31.x, r18, r39\n"
"	mad r42, r31.y, r19, r42\n"
"	mad r42, r31.z, r20, r42\n"
"	mad r39, r31.w, r21, r42\n"
	// row 7
"	mad r42, r32.x, r18, r40\n"
"	mad r42, r32.y, r19, r42\n"
"	mad r42, r32.z, r20, r42\n"
"	mad r40, r32.w, r21, r42\n"
	// row 8
"	mad r42, r33.x, r18, r41\n"
"	mad r42, r33.y, r19, r42\n"
"	mad r42, r33.z, r20, r42\n"
"	mad r41, r33.w, r21, r42\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy\n"
"	sample_resource(1)_sampler(1) r27, r0.xy\n"
"	sample_resource(2)_sampler(2) r28, r0.xy\n"
"	sample_resource(3)_sampler(3) r29, r0.xy\n"
"	sample_resource(4)_sampler(4) r30, r0.xy\n"
"	sample_resource(5)_sampler(5) r31, r0.xy\n"
"	sample_resource(6)_sampler(6) r32, r0.xy\n"
"	sample_resource(7)_sampler(7) r33, r0.xy\n"

	// increment sample counters of A
"	add r0.x, r0.x, r0.1\n"

	// compute Ablk * Bblk3

	// row 1
"	mad r42, r26.x, r22, r34\n"	// r42 := Ablk[0,0]*Bblk3[0,*] + Cblk[0,*]
"	mad r42, r26.y, r23, r42\n"	// r42 := Ablk[0,1]*Bblk3[1,*] + r42
"	mad r42, r26.z, r24, r42\n"	// r42 := Ablk[0,2]*Bblk3[2,*] + r42
"	mad r34, r26.w, r25, r42\n"	// Cblk[0,*] := Ablk[0,3]*Bblk3[3,*] + r42
	// row 2
"	mad r42, r27.x, r22, r35\n"
"	mad r42, r27.y, r23, r42\n"
"	mad r42, r27.z, r24, r42\n"
"	mad r35, r27.w, r25, r42\n"
	// row 3
"	mad r42, r28.x, r22, r36\n"
"	mad r42, r28.y, r23, r42\n"
"	mad r42, r28.z, r24, r42\n"
"	mad r36, r28.w, r25, r42\n"
	// row 4
"	mad r42, r29.x, r22, r37\n"
"	mad r42, r29.y, r23, r42\n"
"	mad r42, r29.z, r24, r42\n"
"	mad r37, r29.w, r25, r42\n"
	// row 5
"	mad r42, r30.x, r22, r38\n"
"	mad r42, r30.y, r23, r42\n"
"	mad r42, r30.z, r24, r42\n"
"	mad r38, r30.w, r25, r42\n"
	// row 6
"	mad r42, r31.x, r22, r39\n"
"	mad r42, r31.y, r23, r42\n"
"	mad r42, r31.z, r24, r42\n"
"	mad r39, r31.w, r25, r42\n"
	// row 7
"	mad r42, r32.x, r22, r40\n"
"	mad r42, r32.y, r23, r42\n"
"	mad r42, r32.z, r24, r42\n"
"	mad r40, r32.w, r25, r42\n"
	// row 8
"	mad r42, r33.x, r22, r41\n"
"	mad r42, r33.y, r23, r42\n"
"	mad r42, r33.z, r24, r42\n"
"	mad r41, r33.w, r25, r42\n"

"	add r2.x, r2.x, l0.x\n"	// loop counter ++
"endloop\n"

// convert [x,y] index to equivalent linear index in the result data
"flr r4.xy, vWinCoord0.xy\n"
"mul r4.y, r4.y, l0.y\n"			// multiply y coordinate by 8 to get real position in result data
"ftoi r1.x, cb0[0].y\n"				// r1.x := int(pitch);
"ftoi r4.xy, r4.xy\n"				
"imad r0.x, r4.y, r1.x, r4.x\n"		// index := y*pitch + x -> index with account of the alignment pitch

// store the result

"mov g[r0.x], r34\n"
"iadd r0.x, r0.x, r1.x\n"			// index += pitch -> switch to the next row

"mov g[r0.x], r35\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r36\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r37\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r38\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r39\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r40\n"
"iadd r0.x, r0.x, r1.x\n"			

"mov g[r0.x], r41\n"

"end\n";

/*
	Matrix multiplication C := A*B for the case
	when A has size [4*M,4*N] and B has size [4*N,K]
	where M, N, K any integer numbers
*/
const char kernelMatMul4x4by4x4R_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[1]\n"	// [A.width, C.pitch]
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_literal l0, 4.0f, 4.0f, 4.0f, 4.0f\n"

// 2D index of first row in first block of A
"flr r0.0yz0, vWinCoord0.y\n"	// [0,y]
"mul r0.yz, r0.yz, l0.yz\n"		// multiply y coordinate by 4

// 2D index of first column of block of B
"flr r1.x000, vWinCoord0.x\n"	// [x,0]

// clear float4 accumulators for small matrix multiply result
"mov r12, r12.0000\n"	
"mov r13, r13.0000\n"
"mov r14, r14.0000\n"
"mov r15, r15.0000\n"

"mov r2, r2.0000\n"	// r2.x is the loop counter
"mov r3, cb0[0]\n"

"whileloop\n"
"    ge r2.y, r2.x, r3.x\n"	// while(loop counter < width)
"    break_logicalnz r2.y\n"

	// load the next block of the rhs
"	sample_resource(1)_sampler(1) r4, r1.xy\n"
"	add r1.y, r1.y, r1.1\n"
"	sample_resource(1)_sampler(1) r5, r1.xy\n"
"	add r1.y, r1.y, r1.1\n"
"	sample_resource(1)_sampler(1) r6, r1.xy\n"
"	add r1.y, r1.y, r1.1\n"
"	sample_resource(1)_sampler(1) r7, r1.xy\n"
"	add r1.y, r1.y, r1.1\n"
	
	// load the next block of the lhs
"	sample_resource(0)_sampler(0) r8, r0.xy\n"
"	add r0.y, r0.y, r0.1\n"
"	sample_resource(0)_sampler(0) r9, r0.xy\n"
"	add r0.y, r0.y, r0.1\n"
"	sample_resource(0)_sampler(0) r10, r0.xy\n"
"	add r0.y, r0.y, r0.1\n"
"	sample_resource(0)_sampler(0) r11, r0.xy\n"
"	add r0.x, r0.x, r0.1\n"
"	mov r0._y, r0.z\n"

	// compute small matrix multiplication

	// first row
"	mad r16, r8.x, r4, r12\n"
"	mad r16, r8.y, r5, r16\n"
"	mad r16, r8.z, r6, r16\n"
"	mad r12, r8.w, r7, r16\n"

	// second row
"	mad r16, r9.x, r4, r13\n"
"	mad r16, r9.y, r5, r16\n"
"	mad r16, r9.z, r6, r16\n"
"	mad r13, r9.w, r7, r16\n"

	// third row
"	mad r16, r10.x, r4, r14\n"
"	mad r16, r10.y, r5, r16\n"
"	mad r16, r10.z, r6, r16\n"
"	mad r14, r10.w, r7, r16\n"

	// fourth row
"	mad r16, r11.x, r4, r15\n"
"	mad r16, r11.y, r5, r16\n"
"	mad r16, r11.z, r6, r16\n"
"	mad r15, r11.w, r7, r16\n"

"	add r2.x, r2.x, r2.1\n"	// loop counter ++
"endloop\n"

// store the result

// convert [x,y] index to the linear form
"flr r0.xy, vWinCoord0.xy\n"
"mul r0.y, r0.y, l0.y\n"
"ftoi r0, r0\n"
"ftoi r3, cb0[0]\n"
"imad r0.x, r0.y, r3.y, r0.x\n"	// index := y*pitch + x - index with account of the alignment pitch

"mov g[r0.x], r12\n"
"iadd r0.x, r0.x, r3.y\n"		// index := index + pitch

"mov g[r0.x], r13\n"
"iadd r0.x, r0.x, r3.y\n"		// index := index + pitch

"mov g[r0.x], r14\n"
"iadd r0.x, r0.x, r3.y\n"		// index := index + pitch

"mov g[r0.x], r15\n"

"end\n";

// matrix transposition
const char kernelMatTranspR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
//"sample_resource(0)_sampler(0) r0, vWinCoord0\n"
//"sample_resource(1)_sampler(1) r1, vWinCoord0\n"
//"mul g[0], r0, r1\n"
"end\n";

class Kernel
{
public:
	Kernel(long iKernel, CALtarget target);
	~Kernel(void);

	CALresult err;	// error code for last operation
	long iKernel;	// kernel code
	CALobject obj;	// CAL object
	CALimage img;	// CAL image

	long nInputs;	// number of kernel inputs
	long nOutputs;	// number of kernel outputs
	long nConstants;	// number of kernel constants
	BOOL usesGlobalBuffer;	// TRUE when kernel uses a global buffer

	CALformat* constFormats;	// data formats for each constant
	long* constSizes;			// sizes for each constant
};

/*
	compute shader kernels
*/

const char kernelFillBy2xNComp_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_group 64\n"
"dcl_cb cb0[1]\n"
"mov r0, vaTid0.x\n"
"dcl_literal l0, 2, 0, 0, 0\n"
"imul r0.x, r0.x, l0.x\n"
"mov g[r0.x], cb0[0]\n"
"mov g[r0.x+1], cb0[0]\n"
"end\n";

const char kernelFillBy4xNComp_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_group 64\n"
"dcl_cb cb0[1]\n"
"mov r0, vaTid0.x\n"
"dcl_literal l0, 4, 0, 0, 0\n"
"imul r0.x, r0.x, l0.x\n"
"mov g[r0.x], cb0[0]\n"
"mov g[r0.x+1], cb0[0]\n"
"mov g[r0.x+2], cb0[0]\n"
"mov g[r0.x+3], cb0[0]\n"
"end\n";

const char kernelFillBy8xNComp_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_group 64\n"
"dcl_cb cb0[1]\n"
"mov r0, vaTid0.x\n"
"dcl_literal l0, 8, 0, 0, 0\n"
"imul r0.x, r0.x, l0.x\n"
"mov g[r0.x], cb0[0]\n"
"mov g[r0.x+1], cb0[0]\n"
"mov g[r0.x+2], cb0[0]\n"
"mov g[r0.x+3], cb0[0]\n"
"mov g[r0.x+4], cb0[0]\n"
"mov g[r0.x+5], cb0[0]\n"
"mov g[r0.x+6], cb0[0]\n"
"mov g[r0.x+7], cb0[0]\n"
"end\n";

const char kernelFillBy16xNComp_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_group 64\n"
"dcl_cb cb0[1]\n"
"mov r0, vaTid0.x\n"
"dcl_literal l0, 16, 0, 0, 0\n"
"imul r0.x, r0.x, l0.x\n"
"mov g[r0.x], cb0[0]\n"
"mov g[r0.x+1], cb0[0]\n"
"mov g[r0.x+2], cb0[0]\n"
"mov g[r0.x+3], cb0[0]\n"
"mov g[r0.x+4], cb0[0]\n"
"mov g[r0.x+5], cb0[0]\n"
"mov g[r0.x+6], cb0[0]\n"
"mov g[r0.x+7], cb0[0]\n"
"mov g[r0.x+8], cb0[0]\n"
"mov g[r0.x+9], cb0[0]\n"
"mov g[r0.x+10], cb0[0]\n"
"mov g[r0.x+11], cb0[0]\n"
"mov g[r0.x+12], cb0[0]\n"
"mov g[r0.x+13], cb0[0]\n"
"mov g[r0.x+14], cb0[0]\n"
"mov g[r0.x+15], cb0[0]\n"
"end\n";

const char kernelAddR_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_cb cb0[1]\n"

"ftoi r0.x, cb0[0].w"
"ilt r0.y, vaTid0.x, r0.x\n"

"if_logicalnz r0.y\n"
"	itof r0.z, vaTid0.x\n"				// r0.z := thread_index
"	mul r0.y, r0.z, cb0[0].y\n"			// r0.y := thread_index*(1/width) == y index
"	mod r0.x, r0.z, cb0[0].x\n"			// r0.x := thread_index % width == x index
"	flr r0.xy, r0.xy\n"					// [x,y] := floor(r0.xy)
"	mad r1.x, r0.y, cb0[0].z, r0.x\n"	// index := y*pitch + x - index with account of the alignment pitch
"	ftoi r1.x, r1.x"					// convert to integer
"	sample_resource(0)_sampler(0) r2, r0.xy\n"
"	sample_resource(1)_sampler(1) r3, r0.xy\n"
"	add r4, r2, r3\n"
"	mov g[r1.x], r4\n"
"endif\n"

"end\n";

const char kernelSubR_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_cb cb0[1]\n"

"ftoi r0.x, cb0[0].w"
"ilt r0.y, vaTid0.x, r0.x\n"

"if_logicalnz r0.y\n"
"	itof r0.z, vaTid0.x\n"				// r0.z := thread_index
"	mul r0.y, r0.z, cb0[0].y\n"			// r0.y := thread_index*(1/width) == y index
"	mod r0.x, r0.z, cb0[0].x\n"			// r0.x := thread_index % width == x index
"	flr r0.xy, r0.xy\n"					// [x,y] := floor(r0.xy)
"	mad r1.x, r0.y, cb0[0].z, r0.x\n"	// index := y*pitch + x - index with account of the alignment pitch
"	ftoi r1.x, r1.x"					// convert to integer
"	sample_resource(0)_sampler(0) r2, r0.xy\n"
"	sample_resource(1)_sampler(1) r3, r0.xy\n"
"	sub r4, r2, r3\n"
"	mov g[r1.x], r4\n"
"endif\n"

"end\n";

const char kernelEwMulR_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_cb cb0[1]\n"

"ftoi r0.x, cb0[0].w"
"ilt r0.y, vaTid0.x, r0.x\n"

"if_logicalnz r0.y\n"
"	itof r0.z, vaTid0.x\n"				// r0.z := thread_index
"	mul r0.y, r0.z, cb0[0].y\n"			// r0.y := thread_index*(1/width) == y index
"	mod r0.x, r0.z, cb0[0].x\n"			// r0.x := thread_index % width == x index
"	flr r0.xy, r0.xy\n"					// [x,y] := floor(r0.xy)
"	mad r1.x, r0.y, cb0[0].z, r0.x\n"	// index := y*pitch + x - index with account of the alignment pitch
"	ftoi r1.x, r1.x"					// convert to integer
"	sample_resource(0)_sampler(0) r2, r0.xy\n"
"	sample_resource(1)_sampler(1) r3, r0.xy\n"
"	mul r4, r2, r3\n"
"	mov g[r1.x], r4\n"
"endif\n"

"end\n";

const char kernelEwDivR_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_cb cb0[1]\n"

"ftoi r0.x, cb0[0].w"
"ilt r0.y, vaTid0.x, r0.x\n"

"if_logicalnz r0.y\n"
"	itof r0.z, vaTid0.x\n"				// r0.z := thread_index
"	mul r0.y, r0.z, cb0[0].y\n"			// r0.y := thread_index*(1/width) == y index
"	mod r0.x, r0.z, cb0[0].x\n"			// r0.x := thread_index % width == x index
"	flr r0.xy, r0.xy\n"					// [x,y] := floor(r0.xy)
"	mad r1.x, r0.y, cb0[0].z, r0.x\n"	// index := y*pitch + x - index with account of the alignment pitch
"	ftoi r1.x, r1.x"					// convert to integer
"	sample_resource(0)_sampler(0) r2, r0.xy\n"
"	sample_resource(1)_sampler(1) r3, r0.xy\n"
"	div_zeroop(zero) r4, r2, r3\n"
"	mov g[r1.x], r4\n"
"endif\n"

"end\n";

const char kernelMatVecR_CS[] =
"il_cs_2_0\n"
"dcl_num_thread_per_blk 64\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_cb cb0[1]\n"

//"ftoi r0, cb0[0]"	// r0.x := width, r0.y := height; r0.z := lastRow; r0.w := result.physNumElements

// lastRow - index of last row quadruple

//"ilt r1.x, vaTid0.x, r0.w\n"
//"if_logicalnz r1.x\n"	// if thread_index < result.physNumElements

//"	ilt r1.x, vaTid0.x, r0.z\n"
//"	if_logicalnz r1.x\n"		// if thread_index < lastRow (the case of processing 4 rows at once)

"		mov r0, r0.0000\n"			// r0.xy is index of the first rhs element
"		itof r0.w, vaTid0.x\n"		// r0.w := thread_index	== y/4

		// r0.zw -> 2D position of first input row
"		dcl_literal l2, 0.0f, 0.0f, 0.0f, 4.0f\n"
"		mul r0.w, r0.w, l2.w\n"		// multiply "y" coordinate by 4

"		add r1.0y, r0.w, r1.1\n"	// r1.xy - 2D position of second input row
"		add r1.z, r1.y, r1.1\n"		// r1.xz - 2D position of third input row
"		add r1.w, r1.z, r1.1\n"		// r1.xw - 2D position of fourth input row

		// clear madd accumulators
"		mov r9, r9.0000\n"
"		mov r10, r10.0000\n"
"		mov r11, r11.0000\n"
"		mov r12, r12.0000\n"

"		mov r2.x, r2.0\n"			// r2.x := 0 (loop counter)
"		mov r3.x, cb0[0].x\n"		// r3.x := width

		// loop over "width" quadruples
"		whileloop\n"
"		  ge r2.y, r2.x, r3.x\n"	// while(loop counter < width)
"			break_logicalnz r2.y\n"

			// load next quadruple of rhs
"			sample_resource(1)_sampler(1) r4, r0.xy\n"

			// load next quadruples of each row
"			sample_resource(0)_sampler(0) r5, r0.zw\n"
"			sample_resource(0)_sampler(0) r6, r1.xy\n"
"			sample_resource(0)_sampler(0) r7, r1.xz\n"
"			sample_resource(0)_sampler(0) r8, r1.xw\n"

			// do element wise multiply	
"			mad r9, r4, r5, r9\n"
"			mad r10, r4, r6, r10\n"
"			mad r11, r4, r7, r11\n"
"			mad r12, r4, r8, r12\n"

"			add r0.xz, r0.xz, r0.11\n"	// increment rhs and first row column index
"			add r1.x, r1.x, r1.1\n"		// increment column index for all other rows

"			add r2.x, r2.x, r2.1\n"	// loop counter ++
"		endloop\n"

		// now do final horizontal add
"		dp4 r2.x, r9, r9.1111\n"	// r +* ones == r.x+r.y+r.z+r.w
"		dp4 r2.y, r10, r10.1111\n"
"		dp4 r2.z, r11, r11.1111\n"
"		dp4 r2.w, r12, r12.1111\n"

		// write the result
"		mov g[vaTid0.x], r2"

//"	endif\n"
//"else"
//"	mov g[vaTid0.x], r2.0000\n"
//"endif\n"

"end\n";