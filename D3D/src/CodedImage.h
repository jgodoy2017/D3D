 /**
  @file CodedImage.h

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#ifndef CODEDIMAGE_H_
#define CODEDIMAGE_H_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <bitset>
#include <unistd.h>


#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <math.h>
#include "Reader.h"

namespace std {

class CodedImage {
public:
	CodedImage();
	CodedImage(int, int);
	CodedImage(Reader&);
	CodedImage(Reader&, string);
	virtual ~CodedImage();

		void loadImage(Reader&);
		void setMagic(Reader&);
		void setCompMov(Reader&);
		void setWidth(Reader&,bool);
		void setHeigth(Reader&,bool);
		void setWhite(Reader&,bool);
		void setBSize(Reader&);
		void setNmax(Reader&);
		void setCantidadImagenes(Reader&);
		int getPixelAlto(int,int);
		void setPixelAlto(int,int,int);
		int getPixelAncho(int,int);
		void setPixelAncho(int,int,int);

		string path;
		string name;
		string magic;

		int width;
		int heigth;
		int white;

		int v_width;
		int v_heigth;
		int v_white;

		int Nmax;
		int bsize;
		int cantidad_imagenes;

		/* esta variable representa la imagen codificada, es un array de chars donde cada elemento es
		cada byte leido del archivo */
		char *image;

//		bool activarCompMov=false;
		bool activarCompMov;
		int *vector_alto;
		int *vector_ancho;
};

} /* namespace std */

#endif /* CODEDIMAGE_H_ */
