 /**
  @file Image.h

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#ifndef IMAGE_H_
#define IMAGE_H_

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

class Image {
public:
	Image();
	Image(int, int, int);
	Image(int, int, int,int);
	virtual ~Image();
	void setImage();
	
	int getPixel(int,int);
	void setPixel(int,int,int);
	void vectorToCoords(int,int&,int&);
	int coordsToVector(int,int);

	string path;
	string name;
	string magic;

	int width;
	int heigth;
	int white;
	int nBits;

	/* esta variable representa la imagen, es un array de enteros donde cada elemento es el
	valor del pixel correspondiente. Es como un array de una dimensión, con todas las filas
	de la imagen concatenadas. */
	int *image;

	};

} /* namespace std */

#endif /* IMAGE_H_ */
