#ifndef MINIREADER
#define MINIREADER

#include <iostream>
#include <stdlib.h>
#include "Reader.h"

namespace std {
	
	class MiniReader{
		
	public:
		MiniReader(char*, int*, int*, int*, int*);
		~MiniReader();
		
		int* loadImage();
		void getMinMax(int*, int*);
	
	private:
		Reader *reader;
		int width;
		int height;
		int white;
		int minVal;
		int maxVal;
		int nBits;
	};

}

#endif
