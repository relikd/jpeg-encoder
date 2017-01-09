/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

/* Matrix transpose with OpenCL
* Device code.
*/

// https://developer.nvidia.com/opencl

#define BLOCK_DIM 8

// This kernel is optimized to ensure all global reads and writes are coalesced,
// and to avoid bank conflicts in shared memory.  This kernel is up to 11x faster
// than the naive kernel below.  Note that the shared memory array is sized to 
// (BLOCK_DIM+1)*BLOCK_DIM.  This pads each row of the 2D block in shared memory 
// so that bank conflicts do not occur when threads address the array column-wise.
__kernel void transpose(__global float *odata, __global float *idata, int offset, int width, int height, __local float* block)
{
	// read the matrix tile into shared memory
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);

	if((xIndex + offset < width) && (yIndex < height))
	{
		unsigned int index_in = yIndex * width + xIndex + offset;
		block[get_local_id(1) * (BLOCK_DIM + 1) + get_local_id(0)] = idata[index_in];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	// write the transposed matrix tile to global memory
	xIndex = get_group_id(1) * BLOCK_DIM + get_local_id(0);
	yIndex = get_group_id(0) * BLOCK_DIM + get_local_id(1);
	if((xIndex < height) && (yIndex + offset < width))
    {
//		unsigned int index_out = yIndex * height + xIndex;
		unsigned int off_x = (xIndex / BLOCK_DIM) * BLOCK_DIM;
		unsigned int off_y = (yIndex / BLOCK_DIM) * BLOCK_DIM;
		unsigned int idx_x = xIndex - off_x;
		unsigned int idx_y = yIndex - off_y;
		unsigned int index_out = (off_y + idx_x) + width * (off_x + idx_y);
		odata[index_out] = block[get_local_id(0) * (BLOCK_DIM + 1) + get_local_id(1)];
	}
}

// This naive transpose kernel suffers from completely non-coalesced writes.
// It can be up to 10x slower than the kernel above for large matrices.
__kernel void transpose_naive(__global float *odata, __global float* idata, int offset, int width, int height)
{
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);
	
	if (xIndex + offset < width && yIndex < height)
	{
		unsigned int index_in  = xIndex + offset + width * yIndex;
		unsigned int index_out = yIndex + height * xIndex;
		odata[index_out] = idata[index_in];
	}
}

__kernel void transpose_naive8(__global float *odata, __global float* idata, int offset, int width, int height)
{
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);
	
	if (xIndex + offset < width && yIndex < height)
	{
		unsigned int index_in  = xIndex + offset + width * yIndex;
//		unsigned int index_out = yIndex + height * xIndex;
		unsigned int off_x = (xIndex / BLOCK_DIM) * BLOCK_DIM;
		unsigned int off_y = (yIndex / BLOCK_DIM) * BLOCK_DIM;
		unsigned int idx_x = xIndex - off_x;
		unsigned int idx_y = yIndex - off_y;
		unsigned int index_out = (off_y + idx_x) + width * (off_x + idx_y);
		odata[index_out] = idata[index_in];
	}
}

__kernel void shared_copy(__global float *odata, __global float *idata, int offset, int width, int height, __local float* block)
{
	// read the matrix tile into shared memory
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);
	
	unsigned int index_in = yIndex * width + xIndex + offset;
	if((xIndex + offset < width) && (yIndex < height))
	{
		block[get_local_id(1) * (BLOCK_DIM + 1) + get_local_id(0)] = idata[index_in];
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);
	
	if((xIndex < height) && (yIndex+ offset < width))
	{
		odata[index_in] = block[get_local_id(1) * (BLOCK_DIM + 1) + get_local_id(0)];
	}
}

__kernel void arai_separated(__global float *odata, __global float *idata, __global float *matrixA, int offset, int width, int height, __local float* blockA, __local float* blockB)
{
	// read the matrix tile into shared memory
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);
	
	if ( (xIndex + offset < width) && (yIndex < height) )
	{
		unsigned int gIndex = yIndex * width + xIndex + offset;
		
		blockA[get_local_id(1) * (BLOCK_DIM + 1) + get_local_id(0)] = matrixA[get_local_id(1) * 8 + get_local_id(0)];
		blockB[get_local_id(1) * (BLOCK_DIM + 1) + get_local_id(0)] = idata[gIndex];
		
		barrier(CLK_LOCAL_MEM_FENCE);
		
		unsigned int rowa = get_local_id(1) * (BLOCK_DIM + 1);
		unsigned int rowb = get_local_id(0);
		
		odata[gIndex] =
		blockA[rowa + 0] * blockB[rowb + 0 * (BLOCK_DIM + 1)] +
		blockA[rowa + 1] * blockB[rowb + 1 * (BLOCK_DIM + 1)] +
		blockA[rowa + 2] * blockB[rowb + 2 * (BLOCK_DIM + 1)] +
		blockA[rowa + 3] * blockB[rowb + 3 * (BLOCK_DIM + 1)] +
		blockA[rowa + 4] * blockB[rowb + 4 * (BLOCK_DIM + 1)] +
		blockA[rowa + 5] * blockB[rowb + 5 * (BLOCK_DIM + 1)] +
		blockA[rowa + 6] * blockB[rowb + 6 * (BLOCK_DIM + 1)] +
		blockA[rowa + 7] * blockB[rowb + 7 * (BLOCK_DIM + 1)];
		
		barrier(CLK_GLOBAL_MEM_FENCE);
		
		blockA[get_local_id(1) * (BLOCK_DIM + 1) + get_local_id(0)] = odata[gIndex];
		blockB[get_local_id(0) * (BLOCK_DIM + 1) + get_local_id(1)] = matrixA[get_local_id(1) * 8 + get_local_id(0)];
		
		barrier(CLK_LOCAL_MEM_FENCE);
		
		odata[gIndex] =
		blockA[rowa + 0] * blockB[rowb + 0 * (BLOCK_DIM + 1)] +
		blockA[rowa + 1] * blockB[rowb + 1 * (BLOCK_DIM + 1)] +
		blockA[rowa + 2] * blockB[rowb + 2 * (BLOCK_DIM + 1)] +
		blockA[rowa + 3] * blockB[rowb + 3 * (BLOCK_DIM + 1)] +
		blockA[rowa + 4] * blockB[rowb + 4 * (BLOCK_DIM + 1)] +
		blockA[rowa + 5] * blockB[rowb + 5 * (BLOCK_DIM + 1)] +
		blockA[rowa + 6] * blockB[rowb + 6 * (BLOCK_DIM + 1)] +
		blockA[rowa + 7] * blockB[rowb + 7 * (BLOCK_DIM + 1)];
	}
}