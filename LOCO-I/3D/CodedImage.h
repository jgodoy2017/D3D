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

namespace std {

class CodedImage {
public:
	CodedImage();
	CodedImage(int, int);
	CodedImage(string);
	CodedImage(string, string);
	virtual ~CodedImage();

	void loadImage();
		void setMagic(ifstream&,char&);
		void setWidth(ifstream&,char&);
		void setHeigth(ifstream&,char&);
		void setWhite(ifstream&,char&);
		void setNmax(ifstream&,char&);
		void setCantidadImagenes(ifstream&,char&);

		string path;

		string name;

		string magic;

		int width;
		int heigth;

		int cantidad_imagenes;

		int white;

		/* esta variable representa la imagen codificada, es un array de chars donde cada elemento es
		cada byte leido del archivo */
		char *image;

		int Nmax;

};

} /* namespace std */

#endif /* CODEDIMAGE_H_ */
