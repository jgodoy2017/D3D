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

namespace std {

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
			virtual ~Decoder();
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
		void completaArray();
		int getBit();
		int getError(int);

		int fixPrediction(int, int);

		/* Este objeto representa la imagen codificada que está decodificando */
		CodedImage codedImage;
		/* Esta variable lleva la cuenta de los bytes leídos de codedImage */
		int codedImagePointer=0;
		int Nmax;
		int i;

		static const int CANTIDAD_MAXIMA_CONTEXTOS=9*9*5;

		Context contexts[CANTIDAD_MAXIMA_CONTEXTOS];


	string file;

	Image image;

	/* Algún tamaño apropiado
	Array auxiliar para poder trabajar con los bits individuales de la imagen codificada */
	bool fileToBits[800];

	/* Puntero que señala el próximo lugar a leer de decode */
	int fileToBitsPointer=0;
};

} /* namespace std */

#endif /* DECODER_H_ */
