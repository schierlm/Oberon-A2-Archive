#pragma once

enum KernelCode
{
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

// matrix vector multiplication
KernMatVecR_PS,
KernMatVec4PartsR_PS,
KernMatVec8PartsR_PS,

// matrix multiplication
KernMatMul88Parts8x4by4x4R_PS,

// split a matrix to parts
KernSplitMatrixTo4Parts_PS,
KernSplitMatrixTo8Parts_PS,

// reshape an array to matrix
//KernReshapeToMatrixR_PS,

NKernels		// total number of kernels
};

/*
	Addition
*/
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

/* 
	Subtract
*/
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

/* 
	Elementwise multiply
*/
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

/*
	Elementwise divide
*/
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

/*
	Matrix vector multiplication
*/
const char kernelMatVecR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"

"dcl_cb cb0[1]\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 0.0f, 0.0f, 0.0f, 4.0f\n"

"mov r0, r0.0000\n"	// r0.xy is index of the first rhs element

// r0.zw -> 2D position of first input row
"flr r0.___w, vWinCoord0.x\n"
"mul r0.w, r0.w, l0.w\n"

"mov r1.0yzw, r0.w\n"			
"add r1.y, r1.y, r1.1\n"		// 2D position of second input row
"add r1.z, r1.y, r1.1\n"		// 2D position of third input row
"add r1.w, r1.z, r1.1\n"		// 2D position of fourth input row

"sub r0.xz, r0.xz, r0.11\n"		// account first increment
"sub r1.x, r1.x, r1.1\n"		// account first increment

"mov r20, r20.0000\n"
"mov r21, r21.0000\n"
"mov r22, r22.0000\n"
"mov r23, r23.0000\n"

"mov r10, r10.0000\n"			// loop counter
"sub r10.x, r10.x, r10.1\n"		// account first increment

"mov r11, cb0[0]\n"

"whileloop\n"

	// increment counters of row elements
"	add r0.xz, r0.xz, r0.11\n"
"	add r1.x, r1.x, r1.1\n"

"	add r10.x, r10.x, r10.1\n"	// loop counter ++
"   ge r10.y, r10.x, r11.x\n"	// while(loop counter < cb0[0].x)
"   break_logicalnz r10.y\n"

	// load next part of rhs
"	sample_resource(1)_sampler(1) r5, r0.xy\n"

	// load netx parts of rows
"	sample_resource(0)_sampler(0) r6, r0.zw\n"
"	sample_resource(0)_sampler(0) r7, r1.xy\n"
"	sample_resource(0)_sampler(0) r8, r1.xz\n"
"	sample_resource(0)_sampler(0) r9, r1.xw\n"

	// do elementwise multiply and accumulate	
"	mad r20, r5, r6, r20\n"
"	mad r21, r5, r7, r21\n"
"	mad r22, r5, r8, r22\n"
"	mad r23, r5, r9, r23\n"
"endloop\n"

// now do final horizontal add
"dp4 r30.x, r20, r20.1111\n"	// r +* ones == r.x+r.y+r.z+r.w
"dp4 r30.y, r21, r21.1111\n"
"dp4 r30.z, r22, r22.1111\n"
"dp4 r30.w, r23, r23.1111\n"

"mov o0, r30\n"

"end\n";

/*
	Matrix vector multiplication for the case when matrix is splitted into 8 parts
*/
const char kernelMatVec8PartsR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"

"dcl_cb cb0[1]\n"

"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(2)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(3)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(4)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(5)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(6)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(7)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_resource_id(8)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 0.0f, 0.5f, 0.0f, 0.0f\n"

"mov r0, r0.0000\n"	// r0.xy is [x,y] of the first rhs element

"flr r2.y, vWinCoord0.x\n"
"mul r2.y, r2.y, l0.y\n"
"flr r1.0y, r2.y\n"			// r1.xy -> [x,y] of first element in input row
"sub r2.y, r2.y, r1.y\n"	// remainder of division by 2

"sub r0.x, r0.x, r0.1\n"		// account first increment
"sub r1.x, r1.x, r1.1\n"		// account first increment

"mov r20, r20.0000\n"
"mov r21, r21.0000\n"
"mov r22, r22.0000\n"
"mov r23, r23.0000\n"

