#define BLOCK_DIM 8

// This kernel is optimized to ensure all global reads and writes are coalesced,
// and to avoid bank conflicts in shared memory.  This kernel is up to 11x faster
// than the naive kernel below.  Note that the shared memory array is sized to 
// (BLOCK_DIM+1)*BLOCK_DIM.  This pads each row of the 2D block in shared memory 
// so that bank conflicts do not occur when threads address the array column-wise.
__kernel void dct_separated(__global float *odata, __global float *idata, __global float *matrixA, int width, int height, __local float* blockA, __local float* blockB)
{
	// read the matrix tile into shared memory
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);
	
	if ( (xIndex < width) && (yIndex < height) )
	{
		unsigned int gIndex = yIndex * width + xIndex;
		
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


__constant float A1 = 0.707106781186547524400844362104849039284835937688474036588F;
__constant float A2 = 0.541196100146196984399723205366389420061072063378015444681F;
__constant float A3 = 0.707106781186547524400844362104849039284835937688474036588F;
__constant float A4 = 1.306562964876376527856643173427187153583761188349269527548F;
__constant float A5 = 0.382683432365089771728459984030398866761344562485627041433F;

__constant float S0 = 0.353553390593273762200422181052424519642417968844237018294F;
__constant float S1 = 0.254897789552079584470969901993921956841309245954467684863F;
__constant float S2 = 0.270598050073098492199861602683194710030536031689007722340F;
__constant float S3 = 0.300672443467522640271860911954610917533627944800336361060F;
__constant float S4 = 0.353553390593273762200422181052424519642417968844237018294F;
__constant float S5 = 0.449988111568207852319254770470944197769000863706422492617F;
__constant float S6 = 0.653281482438188263928321586713593576791880594174634763774F;
__constant float S7 = 1.281457723870753089398043148088849954507561675693672456063F;

__kernel void dct_arai(__global float *odata, __global float *idata, int width, int height, __local float* block)
{
	// read the matrix tile into shared memory
	unsigned int xIndex = get_global_id(0);
	unsigned int yIndex = get_global_id(1);
	
	if ( get_local_id(0) == 0 && (xIndex < width) && (yIndex < height))
	{
		{ // {} to limit variable scope
			unsigned int inIndex = yIndex * width + xIndex;
			
			float a0 = idata[inIndex] + idata[inIndex + 7];
			float a7 = idata[inIndex] - idata[inIndex + 7];
			float a1 = idata[inIndex + 1] + idata[inIndex + 6];
			float a6 = idata[inIndex + 1] - idata[inIndex + 6];
			float a2 = idata[inIndex + 2] + idata[inIndex + 5];
			float a5 = idata[inIndex + 2] - idata[inIndex + 5];
			float a3 = idata[inIndex + 3] + idata[inIndex + 4];
			
			float b4 = -(idata[inIndex + 3] - idata[inIndex + 4]) - a5;
			float b0 = a0 + a3;
			float b1 = a1 + a2;
			float b3 = a0 - a3;
			float b6 = a6 + a7;
			
			float A5_block = (b4 + b6) * A5;
			
			float d2 = ((a1 - a2) + b3) * A1;
			float d4 = -(b4 * A2) - A5_block;
			float d5 = (a5 + a6) * A3;
			float d6 = (b6 * A4) - A5_block;
			
			float e5 = d5 + a7;
			float e7 = a7 - d5;
			
			unsigned int row = get_local_id(1);
			// put transposed result to local memory
			block[                       row ] = (b0 + b1) * S0;
			block[ 1 * (BLOCK_DIM + 1) + row ] = (e5 + d6) * S1;
			block[ 2 * (BLOCK_DIM + 1) + row ] = (d2 + b3) * S2;
			block[ 3 * (BLOCK_DIM + 1) + row ] = (e7 - d4) * S3;
			block[ 4 * (BLOCK_DIM + 1) + row ] = (b0 - b1) * S4;
			block[ 5 * (BLOCK_DIM + 1) + row ] = (d4 + e7) * S5;
			block[ 6 * (BLOCK_DIM + 1) + row ] = (b3 - d2) * S6;
			block[ 7 * (BLOCK_DIM + 1) + row ] = (e5 - d6) * S7;
		}
	
		barrier(CLK_LOCAL_MEM_FENCE); // wait till finished
		
		{
			unsigned int localOffset = get_local_id(1) * (BLOCK_DIM + 1);
			
			float a0 = block[localOffset] + block[localOffset + 7];
			float a7 = block[localOffset] - block[localOffset + 7];
			float a1 = block[localOffset + 1] + block[localOffset + 6];
			float a6 = block[localOffset + 1] - block[localOffset + 6];
			float a2 = block[localOffset + 2] + block[localOffset + 5];
			float a5 = block[localOffset + 2] - block[localOffset + 5];
			float a3 = block[localOffset + 3] + block[localOffset + 4];
			
			float b4 = -(block[localOffset + 3] - block[localOffset + 4]) - a5;
			float b0 = a0 + a3;
			float b1 = a1 + a2;
			float b3 = a0 - a3;
			float b6 = a6 + a7;
			
			float A5_block = (b4 + b6) * A5;
			
			float d2 = ((a1 - a2) + b3) * A1;
			float d4 = -(b4 * A2) - A5_block;
			float d5 = (a5 + a6) * A3;
			float d6 = (b6 * A4) - A5_block;
			
			float e5 = d5 + a7;
			float e7 = a7 - d5;
			
			unsigned int outIndex = (yIndex - get_local_id(1)) * width + (xIndex + get_local_id(1));
			odata[outIndex] = (b0 + b1) * S0;
			odata[outIndex + 1 * width] = (e5 + d6) * S1;
			odata[outIndex + 2 * width] = (d2 + b3) * S2;
			odata[outIndex + 3 * width] = (e7 - d4) * S3;
			odata[outIndex + 4 * width] = (b0 - b1) * S4;
			odata[outIndex + 5 * width] = (d4 + e7) * S5;
			odata[outIndex + 6 * width] = (b3 - d2) * S6;
			odata[outIndex + 7 * width] = (e5 - d6) * S7;
		}
	}
}
