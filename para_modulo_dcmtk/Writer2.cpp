#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>
#include "Writer2.h"

namespace std{

Writer2::Writer2(BYTE **ptr_){

	ptr=ptr_; //está bien inicializado? me mareo con los asteriscos

}


void Writer2::write(int num, int bits){
	int numInBits = num % (1 << bits);

//	cout << "pWriter2=" << pWriter2 << " bits=" << bits << endl;

	if(pWriter2 + bits < 32){
		for(int cBit = pWriter2; cBit < pWriter2 + bits; cBit++){
			vWriter2[cBit] = (numInBits & (1 << ((bits-1) - (cBit-pWriter2)))) >> ((bits-1) - (cBit-pWriter2));
		}
		pWriter2 += bits;
	}else{
		for(int cBit = pWriter2; cBit < 32; cBit++){
			vWriter2[cBit] = (numInBits & (1 << ((bits-1) - (cBit-pWriter2)))) >> ((bits-1) - (cBit-pWriter2));
		}

		writeToArray();

//		cout << "numInBits=" << numInBits;
		numInBits = numInBits & ((1 << (pWriter2 + bits - 32)) - 1);
//		cout << " --> " << numInBits << endl;

		for(int cBit=0; cBit < pWriter2 + bits - 32; cBit++){
			vWriter2[cBit] = (numInBits & (1 << ((bits - (32 - pWriter2) - 1) - cBit))) >> ((bits - (32 - pWriter2) - 1) - cBit);
		}
		pWriter2 += bits - 32;
	}

//	for(int cBit=0; cBit<32; cBit++) cout << vWriter2[cBit];
//	cout << endl;
}

void Writer2::writeChar(char chr){
	write((int)(chr % 256), 8);
}

void Writer2::writeString(const char* str, int bytes){
	for(int cChar=0; cChar<bytes; cChar++) writeChar(str[cChar]);
}

char* Writer2::num2str(unsigned int num){
	char* str = new char[4];

	str[0] = ((num & 0xFF000000) >> 24);
	str[1] = ((num & 0x00FF0000) >> 16);
	str[2] = ((num & 0x0000FF00) >>  8);
	str[3] =  (num & 0x000000FF);

	return str;
}

void Writer2::writeToArray(){

	char* str;
	unsigned int num=0;

	for(int cBit=0; cBit<32; cBit++) num = 2*num + vWriter2[cBit];

	str = num2str(num);


	ptr[0][ptrPointer]=(BYTE)str[0]; ptrPointer++;
	ptr[0][ptrPointer]=(BYTE)str[1]; ptrPointer++;
	ptr[0][ptrPointer]=(BYTE)str[2]; ptrPointer++;
	ptr[0][ptrPointer]=(BYTE)str[3]; ptrPointer++;


}

void Writer2::close(){
	if(pWriter2 > 0){
//		cout << "close(): pWriter2=" << pWriter2 << endl;
		for(int cBit=pWriter2; cBit<32; cBit++) vWriter2[cBit]=0;
		for(int cByte = 0; cByte < 1 + (pWriter2-1)/8; cByte++) flushByte(cByte);
	}

}

void Writer2::flushByte(int cByte){


	char* byteToFile = new char[1];
	unsigned int num=0;

//	cout << "flushByte(): bits flusheados: " << 8 * cByte << "-" << 8 * cByte + 7 << endl;
	for(int cBit = 8 * cByte; cBit < 8 * cByte + 8; cBit++) num = 2*num + vWriter2[cBit];

	byteToFile[0] = (num & 0xFF);

	ptr[0][ptrPointer]=byteToFile[0]; ptrPointer++;


}

Writer2::~Writer2(){}

}