"mov r10, r10.0000\n"			// loop counter
"sub r10.x, r10.x, r10.1\n"		// account first increment

"mov r11, cb0[0]\n"

"eq r2.z, r2.y, r2.0\n"
"if_logicalnz r2.z\n"	// if r2.y == 0

"	whileloop\n"

		// increment counters of row elements
"		add r0.x, r0.x, r0.1\n"
"		add r1.x, r1.x, r1.1\n"

"		add r10.x, r10.x, r10.1\n"	// loop counter ++
"		ge r10.y, r10.x, r11.x\n"	// while(loop counter < cb0[0].x)
"		break_logicalnz r10.y\n"

		// load next part of rhs
"		sample_resource(8)_sampler(8) r5, r0.xy\n"

		// load next parts from 4 rows
"		sample_resource(0)_sampler(0) r6, r1.xy\n"
"		sample_resource(1)_sampler(1) r7, r1.xy\n"
"		sample_resource(2)_sampler(2) r8, r1.xy\n"
"		sample_resource(3)_sampler(3) r9, r1.xy\n"

		// do elementwise multiply	
"		mad r20, r5, r6, r20\n"
"		mad r21, r5, r7, r21\n"
"		mad r22, r5, r8, r22\n"
"		mad r23, r5, r9, r23\n"
"	endloop\n"

"else\n"

"	whileloop\n"

		// increment counters of row elements
"		add r0.x, r0.x, r0.1\n"
"		add r1.x, r1.x, r1.1\n"

"		add r10.x, r10.x, r10.1\n"	// loop counter ++
"		ge r10.y, r10.x, r11.x\n"	// while(loop counter < cb0[0].x)
"		break_logicalnz r10.y\n"

		// load next part of rhs
"		sample_resource(8)_sampler(8) r5, r0.xy\n"

		// load next parts from 4 rows
"		sample_resource(4)_sampler(4) r6, r1.xy\n"
"		sample_resource(5)_sampler(5) r7, r1.xy\n"
"		sample_resource(6)_sampler(6) r8, r1.xy\n"
"		sample_resource(7)_sampler(7) r9, r1.xy\n"

		// do elementwise multiply	
"		mad r20, r5, r6, r20\n"
"		mad r21, r5, r7, r21\n"
"		mad r22, r5, r8, r22\n"
"		mad r23, r5, r9, r23\n"
"	endloop\n"

"endif\n"

// now do final horizontal add
"dp4 r30.x, r20, r20.1111\n"	// r +* ones == r.x+r.y+r.z+r.w
"dp4 r30.y, r21, r21.1111\n"
"dp4 r30.z, r22, r22.1111\n"
"dp4 r30.w, r23, r23.1111\n"

"mov o0, r30\n"

"end\n";

/*
	Matrix vector multiplication for the case when matrix is splitted into 4 parts
*/
const char kernelMatVec4PartsR_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_output_generic o0\n"

"dcl_cb cb0[1]\n"

"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(2)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(3)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_resource_id(4)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 0.0f, 0.5f, 0.0f, 0.0f\n"

"mov r0, r0.0000\n"	// r0.xy is [x,y] of the first rhs element
"mov r1.0y, vWinCoord0.x\n"	// r1.xy -> [x,y] of first element in input row

"sub r0.x, r0.x, r0.1\n"		// account first increment
"sub r1.x, r1.x, r1.1\n"		// account first increment

"mov r20, r20.0000\n"
"mov r21, r21.0000\n"
"mov r22, r22.0000\n"
"mov r23, r23.0000\n"

"mov r10, r10.0000\n"			// loop counter
"sub r10.x, r10.x, r10.1\n"		// account first increment

"mov r11, cb0[0]\n"


"whileloop\n"

	// increment counters of row elements
"	add r0.x, r0.x, r0.1\n"
"	add r1.x, r1.x, r1.1\n"

"	add r10.x, r10.x, r10.1\n"	// loop counter ++
"	ge r10.y, r10.x, r11.x\n"	// while(loop counter < cb0[0].x)
"	break_logicalnz r10.y\n"

	// load next part of rhs
"	sample_resource(4)_sampler(4) r5, r0.xy\n"

	// load next parts from 4 rows
