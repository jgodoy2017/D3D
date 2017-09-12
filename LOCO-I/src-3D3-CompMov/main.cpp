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
//	path = path + '\\';

/*	ofstream salida;
	salida.open(path_salida.c_str(), ios::binary);

	Coder coder1(path,Nmax,1);
	coder1.code(false,salida);
	salida.close();
*/
	CodedImage codedImage(path+"_coded_Nmax_"+str_(Nmax));
	int codedImagePointer = 0;

	for (int imagenActual = 0;imagenActual<codedImage.cantidad_imagenes;imagenActual++){
		if (codedImage.activarCompMov) {
			Decoder decoder1(codedImage,true);
			decoder1.decode(true,codedImagePointer);
		}
			Decoder decoder2(codedImage);
			decoder2.decode(false,codedImagePointer);
	}


    return 0;
}
