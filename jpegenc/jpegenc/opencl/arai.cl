#define BLOCK_DIM 9 // (BLOCK_DIM + 1)

__constant float matrixA[64] = {
	0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,
	0.490392640201615215289621119154617190361022949218750000F,  0.415734806151272617835701339572551660239696502685546875F,  0.277785116509801144335511935423710383474826812744140625F,  0.097545161008064151797469776283833198249340057373046875F, -0.097545161008064096286318545026006177067756652832031250F, -0.277785116509800977802058241650229319930076599121093750F, -0.415734806151272673346852570830378681421279907226562500F, -0.490392640201615215289621119154617190361022949218750000F,
	0.461939766255643369241568052530055865645408630371093750F,  0.191341716182544918645191955874906852841377258300781250F, -0.191341716182544863134040724617079831659793853759765625F, -0.461939766255643369241568052530055865645408630371093750F, -0.461939766255643369241568052530055865645408630371093750F, -0.191341716182545168445372496535128448158502578735351562F,  0.191341716182545001911918802761647384613752365112304688F,  0.461939766255643258219265590014401823282241821289062500F,
	0.415734806151272617835701339572551660239696502685546875F, -0.097545161008064096286318545026006177067756652832031250F, -0.490392640201615215289621119154617190361022949218750000F, -0.277785116509801088824360704165883362293243408203125000F,  0.277785116509800922290907010392402298748493194580078125F,  0.490392640201615215289621119154617190361022949218750000F,  0.097545161008064387719862509129598038271069526672363281F, -0.415734806151272562324550108314724639058113098144531250F,
	0.353553390593273786368655464684707112610340118408203125F, -0.353553390593273730857504233426880091428756713867187500F, -0.353553390593273841879806695942534133791923522949218750F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273841879806695942534133791923522949218750F, -0.353553390593273342279445614622090943157672882080078125F, -0.353553390593273619835201770911226049065589904785156250F,  0.353553390593273286768294383364263921976089477539062500F,
	0.277785116509801144335511935423710383474826812744140625F, -0.490392640201615215289621119154617190361022949218750000F,  0.097545161008064137919681968469376442953944206237792969F,  0.415734806151272784369155033346032723784446716308593750F, -0.415734806151272562324550108314724639058113098144531250F, -0.097545161008064013019591698139265645295381546020507812F,  0.490392640201615326311923581670271232724189758300781250F, -0.277785116509800755757453316618921235203742980957031250F,
	0.191341716182544918645191955874906852841377258300781250F, -0.461939766255643369241568052530055865645408630371093750F,  0.461939766255643258219265590014401823282241821289062500F, -0.191341716182544918645191955874906852841377258300781250F, -0.191341716182545279467674959050782490521669387817382812F,  0.461939766255643369241568052530055865645408630371093750F, -0.461939766255643147196963127498747780919075012207031250F,  0.191341716182544779867313877730339299887418746948242188F,
	0.097545161008064151797469776283833198249340057373046875F, -0.277785116509801088824360704165883362293243408203125000F,  0.415734806151272784369155033346032723784446716308593750F, -0.490392640201615326311923581670271232724189758300781250F,  0.490392640201615215289621119154617190361022949218750000F, -0.415734806151272506813398877056897617876529693603515625F,  0.277785116509800755757453316618921235203742980957031250F, -0.097545161008064276697560046613943995907902717590332031F
};

// This kernel is optimized to ensure all global reads and writes are coalesced,
// and to avoid bank conflicts in shared memory.  This kernel is up to 11x faster
// than the naive kernel below.  Note that the shared memory array is sized to 
// (BLOCK_DIM+1)*BLOCK_DIM.  This pads each row of the 2D block in shared memory 
// so that bank conflicts do not occur when threads address the array column-wise.
__kernel void dct_separated(__global float *odata, __global float *idata, unsigned int width, __local float* memA, __local float* memB, __local float* memC)
{
	const unsigned int gIndex = get_global_id(1) * width + get_global_id(0);
	const unsigned int localIndex = get_local_id(1) * BLOCK_DIM + get_local_id(0);
	
	// read the matrix tile into shared memory (cause its blazing fast)
	memA[ localIndex ] = matrixA[ get_local_id(1) * 8 + get_local_id(0) ];
	memB[ localIndex ] = idata[ gIndex ];
	
	barrier(CLK_LOCAL_MEM_FENCE);
	{
		float sum = 0;
		int i = 8;
		while (i--) {
			sum += memA[ get_local_id(1) * BLOCK_DIM + i ]  *  memB[ i * BLOCK_DIM + get_local_id(0) ];
		}
		memC[ localIndex ] = sum;
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	{
		float sum = 0;
		int i = 8;
		while (i--) {
			sum += memC[ get_local_id(1) * BLOCK_DIM + i ]  *  memA[ get_local_id(0) * BLOCK_DIM + i ];
		}
		odata[ gIndex ] = sum;
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

__kernel void dct_arai(__global float *odata, __global float *idata, unsigned int width, __local float* mem)
{
	unsigned int gIndex = get_global_id(1) * width + get_global_id(0);
	const unsigned int offset = get_local_id(1) * BLOCK_DIM;
	
	// copy data pairwise (0+7,0-7)
	const unsigned int correspondingIndex = gIndex - get_local_id(0) - get_local_id(0) + 7;
	mem[ offset + get_local_id(0) ] = ( (get_local_id(0) < 4)
									   ? idata[gIndex] + idata[correspondingIndex]
									   : idata[correspondingIndex] - idata[gIndex] );
	barrier(CLK_LOCAL_MEM_FENCE);
	
	if (get_local_id(0) == 0) // process only if beginning of row
	{
		float b0 = mem[offset + 0] + mem[offset + 3];
		float b1 = mem[offset + 1] + mem[offset + 2];
		float b3 = mem[offset + 0] - mem[offset + 3];
		float b4 = -(mem[offset + 4]) - mem[offset + 5];
		float b6 = mem[offset + 6] + mem[offset + 7];
		
		float A5_block = (b4 + b6) * A5;
		
		float d2 = ((mem[offset + 1] - mem[offset + 2]) + b3) * A1;
		float d4 = -(b4 * A2) - A5_block;
		float d5 = (mem[offset + 5] + mem[offset + 6]) * A3;
		float d6 = (b6 * A4) - A5_block;
		
		float e5 = d5 + mem[offset + 7];
		float e7 = mem[offset + 7] - d5;
		
		// get transposed index inside block
		gIndex = gIndex - (get_local_id(1) * width) + get_local_id(1);
		
		odata[ gIndex + 0 * width ] = (b0 + b1) * S0;
		odata[ gIndex + 1 * width ] = (e5 + d6) * S1;
		odata[ gIndex + 2 * width ] = (d2 + b3) * S2;
		odata[ gIndex + 3 * width ] = (e7 - d4) * S3;
		odata[ gIndex + 4 * width ] = (b0 - b1) * S4;
		odata[ gIndex + 5 * width ] = (d4 + e7) * S5;
		odata[ gIndex + 6 * width ] = (b3 - d2) * S6;
		odata[ gIndex + 7 * width ] = (e5 - d6) * S7;
	}
}
