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
	Image(int, int);
	Image(string);
	Image(string, string);
	virtual ~Image();
	void loadImage();
	void setImage();
	void setMagic(ifstream&,char&);
	void setWidth(ifstream&,char&);
	void setHeigth(ifstream&,char&);
	void setWhite(ifstream&,char&);
	int binaryToInt(char);

	string path;

	string name;

	string magic;

	int width;
	int heigth;

	int white;

	/* esta variable representa la imagen, es un array de enteros donde cada elemento es el
	valor del pixel correspondiente. Es como un array de una dimensión, con todas las filas
	de la imagen concatenadas. */
	int *image;

	};

} /* namespace std */

#endif /* IMAGE_H_ */
