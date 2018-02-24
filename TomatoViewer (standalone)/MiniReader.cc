#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "MiniReader.h"
#include "Reader.h"

namespace std{

MiniReader::MiniReader(char *fileName, int *width, int *height, int *white, int *nBits){
	Reader *lReader = new Reader();
	this->reader = lReader;
	
	reader->open(fileName);
	reader->readString(3);

	int lWidth = 0;
	int cifra = reader->read(8);
	
	while(cifra != ' '){
		lWidth = 10*lWidth + (cifra - '0');
		cifra = reader->read(8);
	}
	
	int lHeight = 0;
	cifra = reader->read(8);
	
	while(cifra != '\n'){
		lHeight = 10*lHeight + (cifra - '0');
		cifra = reader->read(8);
	}
	
	int lWhite = 0;
	cifra = reader->read(8);
	
	while(cifra != '\n'){
		lWhite = 10*lWhite + (cifra - '0');
		cifra = reader->read(8);
	}
	
	int lnBits = ((lWhite <= 0xFF) ? 8 : 16);
	
	this->width  = lWidth;
	this->height = lHeight;
	this->white  = lWhite;
	this->nBits  = lnBits;
	
	*width  = lWidth;
	*height = lHeight;
	*white  = lWhite;
	*nBits  = lnBits;
}

int *MiniReader::loadImage(){
	int *lImage = new int[this->width * this->height];
	int lMinVal = this->white, lMaxVal = 0;
	
	for(int px=0; px < this->width * this->height; px++){
		lImage[px] = reader->read(this->nBits);
		if(lImage[px] < lMinVal) lMinVal = lImage[px];
		if(lImage[px] > lMaxVal) lMaxVal = lImage[px];
	}
	
	this->minVal = lMinVal;
	this->maxVal = lMaxVal;
	
	return lImage;
}

void MiniReader::getMinMax(int* lMinVal, int* lMaxVal){
	*lMinVal = this->minVal;
	*lMaxVal = this->maxVal;
}


MiniReader::~MiniReader(){}

}