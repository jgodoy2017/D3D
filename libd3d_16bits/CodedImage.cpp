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
	
CodedImage::CodedImage() {}

CodedImage::CodedImage(int heigth, int width) {
	this->heigth=heigth;
	this->width=width;
}

CodedImage::CodedImage(int heigth, int width, int white, int reserva) {
	this->heigth=heigth;
	this->width=width;
	this->white=white;
	
	int bytespersample = ((white <= 0xFF) ? 1 : 2);

//	if (reserva) image=(unsigned char*)malloc(this->width*this->heigth*sizeof(unsigned char));// Reserva memoria para guardar toda la imagen
	if (reserva) image=(unsigned char*)malloc(this->width * this->heigth * bytespersample * sizeof(unsigned char));
}

CodedImage::~CodedImage() {
	// TODO Auto-generated destructor stub
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

} /* namespace std */
