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

	typedef struct pixels3D{

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

	} pixels3D;

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
	Coder(Image,Image,int);
	Coder(string,int,int);

	Image setInitialImage();
	void code(bool, ofstream&);
	void CompMov(Image&, Image&); // Compensación de movimiento
	void itera(string, int&, int, int); // Compensación de movimiento
	int selectMED(grad);
	int getPredictedValue(int, pixels3D);
	grad getGradients3D(int, pixels3D);
	pixels getPixels(int);
	pixels getPixels_(int);
	pixels3D getPixels3D(int,int);
	int getP(pixels);
	grad setGradients(pixels);
	void setContextsArray();
	int getContext(grad,grad, int&, bool&);
	int getContext_(int, int);
	int getPredictedValue(pixels);
	int getK(int);
	int getKPrime(Racha&);
	int rice(int,float,int);
	int rice_rachas(int,int,int,int&);
	void encode(int, int, ofstream&, int,int);
	void encode_(int, int, ofstream&);
	void updateContexto(int, int);
	void updateContexto_(int, int,int,int,int);
	void writeCode(ofstream&);	/* writecode() y flushEncoder() son métodos reciclados de la (propia) tarea del curso de Compresión de Datos Sin Pérdida*/
	void flushEncoder(ofstream&);
	void writeHeader(ofstream&);
	void writeHeaderVector(ofstream&); // Compensación de Movimiento
	void writeWidth(ofstream&,bool);
	void writeHeigth(ofstream&,bool);
	void writeWhite(ofstream&,bool);
	void writeMagic(ofstream&);
	void writeCompMov(ofstream&, bool); // Compensación de Movimiento
	void writeNmax(ofstream&);
	void writeCantidadImagenes(ofstream&);
	int getRachaParams(Image&, int, int, int&);
	int getRachaParams2(Image&, int, int, int&);
	int encodeRacha(Racha&);
	int encodeRacha2(Racha&);
	void encodeMuestraInterrupcion(Racha&, int,int, ofstream&, int);
	int reduccionDeRango(int);
	int fixPrediction(int,int, int);
	bool hasEnding (std::string const &fullString, std::string const &ending);
	string str_(int n);

	int max(int, int);

	int correctPredictedValue(int, int);

	int getProxImageAnterior(int, bool);

	virtual ~Coder();

	/* Este objeto representa la imagen a ser codificada */

	Image* images;

	Image image_; //imagen vacía para tener los parámetros que nos interesan, largo, ancho, white, etc.

	Image image;
	Image image2;

	int width, heigth, white, magic;
	int Nmax;		//el valor de Nmax
	int i;

	int contador=0; /*** eliminar ***/



	static const int CANTIDAD_MAXIMA_CONTEXTOS=9*9*9*9*9;

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
	bool debug2=false;
	bool debug3=false;
	bool debug4=false;

	int range;

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
	int qMax_;
	int RUNcnt;
	int RUNindex = 0;
	bool golombLimitado(int);

	int acum=0;

	ContextRun cntx[2];    // Contextos especiales para rachas.
	float get_s(int);

	string path;

	int cantidad_imagenes;
	/** Compensación de movimiento
		 *
		 */
		int *tempimage;
		int *h_vector;
		int *v_vector;
		bool activarCompMov=false;
		int vector_ind;
		int v_ancho;
		int v_alto;
		int v_blanco;
		int bsize = 10; // Tamaño del Macrobloque cuadrado
};

} /* namespace std */

#endif /* CODER_H_ */
