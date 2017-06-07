#include <cstdlib>
#include <iostream>
#include "utils.h"

int main(int argc, char **argv){
	int l_uchar=0;
	unsigned char* uchar;
	bool flag=false;
	
	Utils utils;
	
	for(int i=0; i<800; i++){
		flag=!flag;
		utils.bitsToFile[i]=flag;
	}
	
	printf(">> bitsToFilePointer: ");
	scanf("%d", &utils.bitsToFilePointer);
	printf("\n");
	
	printf(">> Convirtiendo: bool[] --> unsigned char[]\n>> Entrada: ");
	for(int i=0; i<utils.bitsToFilePointer; i++) printf("%d", utils.bitsToFile[i] ? 1 : 0);
	printf("\n");
	
	uchar=utils.bool2uchar(&l_uchar);
	printf(">> Salida:  ");
	for(int i=0; i<l_uchar; i++) printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(uchar[i]));
	printf("\n");
	printf(">> Salida:  Largo: %d bytes + %d bits\n\n", l_uchar-1, utils.bitsToFilePointer%8);
	
	for(int i=0; i<800; i++){
		utils.bitsToFile[i]=false;
	}
	
	printf(">> Convirtiendo: unsigned char[] --> bool[]\n");
	printf(">> Entrada: Largo: %d bytes + %d bits\n", l_uchar-1, utils.bitsToFilePointer%8);
	printf(">> Entrada: ");
	for(int i=0; i<l_uchar; i++) printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(uchar[i]));
	utils.uchar2bool(uchar, utils.bitsToFilePointer);
	printf("\n");
	printf(">> Salida:  ");
	for(int i=utils.bitsToFilePointer/2; i<utils.bitsToFilePointer; i++) printf("%d", utils.bitsToFile[i] ? 1 : 0);
	printf("\n");
}
