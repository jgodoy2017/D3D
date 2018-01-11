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
#include "Reader.h"

namespace std {
	
CodedImage::CodedImage() {}

CodedImage::CodedImage(int heigth, int width) {
	this->heigth=heigth;
	this->width=width;
}

CodedImage::CodedImage(int heigth, int width,int white,int reserva) {
	this->heigth=heigth;
	this->width=width;
	this->white=white;

	if (reserva) image=(unsigned char*)malloc(this->width*this->heigth*sizeof(unsigned char));// Reserva memoria para guardar toda la imagen
}

CodedImage::CodedImage(Reader &reader){
	this->path="";
	this->name="";
	loadImage(reader);
}

CodedImage::CodedImage(Reader &reader, string path){
	//constructor

	this->path=path;
	this->name="";

	loadImage(reader);
}

CodedImage::~CodedImage() {
	// TODO Auto-generated destructor stub
}

void CodedImage::loadImage(Reader &reader){
	/** Carga la imagen codificada con la que trabajará el codificador como un array en memoria.
	El funcionamiento es similar al método con el mismo nombre de la clase Image
	salvo que esta vez los elementos de codedImage se guardan en un array de chars, en vez de un array de enteros
	ya que esta vez cada byte del archivo no tiene una interpretación real, es solo código */

	cout << "// START CODEDIMAGE" << endl;

	int contador=0;
	
	setMagic(reader);
	setWidth(reader,false);
	setHeigth(reader,false);
	setWhite(reader,false);
	setNmax(reader);
	setCantidadImagenes(reader);
	setCompMov(reader);

	vector_alto =  (int*)malloc((this->v_width*this->v_heigth)*sizeof(int));
	vector_ancho = (int*)malloc((this->v_width*this->v_heigth)*sizeof(int));

	cout << "// END CODEDIMAGE" << endl;
}

int CodedImage::getPixelAlto(int x, int y){
	if(x + y*v_width >= v_width*v_heigth) cout<<"Atención: codedImage.getPixelAlto - Fuera de la imagen"<<endl;
	return vector_alto[x + y*v_width];
}

void CodedImage::setPixelAlto(int pixel, int x, int y){
	if(x + y*v_width >= v_width*v_heigth) cout<<"Atención: codedImage.setPixelAlto - Fuera de la imagen"<<endl;
	vector_alto[x + y*v_width] = pixel;
}

int CodedImage::getPixelAncho(int x, int y){
	if(x + y*v_width >= v_width*v_heigth) cout<<"Atención: codedImage.getPixelAncho - Fuera de la imagen"<<endl;
	return vector_ancho[x + y*v_width];
}

void CodedImage::setPixelAncho(int pixel, int x, int y){
	if(x + y*v_width >= v_width*v_heigth) cout<<"Atención: codedImage.setPixelAncho - Fuera de la imagen"<<endl;
	vector_ancho[x + y*v_width] = pixel;
}

void CodedImage::setCantidadImagenes(Reader &reader){
	int lCantidad_imagenes = 0;
	int cifra = reader.read(8);
	
	while(cifra != '\n'){
		lCantidad_imagenes = 10*lCantidad_imagenes + (cifra - '0');
		cifra = reader.read(8);
	}
	
	this->cantidad_imagenes = lCantidad_imagenes;
	cout << "cantidad_imagenes: " << this->cantidad_imagenes << endl;
}

void CodedImage::setNmax(Reader &reader){
	int lNmax = 0;
	int cifra = reader.read(8);
	
	while(cifra != '\n'){
		lNmax = 10*lNmax + (cifra - '0');
		cifra = reader.read(8);
	}
	
	this->Nmax = lNmax;
	cout << "Nmax: " << this->Nmax << endl;
}

void CodedImage::setMagic(Reader &reader){
	string lMagic = reader.readString(2);
	reader.readString(1);

	this->magic = lMagic;
	cout << "magic: " << this->magic << endl;
}

void CodedImage::setCompMov(Reader &reader){
	int cifra = reader.read(8);

	if (cifra == '1') {
		activarCompMov=true;
		reader.read(8);
		
		setWidth(reader,true);
		setHeigth(reader,true);
		setWhite(reader,true);
		setBSize(reader);
	} else {
		activarCompMov=false;
		reader.read(8);
	}
}

void CodedImage::setWidth(Reader &reader, bool vector){
	int lWidth = 0;
	int cifra = reader.read(8);
	
	while(cifra != ' '){
		lWidth = 10*lWidth + (cifra - '0');
		cifra = reader.read(8);
	}
	
	if(vector) this->v_width = lWidth;
	else       this->width   = lWidth;
}

void CodedImage::setHeigth(Reader &reader, bool vector){
	int lHeigth = 0;
	int cifra = reader.read(8);
	
	while(cifra != '\n'){
		lHeigth = 10*lHeigth + (cifra - '0');
		cifra = reader.read(8);
	}
	
	if(vector) this->v_heigth = lHeigth;
	else       this->heigth   = lHeigth;
}

void CodedImage::setBSize(Reader &reader){
	int lBsize = 0;
	int cifra = reader.read(8);
	
	while(cifra != '\n'){
		lBsize = 10*lBsize + (cifra - '0');
		cifra = reader.read(8);
	}
	
	this->bsize = lBsize;
	cout << "bsize: " << this->bsize << endl;
}

void CodedImage::setWhite(Reader &reader, bool vector){
	int lWhite = 0;
	int cifra = reader.read(8);
	
	while(cifra != '\n'){
		lWhite = 10*lWhite + (cifra - '0');
		cifra = reader.read(8);
	}
	
	if(vector) this->v_white = lWhite;
	else       this->white   = lWhite;
}

} /* namespace std */
