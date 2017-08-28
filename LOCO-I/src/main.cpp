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


bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
string* loadImagePaths(string path){


		int contador=0;
		string image_paths[50];


		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir (path.c_str())) != NULL) {
		  /* print all the files and directories within directory */
		  while ((ent = readdir (dir)) != NULL) {


		    if (hasEnding(ent->d_name,".pgm")) {
		    	//cout <<contador<< endl;
		    	contador++;
		    	image_paths[contador]=(string)ent->d_name;
		    }

		  }
		  closedir (dir);
		}

		cout <<contador<< endl;
return image_paths;
}

string str_(int n){


	stringstream ss1;
	ss1 << n;
	string n_ = ss1.str();

	return n_;
}

int main(int nargs, char *args[]){

/**		suposiciones: todas las imágenes del stack tienen el mismo ancho, mismo largo		*/

 string path="/home/felipe/Documents/ATIPI/prueba/";
// string path=args[1];
	int Nmax=64;



	Coder coder1(path,Nmax,1);
	coder1.code();

	CodedImage codedImage(path+"_coded_Nmax_"+str_(Nmax)+"_"+"1");

	Decoder decoder(codedImage);
	decoder.decode();

	/*
	 *

	stringstream ss1;
	ss1 << Nmax;
	string nmax = ss1.str();

	CodedImage codedImage(path+"_coded_Nmax_"+nmax+"_"+"1");



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
