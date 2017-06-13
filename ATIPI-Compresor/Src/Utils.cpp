#include <cstdlib>
#include <iostream>
#include "Utils.h"

Utils::Utils(){}
Utils::~Utils(){}


unsigned char* Utils::bool2uchar(int* u_length){
	unsigned char* uchar;
	unsigned char mask;
	int largo;
	
	if(this->bitsToFilePointer){
		largo=1+this->bitsToFilePointer/8;
		*u_length=largo;
	
		uchar=(unsigned char*)malloc(largo*sizeof(unsigned char));
		for(int i=0; i<largo; i++) uchar[i]=0x00;
		
		for(int i=0; i<this->bitsToFilePointer; i++){
			if(this->bitsToFile[i]) POKE_BIT(uchar, i, 0xFF, 1);
			else                    POKE_BIT(uchar, i, 0x00, 1);
		}
	}else{
		largo=0;
		uchar=NULL;
	}
	
	*u_length=largo;
	return uchar;
}

void Utils::uchar2bool(unsigned char* uchar, int bits_uchar){	
	if(bits_uchar && (this->bitsToFilePointer+bits_uchar<800)){
		for(int i=0; i<bits_uchar; i++){
			if(((uchar[i/8]&(1<<(7-i%8)))>>(7-i%8))==0x01) this->bitsToFile[this->bitsToFilePointer+i]=true;
			else                                           this->bitsToFile[this->bitsToFilePointer+i]=false;
		}
		
		this->bitsToFilePointer+=bits_uchar;
	}		
}

