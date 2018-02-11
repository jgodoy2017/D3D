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
//#include "Writer.h"
#include "Writer2.h"
//#include "Reader.h"
#include "pubtypes.h"

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

	Image setInitialImage();

	int code_dcmtk(Writer2& writer,
			const void* uncompressedData,Image& image, struct JlsParameters* info,bool primeraImagen);

	//return type distinto a charls

	void escribeEncabezado_dcmtk(Writer2 &writer,struct JlsParameters* info);
	void CompMov(Image&, Image&); // Compensación de movimiento
	void drawLine(int,int,int,int,int,int);
	void drawTemp(int,int,int);
	void dibujarVector(int,int,int,int,int,int);
	void dibujoVectorFile();
	void itera(string, int&, int, int, int, int); // Compensación de movimiento
	int selectMED(grad);
	int getPredictedValue(int, pixels3D);
	grad getGradients3D(int, pixels3D);
	pixels getPixels(int);
	pixels getPixels_(int);
	pixels3D getPixels3D(int,int,int,int);
	int getP(pixels);
	grad setGradients(pixels);
	void setContextsArray();
	int getContext(grad,grad, int&, bool&);
	int getContext_(int,int,int);
	int getPredictedValue(pixels);
	int getK(int);
	int getKPrime(Racha&);
	int rice(int,float,int);
	int rice_rachas(int,int,int,int&);
	//void encode(int, int, Writer&, int,int);
	void encode(int, int, Writer2&, int,int);
	//void encode_(int, int, Writer&);
	void updateContexto(int, int);
	void updateContexto_(int, int,int,int,int);
	float varianza(float,int,int[]);
	void setTempImage();
	int getRachaParams(Image&, int, int, int&);
	int getRachaParams2(Image&, int, int, int, int&);
	int encodeRacha(Racha&);
	int encodeRacha2(Racha&, Writer2&);
	void encodeMuestraInterrupcion(Racha&, int,int, int,Writer2&, int);
	int reduccionDeRango(int);
	int fixPrediction(int,int, int);
	bool hasEnding (std::string const &fullString, std::string const &ending);
	string str_(int n);
	string c_str_(char n);

	void cargar_imagen_actual(const void* uncompressedData,struct JlsParameters* info);


	int max(int, int);

	int correctPredictedValue(int, int);

	void getProxImageAnterior(int, int, int&, int&, bool, Image, Image);

	virtual ~Coder();

	/* Este objeto representa la imagen a ser codificada */

	Image* images;

	Image image_; //imagen vacía para tener los parámetros que nos interesan, largo, ancho, white, etc.

	Image image;
	Image image2;

	//Writer writer;
	int width, heigth, white, magic;
	int Nmax;		//el valor de Nmax
	int i;

	int contador=0; /*** eliminar ***/

	int cantidad_bits=0;



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
		bool activarVarianza=true;
		bool dibujarVectores=false;
		int vector_ind;
		int v_ancho;
		int v_alto;
		int v_blanco;
		int bsize = 10; // Tamaño del Macrobloque cuadrado
		int search = 6;
};

} /* namespace std */

#endif /* CODER_H_ */
