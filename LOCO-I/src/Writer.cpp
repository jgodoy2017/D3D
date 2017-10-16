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
		for(int cBit = pWriter; cBit < pWriter + bits; cBit++){
			vWriter[cBit] = (numInBits & (1 << ((bits-1) - (cBit-pWriter)))) >> ((bits-1) - (cBit-pWriter));
		}
		pWriter += bits;
	}else{
		for(int cBit = pWriter; cBit < 32; cBit++){
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
	if(pWriter > 0){
//		cout << "close(): pWriter=" << pWriter << endl;
		for(int cBit=pWriter; cBit<32; cBit++) vWriter[cBit]=0;
		for(int cByte = 0; cByte < 1 + pWriter/8; cByte++) flushByte(cByte);
	}
	
	file.close();
	
}

void Writer::flushByte(int cByte){
	char* byteToFile = new char[1];
	unsigned int num=0;
	
//	cout << "flushByte(): bits flusheados: " << 8 * cByte << "-" << 8 * cByte + 7 << endl;
	for(int cBit = 8 * cByte; cBit < 8 * cByte + 8; cBit++) num = 2*num + vWriter[cBit];
	
	byteToFile[0] = (num & 0xFF);
	file.write(byteToFile, 1);
}

Writer::~Writer(){}

}