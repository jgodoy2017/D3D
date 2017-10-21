/**
* @file main.cpp
* @brief Compresor y descompresor de imágenes PGM
*
*  Utiliza 9 regiones para los gradientes ga y gb, pero 5 regiones para el gradiente gc.
*  Se eligieron las siguientes regiones para gc: [<-3, <0, 0, >0, >3]
*  Según se describe en el informe el Nmax recomendado es = 64
*
*  @author Felipe Tambasco, Mauro Barbosa
*  @date Feb, 2017
*
*/
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <bitset>
#include <unistd.h>

#include <sstream>


#include <iomanip>
#include <dirent.h>

#include "Coder.h"
#include "Decoder.h"
#include "Image.h"

#include "Writer.h"
#include "Reader.h"

using namespace std;

string str_(int n){
	stringstream ss1;
	ss1 << n;
	string n_ = ss1.str();

	return n_;
}

int main(int nargs, char *args[]){

/**		suposiciones: todas las imágenes del stack tienen el mismo ancho, mismo largo		*/

//  string path="/home/felipe/Desktop/buenas/TCGA-VR-A8EO/";
 	string path=args[1];
	
	int Nmax=64;

	string path_salida=path+"_coded_Nmax_"+str_(Nmax);

	Writer* writer = new Writer();
	writer->open(path_salida);

	Coder * coder1 = new Coder(path,Nmax,1);
	coder1->code(false,*writer);

	writer->close();

	Reader* reader = new Reader();
	reader->open(path_salida);

	CodedImage * codedImage = new CodedImage(*reader,path_salida,"");

	Decoder * decoder2 = new Decoder(*codedImage, false);
	Image * prev2 = new Image();

	for (int imagenActual = 0; imagenActual<codedImage->cantidad_imagenes; imagenActual++){
		if (codedImage->activarCompMov) {
			Image * prev1 = new Image();
			Decoder * decoder1 = new Decoder(*codedImage, true);
			decoder1->decode(*reader, true, *prev1, 0);
			decoder1->decode(*reader, true, *prev1, 1);
			//codedImage->flushDecoder();
		}
		decoder2->decode(*reader, false, *prev2, imagenActual);
	}
	reader->close();
	//codedImage->flushDecoder();
    return 0;
}
