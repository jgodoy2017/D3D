#include <fstream>
#include <iostream>
#include <string>
#include "Reader.h"

namespace std{
	
Reader::Reader(){}

void Reader::open(string path){
	file.open(path.c_str(), ios::binary);
}

int Reader::read(int bits){
	int num, numBytes;
	
	if(pReader == 0){
		char* buffer1 = new char[1];
		file.read(buffer1, 1);
		vReader = buffer1[0];
	}
	
//	cout << "pReader=" << pReader << " bits=" << bits << endl;
	if(pReader + bits > 8){
		numBytes = 1 + (bits - (8-pReader) - 1)/8;
		
//		for(int cBit=pReader; cBit<8; cBit++) cout << (((unsigned char)vReader & (1 << (7-cBit))) >> (7-cBit));

		char* buffer = new char[numBytes];
		file.read(buffer, numBytes);
	
//		cout << " ";
//		for(int cBit = 0; cBit < bits - (8-pReader); cBit++) cout << (((unsigned char)buffer[cBit/8] & (1 << (7-cBit%8))) >> (7-cBit%8));
//		cout << endl;
	
		int numHi = readBuffer(8-pReader);
		int numLo = readFirstBits(buffer, bits - (8-pReader));
		int shift = (bits - (8 - pReader));
		
		vReader = buffer[numBytes-1];
		pReader = (bits - (8-pReader)) % 8;
		num = (numHi << shift) + numLo;
		
//		cout << "numHi=" << numHi << " numLo=" << numLo << " num=" << num << " shift=" << shift << endl;			
	}else{
//		for(int cBit = pReader; cBit < pReader + bits; cBit++) cout << (((unsigned char)vReader & (1 << (7-cBit))) >> (7-cBit));
//		cout << endl;
		
		pReader = (pReader + bits) % 8;
		num = readBuffer(bits);
	}
	
	cout << "N=" << num << endl;
	return num;
}

char Reader::readChar(){
	return ((char)(read(8) % 256));
}

string Reader::readString(int bytes){
	char* charStr = new char[bytes];
	
	for(int cChar=0; cChar<bytes; cChar++) charStr[cChar]=readChar();
	
	return string(charStr);
}

int Reader::readBuffer(int bits){
	int num=0;
	int cBitVal;
	
	for(int cBit = pReader; cBit < pReader + bits; cBit++){
		cBitVal = ((vReader & (1 << (7-cBit))) >> (7-cBit));
		num = 2*num + cBitVal;
	}
	
	return num;
}

int Reader::readFirstBits(char* buffer, int bits){
	int num=0;
	int cBitVal;
	
	for(int cByte = 0; cByte < bits/8; cByte++){
		for(int cBit=0; cBit<8; cBit++){
			cBitVal = ((buffer[cByte] & (1 << (7-cBit))) >> (7-cBit));
			num = 2*num + cBitVal;
		}
	}
	
	for(int cBit = 0; cBit < bits%8; cBit++){
		cBitVal = ((buffer[bits/8] & (1 << (7-cBit))) >> (7-cBit));
		num = 2*num + cBitVal;
	}
	
	return num;
}

void Reader::close(){
	file.close();
}

Reader::~Reader(){}

}