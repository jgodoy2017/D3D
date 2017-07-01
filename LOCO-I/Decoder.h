 /**
  @file Decoder.h

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/


#ifndef DECODER_H_
#define DECODER_H_
#define LARGO_ARRAY_BITS 800
#define MAXVAL8BIT 255
#define MAXVAL16BIT 65535

#include "CodedImage.h"
#include "Image.h"
#include "Context.h"
#include "Racha.h"
#include "ContextRun.h"

namespace std {

class Decoder {
public:


	typedef struct pixels{

			int a;
			int b;
			int c;
			int d;

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
		grad setGradients(pixels);
		void setContextsArray();
		int getContext(grad);
		int getContext_(int, int);
		int getPredictedValue(pixels);
		int getK(int);
		void updateContexto(int, int);
		void updateContexto_(int, int);
		void writeHeader(ofstream&);
		void writeWidth(ofstream&);
		void writeHeigth(ofstream&);
		void writeWhite(ofstream&);
		void writeMagic(ofstream&);
		void updateImage(int, int);
		int unRice(int);
		int unRice_(Racha&, int, int);
		void completaArray();
		int getBit();
		int getError(int);
		int getError_(int);
		int getRachaParams(int, int&);
		void updateImageRacha(Racha&, int, ofstream&);
		void updateImageInterruption(Racha&, int, ofstream&);
		int getKPrime(Racha&);
		int fixPrediction(int, int);
		int rangeReduction(bool, int);

		/* Este objeto representa la imagen codificada que está decodificando */
		CodedImage codedImage;
		/* Esta variable lleva la cuenta de los bytes leídos de codedImage */
		int codedImagePointer=0;
		int Nmax;
		int i;

		static const int CANTIDAD_MAXIMA_CONTEXTOS=9*9*9;

		Context contexts[CANTIDAD_MAXIMA_CONTEXTOS];


	string file;

	Image image;

	/* Algún tamaño apropiado
	Array auxiliar para poder trabajar con los bits individuales de la imagen codificada */
	bool fileToBits[LARGO_ARRAY_BITS];

	/* Puntero que señala el próximo lugar a leer de decode */
	int fileToBitsPointer=0;

	ContextRun cntx[2];    // Contextos especiales para rachas.

	bool racha;

	int J[32]={0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,5,5,6,6,7,7,8,9,10,11,12,13,14,15};

	int kr=0;
	int m_r=1;

	bool debug=false;

};

} /* namespace std */

#endif /* DECODER_H_ */
