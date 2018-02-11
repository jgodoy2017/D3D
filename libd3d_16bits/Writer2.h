#include <fstream>
#include <iostream>
#include <string>

#include "pubtypes.h"

#ifndef WRITER2_H
#define WRITER2_H

namespace std{

class Writer2{
public:
	Writer2();
	Writer2(BYTE **ptr_);	//qué onda los asteriscos?
	Writer2(void* buf_, int);	//qué onda los asteriscos?
	virtual ~Writer2();

	void write(int, int);
	void writeChar(char);
	void writeString(const char*, int);
	void close();

public:
	char* num2str(unsigned int);
	void writeToArray();

	void flushByte(int);

	BYTE **ptr;
	int ptrPointer=0;

	BYTE* buf;
	int bufPointer=0;

	int vWriter2[32];
	int pWriter2=0;

	bool deco=false;


	bool debug1=false;
	bool debug2=false;
};

}

#endif
