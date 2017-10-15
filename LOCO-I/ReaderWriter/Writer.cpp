#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>
#include "Writer.h"

namespace std{
	
Writer::Writer(){}

void Writer::open(string path){
	file.open(path.c_str(), ios::binary);
}

void Writer::write(int num, int bits){
	int numInBits = num % (1 << bits);
	
//	cout << "pWriter=" << pWriter << " bits=" << bits << endl;
	
	if(pWriter + bits < 32){
		for(int cBit=pWriter; cBit < pWriter + bits; cBit++){
			vWriter[cBit] = (numInBits & (1 << ((bits-1) - (cBit-pWriter)))) >> ((bits-1) - (cBit-pWriter));
		}
		pWriter += bits;
	}else{
		for(int cBit=pWriter; cBit < 32; cBit++){
			vWriter[cBit] = (numInBits & (1 << ((bits-1) - (cBit-pWriter)))) >> ((bits-1) - (cBit-pWriter));
		}
		writeToFile();
		
//		cout << "numInBits=" << numInBits;
		numInBits = numInBits & ((1 << (pWriter + bits - 32)) - 1);
//		cout << " --> " << numInBits << endl;
		
		for(int cBit=0; cBit < pWriter + bits - 32; cBit++){
			vWriter[cBit] = (numInBits & (1 << ((bits - (32 - pWriter) - 1) - cBit))) >> ((bits - (32 - pWriter) - 1) - cBit);
		}
		pWriter += bits - 32;
	}	
	
//	for(int cBit=0; cBit<32; cBit++) cout << vWriter[cBit];
//	cout << endl;
}

void Writer::writeChar(char chr){
	write((int)(chr % 256), 8);
}

void Writer::writeString(const char* str, int bytes){
	for(int cChar=0; cChar<bytes; cChar++) writeChar(str[cChar]);
}

char* Writer::num2str(unsigned int num){
	char* str = new char[4];

	str[0] = ((num & 0xFF000000) >> 24);
	str[1] = ((num & 0x00FF0000) >> 16);
	str[2] = ((num & 0x0000FF00) >>  8);
	str[3] =  (num & 0x000000FF);
		
	return str;
}

void Writer::writeToFile(){
	char* str = new char[32];
	unsigned int num=0;
	
	for(int cBit=0; cBit<32; cBit++) num = 2*num + vWriter[cBit];

	str = num2str(num);
	file.write(str, 4);
}

void Writer::close(){	
//	cout << "close() pWriter=" << pWriter << endl;
	if(pWriter > 0){
		for(int cBit=pWriter; cBit<32; cBit++) vWriter[cBit]=0;
		writeToFile();		

//		for(int cBit=0; cBit<32; cBit++) cout << vWriter[cBit];
//		cout << endl;
	}
	
	file.close();
}

Writer::~Writer(){}

}