 /**
  @file CodedImage.cpp
  @brief Se encarga de cargar y preparar la imagen a descomprimir

  Lee la imagen codificada completamente, separa el encabezado de los pixeles
  y prepara los datos necesarios para que pueda funcionar Decoder.cpp.

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#include <sstream>
#include "CodedImage.h"

namespace std {
	
	int CodedImage::codedImagePointer = 0;
	int CodedImage::fileToBitsPointer = 0;
	bool CodedImage::fileToBits[800];
	unsigned int CodedImage::bitInByte = 0;

CodedImage::CodedImage() {
}

CodedImage::CodedImage(int heigth, int width) {
		//constructor
	this->heigth=heigth;
	this->width=width;
}

CodedImage::CodedImage(string path){
	//constructor

	this->path=path;
	this->name="";
	loadImage();
}

CodedImage::CodedImage(string path, string name){
	//constructor

	this->path=path;
	this->name=name;

	loadImage();
}

CodedImage::~CodedImage() {
	// TODO Auto-generated destructor stub
}

void CodedImage::loadImage(){
	/** Carga la imagen codificada con la que trabajará el codificador como un array en memoria.
	El funcionamiento es similar al método con el mismo nombre de la clase Image
	salvo que esta vez los elementos de codedImage se guardan en un array de chars, en vez de un array de enteros
	ya que esta vez cada byte del archivo no tiene una interpretación real, es solo código */

	string absolute_path=path+name;
	int contador=0;
	char temp='1';
	ifstream in;
	in.open(absolute_path.c_str(), ios::binary);

	// El funcionamiento de estos 5 métodos es igual a los presentes en la clase Image

	setMagic(in,temp);
	setWidth(in,temp,false);
	setHeigth(in,temp,false);
	setWhite(in,temp,false);
	setNmax(in, temp);
	setCantidadImagenes(in, temp);
	setCompMov(in,temp);

	image=(char*)malloc(cantidad_imagenes*(this->width*this->heigth)*sizeof(char));
	/** es una cota superior para el tamaño del archivo codificado*/
	vector_alto = (int*)malloc((this->v_width*this->v_heigth)*sizeof(int));
	vector_ancho = (int*)malloc((this->v_width*this->v_heigth)*sizeof(int));

	while (true){
		in.read(&temp,1);
		if (in.eof()) break;
		image[contador]=temp;
		contador++;
	}
	in.close();
}

void CodedImage::setCantidadImagenes(ifstream &in,char &temp){
	int contador=0;
	double resultado=0.0;
	int potencia=10;
	in.read(&temp,1);

	while (temp!='\n'){
		int temp_=temp-'0';
		resultado = double(resultado)+(double)temp_/(double)potencia;
		in.read(&temp,1);
		contador++;
		potencia=potencia*10;
	}
	resultado=(double)resultado*(double)(potencia/10);
	this->cantidad_imagenes=round(resultado);
}

void CodedImage::setNmax(ifstream &in,char &temp){
	int contador=0;
	double resultado=0.0;
	int potencia=10;

	in.read(&temp,1);

	while (temp!='\n'){
		int temp_=temp-'0';
		resultado = double(resultado)+(double)temp_/(double)potencia;
		in.read(&temp,1);
		contador++;
		potencia=potencia*10;
	}

	resultado=(double)resultado*(double)(potencia/10);
	this->Nmax=round(resultado);
}

void CodedImage::setMagic(ifstream &in,char &temp){
	string magic="";

	in.read(&temp,1);
	magic=magic +temp;
	in.read(&temp,1);
	magic=magic+temp;
	this->magic=magic;
	in.read(&temp,1);
}

void CodedImage::setCompMov(ifstream &in,char &temp){
	in.read(&temp,1);
	if (temp=='1') {
		activarCompMov=true;
		in.read(&temp,1);
		setWidth(in,temp,true);
		setHeigth(in,temp,true);
		setWhite(in,temp,true);
	} else {
		in.read(&temp,1);
	}
}

void CodedImage::setWidth(ifstream &in,char &temp, bool vector){
	int contador=0;
	double resultado=0.0;
	int potencia=10;

	in.read(&temp,1);

	while (temp!=' '){
		int temp_=temp-'0';
		resultado = double(resultado)+(double)temp_/(double)potencia;
		in.read(&temp,1);
		contador++;
		potencia=potencia*10;
	}

	resultado=(double)resultado*(double)potencia/10;
	if (vector) {
		this->v_width=round(resultado);
	} else {
		this->width=round(resultado);
	}
}

void CodedImage::setHeigth(ifstream &in,char &temp, bool vector){
	int contador=0;
	double resultado=0.0;
	int potencia=10;

	in.read(&temp,1);

	while (temp!='\n'){
		int temp_=temp-'0';
		resultado = double(resultado)+(double)temp_/(double)potencia;
		in.read(&temp,1);
		contador++;
		potencia=potencia*10;
	}

	resultado=(double)resultado*(double)potencia/10;
	if (vector) {
		this->v_heigth=round(resultado);
	} else {
		this->heigth=round(resultado);
	}
}

void CodedImage::setWhite(ifstream &in,char &temp, bool vector){
	int contador=0;
	double resultado=0.0;
	int potencia=10;

	in.read(&temp,1);

	while (temp!='\n'){
		int temp_=temp-'0';
		resultado = double(resultado)+(double)temp_/(double)potencia;
		in.read(&temp,1);
		contador++;
		potencia=potencia*10;
	}

	resultado=(double)resultado*(double)(potencia/10);
	if (vector) {
		this->v_white=round(resultado);
	} else {
		this->white=round(resultado);
	}
}

void CodedImage::completaArray(){
	/** Cuando se lee el último bit disponible del array, este método vuelve a completarlo. */

	char temp;
	int count=0;
	int tam = this->cantidad_imagenes * (this->width*this->heigth + 2*this->v_width*this->v_heigth);

	while ((count<100) && (codedImagePointer<tam)){ //hasta leer 100 bytes o que se termine la informacion
		temp=this->image[codedImagePointer];
		codedImagePointer++;
		std::bitset<8> temp_b(temp);
		for(int j=0;j<8;j++) fileToBits[count*8+j]=temp_b[7-j];
		count++;
	}
}

int CodedImage::getBit(){
	/** Esta función devuelve el próximo bit de fileToBits.
	Cuando llega al último elemento del array, vuelve a llenar el array con los valores de la imagen
	y empieza desde el lugar 0 */

	bitInByte = (bitInByte + 1) % 8;
	if (fileToBitsPointer==0) completaArray();
	int retorno = fileToBits[fileToBitsPointer];
	fileToBitsPointer=((fileToBitsPointer+1)%800); //actualiza el puntero al array de manera circular
	
	return retorno;
}

void CodedImage::flushDecoder(){
	unsigned int bib = bitInByte;
	
	for(unsigned int i=(8-bib)%8; i>0; i--) (void)getBit(); 
}


} /* namespace std */
