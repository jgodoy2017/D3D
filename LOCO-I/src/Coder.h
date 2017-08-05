 /**
  @file Coder.h

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#ifndef CODER_H_
#define CODER_H_

#include "Image.h"
#include "Context.h"
#include "Racha.h"
#include "ContextRun.h"

namespace std {

class Coder {
public:

	typedef struct pixels{

		int a;
		int b;
		int c;
		int d;

	} pixels;	//se define esta estructura para agrupar los píxeles a, b y c

	typedef struct grad_struct{

		int ga;
		int gb;
		int gc;

	} grad;	//se define esta estructura para modelar el vector de gradientes

	Coder();
	Coder(Image,int);

	Coder(Image,int,int);


	void code();
	pixels getPixels(int);
	int getP(pixels);
	grad setGradients(pixels);
	void setContextsArray();
	int getContext(grad);
	int getContext_(int, int);
	int getPredictedValue(pixels);
	int getK(int);
	int getKPrime(Racha&);
	int rice(int);
	void encode(int, int, ofstream&);
	void encode_(int, int, ofstream&);
	void updateContexto(int, int);
	void updateContexto_(int, int);
	void writeCode(ofstream&);	/* writecode() y flushEncoder() son métodos reciclados de la (propia) tarea del curso de Compresión de Datos Sin Pérdida*/
	void flushEncoder(ofstream&);
	void writeHeader(ofstream&);
	void writeWidth(ofstream&);
	void writeHeigth(ofstream&);
	void writeWhite(ofstream&);
	void writeMagic(ofstream&);
	void writeNmax(ofstream&);
	int getRachaParams(Image&, int, int, int&);
	void encodeRacha(Racha&);
	void encodeMuestraInterrupcion(Racha&, int, ofstream&);
	int reduccionDeRango(int);
	int fixPrediction(int, int);

	int max(int, int);

	int correctPredictedValue(int, int);

	virtual ~Coder();

	/* Este objeto representa la imagen a ser codificada */
	Image image;
	int Nmax;		//el valor de Nmax
	int i;

	int contador=0; /*** eliminar ***/



	static const int CANTIDAD_MAXIMA_CONTEXTOS=9*9*9;

	/* Array de contextos, cada entrada representa un contexto posible */
	Context contexts[CANTIDAD_MAXIMA_CONTEXTOS];

	/* En este array auxiliar se van guardando los bits de código generado a escribir en el archivo
	se usa 800 como un tamaño suficiente, este número tiene que ser mayor al error más grande que pueda ser codificado + 7
	ya que ese sería el tamaño más grande posible del array antes de ser escrito en el archivo
	800 es mucho más grande que este número, por lo que resulta suficiente */
	bool bitsToFile[800];

	/* Puntero que señala el próximo lugar a escribir de code */
	int bitsToFilePointer=0;

	bool racha=false;

	int J[32]={0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,5,5,6,6,7,7,8,9,10,11,12,13,14,15};

	int kr=0;
	int m_r=1;

	bool debug=false;

	bool aux;

	/**
	 *
	 * CAMBIOS !
	 *
	 *
	 */

	int Lmax;	//agregados también al constructor de clase Coder(image, int, int)
	int beta;
	int qMax;
	bool golombLimitado(int);

	ContextRun cntx[2];    // Contextos especiales para rachas.
};

} /* namespace std */

#endif /* CODER_H_ */
