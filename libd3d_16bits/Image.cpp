 /**
  @file Image.cpp
  @brief Se encarga de cargar y preparar la imagen a comprimir

  Lee la imagen completamente, separa el encabezado de los pixeles
  y prepara los datos necesarios para que pueda funcionar Coder.cpp.

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#include "Image.h"

namespace std {

Image::Image(){
}

Image::Image(int heigth, int width, int white){
	//constructor

	this->heigth = heigth;
	this->width  = width;
	this->white  = white;
	this->nBits  = ((white <= 0xFF) ? 8 : 16);
	
	image=(int*)malloc(this->width*this->heigth*sizeof(int));// Reserva memoria para guardar toda la imagen
}

Image::Image(int heigth, int width, int white,int reserva){
	//constructor

	this->heigth = heigth;
	this->width  = width;
	this->white  = white;
	this->nBits  = ((white <= 0xFF) ? 8 : 16);

	if (reserva) image=(int*)malloc(this->width*this->heigth*sizeof(int));// Reserva memoria para guardar toda la imagen
}
void Image::setImage(){

	image=(int*)malloc(this->width*this->heigth*sizeof(int));// Reserva memoria para guardar toda la imagen
}

int Image::getPixel(int x, int y){
	if(x + y*width >= width*heigth){
//		cout<<"Atención: getPixel - Fuera de la imagen"<<endl;
		cout << "Image::getPixel(): FUERA DE RANGO> (x,y) = (" << x << "," << y << ") (w,h) = (" << width << "," << heigth << ") px = " << image[x + y*width] << endl;
	}

	return image[x + y*width];
}

void Image::setPixel(int pixel, int x, int y){
	if(x + y*width >= width*heigth) cout<<"Atención: setPixel - Fuera de la imagen"<<endl;
	image[x + y*width] = pixel;
}

void Image::vectorToCoords (int current, int &x, int &y){
	x = current % width;
	y = current / width;
}

int Image::coordsToVector (int x, int y){
	if(x + y*width >= width*heigth) cout<<"Atención: coordsToVector - Fuera de la imagen"<<endl;

	return x + y*width;
}

Image::~Image() {
}

} /* namespace std */
