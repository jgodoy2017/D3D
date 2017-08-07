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

int main(int nargs, char *args[]){


 string path="/home/felipe/Documents/ATIPI/img_prueba/womanc.pgm";
// string path=args[1];
	int Nmax=64;

	Image image(path);

	Coder coder1(image,Nmax,1);
	coder1.code();

	stringstream ss1;
	ss1 << Nmax;
	string nmax = ss1.str();

	CodedImage codedImage(path+"_coded_Nmax_"+nmax+"_"+"1");

	Decoder decoder(codedImage);
	decoder.decode();

	/*

	Coder coder2(image,8,0);
	coder2.code();

	Coder coder3(image,64,0);
	coder3.code();

	Coder coder4(image,256,0);
	coder4.code();

	Coder coder5(image,4,1);
	coder5.code();

	Coder coder6(image,8,1);
	coder6.code();

	Coder coder7(image,64,1);
	coder7.code();

	Coder coder8(image,256,1);
	coder8.code();

	//stringstream ss1;
	//ss1 << Nmax;
	//string nmax = ss1.str();

	//CodedImage codedImage(path+"_coded_Nmax_"+nmax+"_");

	//Decoder decoder(codedImage);
	//decoder.decode();



	//
	if (nargs<3)	{
		cout<<"error...! parámetros insuficientes !"<<endl;
	}
	else{

		cout<<"ejecutando..."<<endl;

		string mode=args[1];
		string path=args[2];

		if (mode.compare("-c")==0){

			string aux= args[3]; int Nmax=atoi(aux.c_str());

			cout<<path<<" "<<Nmax<<endl;


			Image image(path);

			Coder coder(image,Nmax);
			coder.code();

		}else if (mode.compare("-d")==0){


			CodedImage codedImage(path);

			Decoder decoder(codedImage);
			decoder.decode();

		}else if (mode.compare("-cd")==0){

			string aux= args[3]; int Nmax=atoi(aux.c_str());

			Image image(path);

			Coder coder(image,Nmax);
			coder.code();

			CodedImage codedImage(path+"_coded_Nmax_"+aux+"_region_3");

			Decoder decoder(codedImage);
			decoder.decode();

		}else cout<<"error...! comando no encontrado !"<<endl;

	}


***/

    return 0;


}
