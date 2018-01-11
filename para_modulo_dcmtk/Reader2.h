#include <fstream>
#include <iostream>
#include <string>

#include "CodedImage.h"

#ifndef READER2_H
#define READER2_H

namespace std{

class Reader2{
public:
	Reader2();
	virtual ~Reader2();


	int read(int);
	string readString(int);

	int readBuffer(int);
	int readFirstBits(unsigned char*, int);
	char readChar();

	CodedImage codedImage;
	int imPointer=0;

	int pReader=0;
	unsigned char vReader = 0x00;


};

}

#endif
