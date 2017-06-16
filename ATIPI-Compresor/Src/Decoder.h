 /**
  @file Decoder.h

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/


#ifndef DECODER_H_
#define DECODER_H_

#include "CodedImage.h"
#include "Image.h"
#include "Context.h"
#include "ContextRun.h"

using namespace std;

class Decoder {
public:


	typedef struct pixels{

			int a;
			int b;
			int c;

		} pixels;

		typedef struct grad_struct{

			int ga;
			int gb;
			int gc;

		} grad;

		Decoder(CodedImage);
		void decode();
		pixels getPixels(int);
		int getP(pixels);
		grad setGradients(int, pixels);
		void setContextsArray();
		int getContext(grad);
		int getPredictedValue(pixels);
		int getK(int);
		void updateContexto(int, int);
		void writeHeader(ofstream&);
		void writeWidth(ofstream&);
		void writeHeigth(ofstream&);
		void writeWhite(ofstream&);
		void writeMagic(ofstream&);
		void updateImage(int, int);
		int unRice(int);
		void completaArray(void);
		void completaArrayRun(void);
		int getBit();
		int getError(int);
		int decodeRun(ofstream&, int);
		void updateContextoRun(int, int);
		unsigned char* bool2uchar();
		virtual ~Decoder();

		/* Este objeto representa la imagen codificada que está decodificando */
		CodedImage codedImage;
		/* Esta variable lleva la cuenta de los bytes leídos de codedImage */
		int codedImagePointer=0;
		int Nmax;
		int i;

		static const int CANTIDAD_MAXIMA_CONTEXTOS=9*9*5;

		Context contexts[CANTIDAD_MAXIMA_CONTEXTOS];

		/* Array con los 2 contextos especiales que aparecen en el modo de rachas: (a==b) y (a!=b) */
		ContextRun contextsRun[2];

	string file;

	Image image;

	/* Algún tamaño apropiado
	Array auxiliar para poder trabajar con los bits individuales de la imagen codificada */
	bool fileToBits[800];

	/* Puntero que señala el próximo lugar a leer de decode */
	int fileToBitsPointer=0;
	
	// La misma idea, pero con un array+puntero exclusivo para rachas, que no avanza automaticamente el codedImagePointer.
	bool fileToBitsRun[80];
	int fileToBitsRunPointer=0;
};

#endif /* DECODER_H_ */
