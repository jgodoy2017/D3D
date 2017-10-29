 /**
  @file Image.cpp
  @brief Se encarga de cargar y preparar la imagen a comprimir

  Lee la imagen completamente, separa el encabezado de los pixeles
  y prepara los datos necesarios para que pueda funcionar Coder.cpp.

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#include "Image.h"
#include "Reader.h"

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

Image::Image(string path, int type){
	//constructor

	this->path=path;
	this->name="";

	if (type==1)loadImage();
	else loadParams();
}

Image::Image(string path){
	//constructor

	this->path=path;
	this->name="";

	loadImage();
}

Image::Image(string path, string name){
	//constructor

	this->path=path;
	this->name=name;

	loadImage();
}

void Image::setImage(){

	image=(int*)malloc(this->width*this->heigth*sizeof(int));// Reserva memoria para guardar toda la imagen
}

void Image::loadParams(){
	string some_name;
	string absolute_path=path+name;

	Reader* reader = new Reader();
	reader->open(absolute_path.c_str());
	
	setMagic(*reader);
	setWidth(*reader);
	setHeigth(*reader);
	setWhite(*reader);
	
	this->nBits  = ((this->white <= 0xFF) ? 8 : 16);

	reader->close();
}

void Image::loadImage(){
	/** Este método carga la imagen entera en un array
	Es un array de una sola dimensión, donde se concatenan todas las filas de la imagen */

	string absolute_path=path+name;
	
	Reader* reader = new Reader();
	reader->open(absolute_path.c_str());
	
	setMagic(*reader);
	setWidth(*reader);
	setHeigth(*reader);
	setWhite(*reader);

	this->nBits  = ((this->white <= 0xFF) ? 8 : 16);

	image=(int*)malloc(this->width*this->heigth*sizeof(int)); //reserva memoria para guardar toda la imagen
	
//	for(int contador=0; contador < this->width * this->heigth; contador++) image[contador] = reader->read(8);
	for(int contador=0; contador < this->width * this->heigth; contador++) image[contador] = reader->read(nBits);
		
	reader->close();
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

void Image::setMagic(Reader& reader){
	string lMagic = reader.readString(2);
	reader.readString(1);

	this->magic = lMagic;
	cout << "magic: " << this->magic << endl;
}

void Image::setWidth(Reader& reader){
	int lWidth = 0;
	int cifra = reader.read(8);
	
	while(cifra != ' '){
		lWidth = 10*lWidth + (cifra - '0');
		cifra = reader.read(8);
	}
	
	this->width = lWidth;
	cout << "width: " << this->width << endl;
}

void Image::setHeigth(Reader& reader){
	int lHeigth = 0;
	int cifra = reader.read(8);
	
	while(cifra != '\n'){
		lHeigth = 10*lHeigth + (cifra - '0');
		cifra = reader.read(8);
	}
	
	this->heigth = lHeigth;
	cout << "heigth: " << this->heigth << endl;
}

void Image::setWhite(Reader& reader){
	int lWhite = 0;
	int cifra = reader.read(8);
	
	while(cifra != '\n'){
		lWhite = 10*lWhite + (cifra - '0');
		cifra = reader.read(8);
	}
	
	this->white = lWhite;
	cout << "white: " << this->white << endl;
}

Image::~Image() {
}

} /* namespace std */