"	sample_resource(0)_sampler(0) r6, r1.xy\n"
"	sample_resource(1)_sampler(1) r7, r1.xy\n"
"	sample_resource(2)_sampler(2) r8, r1.xy\n"
"	sample_resource(3)_sampler(3) r9, r1.xy\n"

	// do elementwise multiply	
"	mad r20, r5, r6, r20\n"
"	mad r21, r5, r7, r21\n"
"	mad r22, r5, r8, r22\n"
"	mad r23, r5, r9, r23\n"
"endloop\n"

// now do final horizontal add
"dp4 r30.x, r20, r20.1111\n"	// r +* ones == r.x+r.y+r.z+r.w
"dp4 r30.y, r21, r21.1111\n"
"dp4 r30.z, r22, r22.1111\n"
"dp4 r30.w, r23, r23.1111\n"

"mov o0, r30\n"

"end\n";

/*
	Matrix multiplication for the case when both A and B are splitted in 8 parts

	performs series of 8x4 by 4x4 matrix multiplications without loop unrolling
*/
const char kernelMatMul88Parts8x4by4x4R_PS[] = 
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_cb cb0[1]\n"	// [A.width,...]

"dcl_output_generic o0\n"
"dcl_output_generic o1\n"
"dcl_output_generic o2\n"
"dcl_output_generic o3\n"
"dcl_output_generic o4\n"
"dcl_output_generic o5\n"
"dcl_output_generic o6\n"
"dcl_output_generic o7\n"

// parts of A
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(1)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(2)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(3)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(4)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(5)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(6)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(7)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

// parts of B
"dcl_resource_id(8)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(9)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(10)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(11)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(12)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(13)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(14)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"
"dcl_resource_id(15)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 2.0f, 0.5f, 0.0f, 0.0f\n"

// initialize sample counters of A
"mov r0.0y, vWinCoord0.y\n"				// r0 := [0,y]
"sub r0.x, r0.x, r0.1\n"				// account first increment

// initialize sample counter of B
"mov r1.x0, vWinCoord0.x\n"				// [x,0]
"sub r1.y, r1.y, l0.y\n"				// account first increment

// clear float4 accumulators for 8x4 * 4x4 matrix multiply result
"mov r34, r34.0000\n"	
"mov r35, r35.0000\n"
"mov r36, r36.0000\n"
"mov r37, r37.0000\n"
"mov r38, r38.0000\n"	
"mov r39, r39.0000\n"
"mov r40, r40.0000\n"
"mov r41, r41.0000\n"

"mov r2.0y00, cb0[0].x\n"				// r2.x is the loop counter, r2.y := A.width
"sub r2.x, r2.x, r2.1\n"				// account first increment

"whileloop\n"

	// increment sample counter of B
"	add r1.y, r1.y, l0.y\n"

	// increment sample counter of A
"	add r0.x, r0.x, r0.1\n"

"	add r2.x, r2.x, r2.1\n"	// loop counter ++

"   ge r2.z, r2.x, r2.y\n"	// while(loop counter < A.width)
"   break_logicalnz r2.z\n"

	// load next 4x4 block of B

"	mod r3.x, r2.x, l0.x\n"	// r3.x := r2.x % 2

"	eq r3.y, r3.x, r3.0\n"
"	if_logicalnz r3.y\n"	// if r3.x == 0
"		sample_resource(8)_sampler(8) r10, r1.xy00\n"
"		sample_resource(9)_sampler(9) r11, r1.xy00\n"
"		sample_resource(10)_sampler(10) r12, r1.xy00\n"
"		sample_resource(11)_sampler(11) r13, r1.xy00\n"
"	else\n"
"		sample_resource(12)_sampler(12) r10, r1.xy00\n"
"		sample_resource(13)_sampler(13) r11, r1.xy00\n"
"		sample_resource(14)_sampler(14) r12, r1.xy00\n"
"		sample_resource(15)_sampler(15) r13, r1.xy00\n"
"	endif\n"

	// load next 8x4 block of A
"	sample_resource(0)_sampler(0) r26, r0.xy00\n"
"	sample_resource(1)_sampler(1) r27, r0.xy00\n"
"	sample_resource(2)_sampler(2) r28, r0.xy00\n"
"	sample_resource(3)_sampler(3) r29, r0.xy00\n"
"	sample_resource(4)_sampler(4) r30, r0.xy00\n"
"	sample_resource(5)_sampler(5) r31, r0.xy00\n"
"	sample_resource(6)_sampler(6) r32, r0.xy00\n"
"	sample_resource(7)_sampler(7) r33, r0.xy00\n"

	// compute Ablk * Bblk

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

