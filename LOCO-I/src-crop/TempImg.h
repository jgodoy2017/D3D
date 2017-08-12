 /**
  @file TempImg.h

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#ifndef TEMPIMG_H_
#define TEMPIMG_H_

#include "Image.h"

namespace std {

class TempImg {
public:
	typedef struct pixelsTemp{

			int a;
			int b;
			int c;
			int d;
			int a_;
			int b_;
			int c_;
			int d_;
			int e_;
			int f_;
			int g_;

	} pixelsTemp;
	typedef struct grad_structTemp{

			int ga;
			int gb;
			int gc;

		} gradTemp;	//se define esta estructura para modelar el vector de gradientes


	TempImg();
	TempImg(Image, Image);
	void makeImg();
	pixelsTemp getPixels(int, int);
	gradTemp setGradientsTemp(int, pixelsTemp);
	int selectMED(gradTemp);
	int getPredictedValue(int, pixelsTemp, int);
	void initTemp();
	void encode(int, int, ofstream&);
	void writeCode(ofstream&);	/* writecode() y flushEncoder() son métodos reciclados de la (propia) tarea del curso de Compresión de Datos Sin Pérdida*/
	void flushEncoder(ofstream&);
	void writeHeader(ofstream&);
	void writeWidth(ofstream&);
	void writeHeigth(ofstream&);
	void writeWhite(ofstream&);
	void writeMagic(ofstream&);
	void writeNmax(ofstream&);
	virtual ~TempImg();
	gradTemp gradientes;
	int derecha;
	int abajo;
	int restoH;
	int restoV;
	int count_med;
	int count_med_h;
	int count_med_v;
	/* Este objeto representa la imagen a ser codificada */
	Image image;
	Image image2;
	int i;
	int n; //veces ancho bloque
	int m; //veces alto bloque

	int *tempimage;

	/* En este array auxiliar se van guardando los bits de código generado a escribir en el archivo
	se usa 800 como un tamaño suficiente, este número tiene que ser mayor al error más grande que pueda ser codificado + 7
	ya que ese sería el tamaño más grande posible del array antes de ser escrito en el archivo
	800 es mucho más grande que este número, por lo que resulta suficiente */
	bool bitsToFile[800];

	/* Puntero que señala el próximo lugar a escribir de code */
	int bitsToFilePointer=0;
};

} /* namespace std */

#endif /* TEMPIMG_H_ */
