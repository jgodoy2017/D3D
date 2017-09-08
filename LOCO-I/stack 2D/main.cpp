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

int main(int nargs, char *args[]){

	//bug en contextos de rachas

	//chequear mapeo de rice variable map


	string path="/home/felipe/Desktop/buenas/1.3.6.1.4.1.9328.50.4.0001/";
// string path=args[1];


 int Nmax=64;

float suma=0;
int contador=0;

	DIR *dir;
		struct dirent *ent;
		if ((dir = opendir (path.c_str())) != NULL) {
		  /* print all the files and directories within directory */
		  while ((ent = readdir (dir)) != NULL) {


		    if (hasEnding(ent->d_name,".pgm")) {
		    	cout <<ent->d_name<< endl;

		    	Image image(path+ent->d_name);

		    	Coder coder1(image,Nmax,1);
		    	suma=suma+coder1.code();

		    	contador++;
		    }

		  }
		  closedir (dir);
		}

	cout <<suma/float(contador)<< endl;

		/*
		 *

	stringstream ss1;
	ss1 << Nmax;
	string nmax = ss1.str();
	CodedImage codedImage(path+"_coded_Nmax_"+nmax+"_"+"1");



	Decoder decoder(codedImage);
	decoder.decode();



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
