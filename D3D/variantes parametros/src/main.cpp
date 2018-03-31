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

bool startsWith(const string& haystack, const string& needle) {
    return needle.length() <= haystack.length()
        && equal(needle.begin(), needle.end(), haystack.begin());
}

string str_(int n){
	stringstream ss1;
	ss1 << n;
	string n_ = ss1.str();

	return n_;
}

int int_(string ss){

	int numb;
	istringstream ( ss ) >> numb;

	return numb;
}




int main(int nargs, char *args[]){

/**		suposiciones: todas las imágenes del stack tienen el mismo ancho, mismo largo		*/

	//string path="/home/felipe/Desktop/stacks-prueba/stacks_prueba/Stack 10/";
//string path="/home/felipe/Desktop/defensa/mini-stack/";
 	string path=args[1];

 	int tam_imagenes=0;
 	int tam_vectores=0;
 	int tam_total=0;

 	bool test=false;

 	string archivo_salida;
 	bool escribe_salida=false;

	int Nmax=64;
	int reset=64;
	int t1=3;
	int t2=7;
	int t3=21;

	bool activarCompMov=false;
	bool activarVarianza=false;
	bool dibujarVectores=false;
	int search=6;
	int bsize=10;
	int umbralVarianza=500;

	bool code=true;
	bool decode=true;

	bool debug=false;

	bool jpls2d=false;

	bool resultadosPorFrame=false;
	string archivo_resultado_por_frame;

	bool rand=false;

	int ancho,alto,cant;

	cout<<nargs<<endl;

	std::string s;
	std::string delimiter;

	for (int k=2;k<nargs;k++){

		s=args[k];

		if (startsWith(args[k],"-Nmax"))	{
			delimiter="-Nmax";
			Nmax=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-reset"))	{
			delimiter="-reset";
			reset=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-t1"))	{
			delimiter="-t1";
			t1=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-t2"))	{
			delimiter="-t2";
			t2=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-t3"))	{
			delimiter="-t3";
			t3=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}

		if (startsWith(args[k],"-comp"))	{
			delimiter="-comp";
			activarCompMov=(bool)int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-var"))	{
			delimiter="-var";
			activarVarianza=(bool)int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-dib"))	{
			delimiter="-dib";
			dibujarVectores=(bool)int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-s"))	{
			delimiter="-s";
			search=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}
		if (startsWith(args[k],"-bs"))	{
			delimiter="-bs";
			bsize=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}

		if (startsWith(args[k],"-varU"))	{
			delimiter="-varU";
			umbralVarianza=int_(s.substr(s.find(delimiter)+delimiter.length(),s.length()));
		}

		if (startsWith(args[k],"-codeOFF"))	{
			code=false;
		}

		if (startsWith(args[k],"-decoOFF"))	{
			decode=false;
		}

		if (startsWith(args[k],"-2D"))	{
			jpls2d=true;
		}

		if (startsWith(args[k],"-bug"))	{
			debug=true;
		}
		if (startsWith(args[k],"-salida:"))	{
			delimiter="-salida:";
			escribe_salida=true;
			archivo_salida=s.substr(s.find(delimiter)+delimiter.length(),s.length());
		}

		if (startsWith(args[k],"-test"))	{
				test=true;
		}

		if (startsWith(args[k],"-xF"))	{
				delimiter="-xF:";
				resultadosPorFrame=true;
				archivo_resultado_por_frame=s.substr(s.find(delimiter)+delimiter.length(),s.length());

		}

		if (startsWith(args[k],"-rd"))	{
			rand=true;

		}

	}

	cout<<Nmax<<" "<<reset<<" "<<t1<<" "<<t2<<" "<<t3<<" "<<endl;
	cout<<activarCompMov<<" "<<activarVarianza<<" "<<dibujarVectores<<" "<<search<<" "<<bsize<<" "<<umbralVarianza<<" "<<endl;
	cout<<code<<" "<<decode<<" "<<endl;

	string path_salida=path+"_coded_Nmax_"+str_(Nmax);

	if (!(jpls2d)){

	if (code){

	Writer* writer = new Writer();
	writer->open(path_salida);

	Coder * coder1 = new Coder(path,false);

	coder1->setParams(Nmax,reset,t1,t2,t3);
	coder1->setCompParams(activarCompMov,activarVarianza,dibujarVectores,search,bsize,umbralVarianza);
	coder1->setTest(test);
	coder1->setResultadosXFrame(resultadosPorFrame,archivo_resultado_por_frame);
	coder1->setRand(rand);
	coder1->code(false,*writer);

	ancho=coder1->width;
	alto=coder1->heigth;
	cant=coder1->cantidad_imagenes;


	tam_total=writer->cantidad_bytes_escritos;

	writer->close();

	}

	if (decode){

	Reader* reader = new Reader();
	reader->open(path_salida);

	CodedImage * codedImage = new CodedImage(*reader, path_salida);	//se escribe el valor de Nmax en la imagen, arreglar !

	Decoder * decoder2 = new Decoder(*codedImage, false);
	decoder2->setParams(Nmax,reset,t1,t2,t3);
	decoder2->setCompParams(activarCompMov);
	Image * prev2 = new Image();

	for (int imagenActual = 0; imagenActual<codedImage->cantidad_imagenes; imagenActual++){
		if (codedImage->activarCompMov) {
			Image * prev1 = new Image();
			Decoder * decoder1 = new Decoder(*codedImage, true);

			decoder1->setParams(Nmax,reset,t1,t2,t3);
			decoder1->setCompParams(false);
			decoder1->decode(*reader, true, *prev1, 0);
			decoder1->decode(*reader, true, *prev1, 1);
			//codedImage->flushDecoder();
		}
		decoder2->decode(*reader, false, *prev2, imagenActual);
	}
	reader->close();
	//codedImage->flushDecoder();
	}
	}

	else{

		if (code){

			Writer* writer = new Writer();
			writer->open(path_salida);

			Coder * coder1 = new Coder(path,true);

			coder1->setParams(64,64,3,7,21);
			coder1->setCompParams(0,0,0,6,10,500);
			coder1->setMode(jpls2d);
			coder1->setDebug(debug);
			coder1->setTest(test);
			coder1->setResultadosXFrame(resultadosPorFrame,archivo_resultado_por_frame);

			coder1->code2D(*writer);

			ancho=coder1->width;
			alto=coder1->heigth;
			cant=coder1->cantidad_imagenes;

			tam_total=writer->cantidad_bytes_escritos;

			writer->close();


		}

		if (decode){

			Reader* reader = new Reader();
			reader->open(path_salida);


			CodedImage * codedImage = new CodedImage(*reader, path_salida);	//se escribe el valor de Nmax en la imagen, arreglar !

			Decoder * decoder2 = new Decoder(*codedImage, false);
			decoder2->setParams(64,64,3,7,21);
			decoder2->setCompParams(0);
			decoder2->setMode(jpls2d);
			decoder2->setDebug(debug);

			Image * prev2 = new Image();

			for (int imagenActual = 0; imagenActual<codedImage->cantidad_imagenes; imagenActual++){

				decoder2->decode2D(*reader, imagenActual);
			}
			reader->close();


		}
	}

	if (escribe_salida){

	string escribe=path+","+str_((int)jpls2d)+","+str_(tam_total*8)+","+str_(ancho*alto*cant)+"\n";

	std::ofstream outfile;

	archivo_salida="./"+archivo_salida;
	outfile.open(archivo_salida.c_str(), std::ios_base::app);

	outfile << escribe;
	outfile.close();
	}

    return 0;
}