"endloop\n"

// store the result
"mov o0, r34\n"
"mov o1, r35\n"
"mov o2, r36\n"
"mov o3, r37\n"
"mov o4, r38\n"
"mov o5, r39\n"
"mov o6, r40\n"
"mov o7, r41\n"

"end\n";

/*
	Reshape an array to matrix
*/
const char kernelReshapeToMatrixR_PS[] =
"il_ps_2_0\n"
"dcl_cb cb0[1]\n"	// [pitch,delta,]
"dcl_input vObjIndex0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"



"end\n";

/*
	Split a matrix to 4 parts
*/
const char kernelSplitMatrixTo4Parts_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 3.0f, 4.0f, 1.0f, 2.0f\n"

"flr r0.xy, vWinCoord0.xy\n"
"mul r0.y, r0.y, l0.y\n"		// r1.xy := [x,y*4] - 2D position of the first row to copy

"add r0.__zw, r0.yy, l0.zw\n"	// r0 := [x,y,y+1,y+2]
"add r1.xy, r0.xy, l0.0x\n"		// r1 := [x,y+3]

"dcl_output_generic o0\n"
"dcl_output_generic o1\n"
"dcl_output_generic o2\n"
"dcl_output_generic o3\n"

"sample_resource(0)_sampler(0) o0, r0.xy\n"
"sample_resource(0)_sampler(0) o1, r0.xz\n"
"sample_resource(0)_sampler(0) o2, r0.xw\n"
"sample_resource(0)_sampler(0) o3, r1.xy\n"

"end\n";

/*
	Split a matrix to 8 parts
*/
const char kernelSplitMatrixTo8Parts_PS[] =
"il_ps_2_0\n"
"dcl_input_position_interp(linear_noperspective) vWinCoord0.xy__\n"
"dcl_resource_id(0)_type(2d,unnorm)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)\n"

"dcl_literal l0, 3.0f, 8.0f, 1.0f, 2.0f\n"

"flr r0.xy, vWinCoord0.xy\n"
"mul r0.y, r0.y, l0.y\n"				// r1.xy := [x,y*8] - 2D position of the first row to copy

"add r0.__zw, r0.yy, l0.zw\n"			// r0 := [x,y,y+1,y+2]
"add r1.xyzw, r0.xyzw, l0.0xxx\n"		// r1 := [x,y+3,y+4,y+5]
"add r2.xyz, r1.xyz, l0.0xx\n"			// r2 := [x,y+6,y+7]

"dcl_output_generic o0\n"
"dcl_output_generic o1\n"
"dcl_output_generic o2\n"
"dcl_output_generic o3\n"
"dcl_output_generic o4\n"
"dcl_output_generic o5\n"
"dcl_output_generic o6\n"
"dcl_output_generic o7\n"

"sample_resource(0)_sampler(0) o0, r0.xy\n"
"sample_resource(0)_sampler(0) o1, r0.xz\n"
"sample_resource(0)_sampler(0) o2, r0.xw\n"
"sample_resource(0)_sampler(0) o3, r1.xy\n"
"sample_resource(0)_sampler(0) o4, r1.xz\n"
"sample_resource(0)_sampler(0) o5, r1.xw\n"
"sample_resource(0)_sampler(0) o6, r2.xy\n"
"sample_resource(0)_sampler(0) o7, r2.xz\n"

"end\n";

class Kernel
{
public:
	Kernel(KernelCode iKernel, CALtarget target, CALresult* err);
	~Kernel(void);	

	KernelCode iKernel;			// kernel code
	CALobject obj;				// CAL kernel object
	CALimage img;				// CAL kernel image

	long nInputs;				// number of kernel inputs
	long nOutputs;				// number of kernel outputs			
	long nConstants;			// number of kernel constants
	long* constSizes;			// size for each constant
	CALformat* constFormats;	// data format for each constant	

	BOOL usesGlobalBuffer;		// TRUE when kernel uses a global buffer
};
