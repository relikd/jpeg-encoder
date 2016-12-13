//
//  Mat.hpp
//  jpegenc
//
//  Created by Marcel Groß on 13.12.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef Mat_hpp
#define Mat_hpp

#include <stdio.h>

struct Mat {
	float** mat;
	int N=0;
	
	Mat(int N): N(N) {
		mat = new float*[N];
		for (int i = 0; i < N; ++i) {
			mat[i] = new float[N];
		}
	}
};


#endif /* Mat_hpp */
