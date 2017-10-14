/**
*  @file Coder.cpp
*  @brief Realiza el trabajo de comprimir la imagen
*
*  @author Felipe Tambasco, Mauro Barbosa
*  @date Feb, 2017
*
*/

#include "Coder.h"
#include "ContextRun.h"
#include <sstream>
#include <math.h>

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <bitset>
#include <unistd.h>

#include <algorithm>

#include <sstream>

#include <iomanip>
#include <dirent.h>

#include "Coder.h"
#include "Decoder.h"

namespace std {
// PRUEBA !

Coder::Coder() {
}

Coder::Coder(Image img1, Image img2, int Nmax) {
	this->images = new Image[2];
	this->images[0]=img1;
	this->images[1]=img2;
	this->cantidad_imagenes=2;
	this->Nmax=Nmax;
	this->width=img1.width;
	this->heigth=img1.heigth;
	this->white=img1.white;
	this->image=setInitialImage();
	this->beta=max(2, ceil(log2(this->image.white+1)));
	this->Lmax=2*(max(2, ceil(log2(this->image.white+1)) )+max(8, max(2, ceil(log2(this->image.white+1)) )));
	this->qMax=Lmax-beta-1;
	this->qMax_=Lmax-beta-1;
	this->range=this->image.white+1;
}

Coder::Coder(string path, int Nmax, int aux) {
		//constructor

	this->Nmax=Nmax;
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir (path.c_str())) != NULL) {
		while ((ent = readdir (dir)) != NULL) if (hasEnding(ent->d_name,".pgm")) contador++;
		closedir(dir);
	}
	
	string* names = new string[contador];
	this->images = new Image[contador];
	int arch_actual=0;
	
	if ((dir = opendir (path.c_str())) != NULL) {
		while ((ent = readdir (dir)) != NULL) if (hasEnding(ent->d_name,".pgm")) names[arch_actual++]=ent->d_name;
		closedir(dir);
	}

	sort(names, names+contador);

	for (int k=0;k<contador;k++) this->images[k]=Image(path + names[k]);
	
	cantidad_imagenes=contador;
	this->width=images[0].width;
	this->heigth=images[0].heigth;
	this->white=images[0].white;
	this->image=setInitialImage();
	this->path=path;
	this->beta=max(2, ceil(log2(image.white+1)));
	this->Lmax=2*(max(2, ceil(log2(image.white+1)) )+max(8, max(2, ceil(log2(image.white+1)) )));
	this->qMax=Lmax-beta-1;
	this->qMax_=Lmax-beta-1;
	range=image.white+1;
}

bool Coder::hasEnding (std::string const &fullString, std::string const &ending) {

	if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

Image Coder::setInitialImage(){
	Image aux=Image();
	aux.image=(int*)malloc(width*heigth*sizeof(int));

	for (int k=0;k<width*heigth;k++) aux.image[k]=0; //	definir algún criterio para esta imagen

	aux.white=this->white;
	aux.width=this->width;
	aux.heigth=this->heigth;

	return aux;
}

 void Coder::code(bool vector, ofstream &salida){
	v_ancho = 1+(image.width/bsize);
	v_alto = 1+(image.heigth/bsize);
	v_blanco = 255;
	stringstream ss1;
	ss1 << Nmax;
	string nmax = ss1.str();
	Image* imageH = new Image();
	Image* imageV = new Image();

	cout << "// START CODER" << endl;

	if (!vector) writeHeader(salida);
	setContextsArray();

	for (int imagen=0;imagen<cantidad_imagenes;imagen++){
		cout << (vector ? "vector " : "imagen ") << "coder: " << imagen << endl;

		image2 = images[imagen];
		if (imagen > 0) image = images[imagen-1];

		if(!vector) cout << "Procesando imagen >> " << image2.path << endl;

		// ######## Compensación de movimiento
		if (activarCompMov && !vector){
			imageH->image=(int*)malloc(this->image.width*this->image.heigth*sizeof(int));
			imageV->image=(int*)malloc(this->image.width*this->image.heigth*sizeof(int));
			imageH->width=v_ancho;
			imageV->width=v_ancho;
			imageH->heigth=v_alto;
			imageV->heigth=v_alto;
			imageH->white=v_blanco;
			imageV->white=v_blanco;

			CompMov(*imageH, *imageV);

			for(int i=0; i<v_ancho*v_alto; i++){
				imageH->image[i]=0;
				imageV->image[i]=0;
			}

			Coder* coderVec = new Coder(*imageH, *imageV, Nmax);
			coderVec->code(1, salida);
		}

		for(int prox=0;prox<image.heigth*image.width;prox++){
			//bucle principal que recorre la imagen y va codificando cada pixel

			int signo;
			bool esRacha;
			int currentPixel=image2.image[prox]; //valor del pixel actual
			int prox_image_anterior=getProxImageAnterior(prox,vector, *imageH, *imageV);
			pixels3D pxls = getPixels3D(prox_image_anterior,prox);
			grad gradients=getGradients3D(1,pxls); //calcula los gradientes
			int contexto = getContext(getGradients3D(0,pxls),getGradients3D(4,pxls), signo, esRacha);

			if (!esRacha){
				int predicted = getPredictedValue(selectMED(gradients),pxls);	//calcula el valor pixel predicho
				predicted=fixPrediction(predicted,signo, contexto);
				int error_s= currentPixel-predicted;	//calcula el error como la resta entre el valor actual y el valor predicho
				int error_=error_s*signo;
				int k= getK(contexto);	//calcula k para ese contexto
				int error__=reduccionDeRango(error_);
				int error =rice(error__, get_s(contexto),k);	//devuelve mapeo de rice del error
				encode(error,k, salida,0,0);	//codifica el error
				updateContexto(contexto, error_);	//actualiza los valores para el contexto
			}
			else {
				int interruption=0;
				int largo= getRachaParams2(image2, prox, pxls.a, interruption);
				int contexto=getContext_(prox, largo);
				Racha racha(largo, interruption, pxls.a,contexto);
				int cantidad_unos=encodeRacha2(racha);
				encodeMuestraInterrupcion(racha, image2.image[prox+largo], prox+largo,salida,cantidad_unos);
				racha.updateContexto();
				prox=prox+largo;
				if ((racha.interruption))	prox--;	//
			}

		} //pixeles
	}//imagenes
	flushEncoder(salida);	//termina de escribir los últimos bits que hayan quedado en el array de bits
}

 void Coder::CompMov(Image &imageH, Image &imageV){
	int bh = bsize; 	// Aquí se guardará el ancho del Macrobloque cuando sea menor al especificado
    int bv = bsize; 	// Aquí se guardará el alto del Macrobloque cuando sea menor al especificado
    int search = 6;	// Distancia de búsqueda hacia cada dirección
  	int sIzq = 0; 		// Distancia que puede buscar hacia la izquierda, cerca de los bordes será menor a search
  	int sDer = 0;		// Distancia que puede buscar hacia la derecha, cerca de los bordes será menor a search
  	int sArr = 0;		// Distancia que puede buscar hacia la arriba, cerca de los bordes será menor a search
  	int sAba = 0;		// Distancia que puede buscar hacia la abajo, cerca de los bordes será menor a search
  	int restoV = 0;	// Variable temporal utilizada para calcular "bh" en el borde derecho
 	int restoH = 0;	// Variable temporal utilizada para calcular "bv" en el borde inferior
 	int derecha = 0;	// Variable temporal utilizada para calcular "sDer"
 	int abajo = 0;		// Variable temporal utilizada para calcular "sAba"

 	cout<<"Entro CompMov - "<<endl;

 	vector_ind = 0;// Inicializo en cero el índice para los vectores de movimiento
    int s;

    for(int bloqueV=0;(bloqueV)*bsize<image2.heigth;bloqueV++){
    	for(int bloqueH=0;(bloqueH)*bsize<image2.width;bloqueH++){
    		// Con estos dos FOR se recorren todos los Macrobloques
  			// Su posición es dada por (bloqueH, bloqueV)
  			// Se inicializan valores en cada Macrobloque
  			// -------------------------
  			int smin = 429496729;
  			int hmin = 0;
  			int vmin = 0;
  			// --------------------------

  			// Se calcula cuánto se puede mover el search y el tamaño real del macrobloque
  			// -----------------------------
  			restoV = max((bloqueV+1)*bsize-image2.heigth, 0);
  			restoH = max((bloqueH+1)*bsize-image2.width, 0);
  			derecha = abs((bloqueH+1)*bsize - image2.width);
  			abajo = abs((bloqueV+1)*bsize - image2.heigth);
  			sIzq = min(search,bloqueH*bsize);
  			sDer = min(search,derecha - restoH);
  			sArr = min(search,bloqueV*bsize);
  			sAba = min(search,abajo - restoV);
  			bh = bsize - restoH;
  			bv = bsize - restoV;
  			// ------------------------------

  			for(int v=-sArr;v<sAba+1;v++){
  				for(int h=-sIzq;h<sDer+1;h++){
  					// Estos FOR son para mover el macrobloque
					s = 0; // Se inicializa la variable sobre la que se va a iterar en cada Matching
					for(int j=0;j<bv;j++){
						for(int i=0;i<bh;i++){
							// Estos FOR son para moverse por todos los pixeles del Macrobloque
							int pix  = (i + j*image.width)  + (h + v*image.width) + (bloqueH*bsize + bloqueV*bsize*(image.width));
							int pix2 = (i + j*image2.width) + (bloqueH*bsize + bloqueV*bsize*(image2.width));
							itera("ERROR",s,pix,pix2);
						}
					}
					if (s < smin){
						// Se guardan los valores mínimos hasta el momento de este Macrobloque
						smin = s;
						hmin = h;
						vmin = v;
					}
				}
			}

			imageH.image[vector_ind] = hmin;
			imageV.image[vector_ind] = vmin;
			vector_ind++; // Muevo el puntero utilizado en los vectores
		}
  	}
 	cout <<"Salgo CompMov"<< endl;
}

 void Coder::itera(string function, int &s, int pix, int pix2){
  	/* Valores válidos para Function:
  	 * 	"MED3D": Utilizando el predictor 3D
  	 * 	"ERROR": Utilizando la diferencia entre pixeles
  	 * 	"LAPLACE": Utilizando laplaciano
  	 */

	if (function == "MED3D"){
  		pixels3D pxls = getPixels3D(pix, pix2);
  		grad gradientes = getGradients3D(1,pxls);
  		s = s + abs(getPredictedValue(selectMED(gradientes),pxls)-image2.image[pix2]);
  	}

	if (function == "ERROR"){
  		s = s + abs(image.image[pix]-image2.image[pix2]);
  	}

	if (function == "LAPLACE"){
  		int c = pix%image.width;
  		int f = ceil(pix/image.width);
  		s = s + abs((image.image[min(pix+1,(f+1)*image.width-1)]+image.image[max(pix-1,f*image.width)]+image.image[max(pix-image.width,c)]+image.image[min(pix+image.width,(image.heigth-1)*image.width+c)]-4*image.image[pix])-(image2.image[min(pix+1,(f+1)*image2.width-1)]+image2.image[max(pix-1,f*image2.width)]+image2.image[max(pix-image2.width,c)]+image2.image[min(pix+image2.width,(image2.heigth-1)*image2.width+c)]-4*image2.image[pix]));
  	}
}

int Coder::getProxImageAnterior(int prox, bool vector, Image imagenH, Image imagenV){
	int proxAnt=prox;

	if (activarCompMov && !vector){
 		int bloqueV = (prox / image.width) / bsize;
 	 	int bloqueH = (prox % image.width) / bsize;
		int ind = bloqueH + bloqueV * (1 + image.width / bsize);
		proxAnt = prox + imagenH.image[ind] + imagenV.image[ind] * image.width;
	}
	
	return proxAnt;
}

int Coder::max(int uno, int dos){
	 if (uno<dos) return dos;
	 else return uno;
}

float Coder::get_s(int contexto){
	 return float(float(contexts[contexto].B)/float(contexts[contexto].N)); //es N o N_?
}

int Coder::getRachaParams2(Image &image, int prox, int anterior, int &interruption_){
 	int largo=0;
 	interruption_=0;

 	while(anterior==image.image[prox+largo]){
 		largo++;
 		if((prox+largo)%(image.width)==0) {
 			interruption_=1;
 			break;
 		}
 	}

 	return largo;
}

int Coder::reduccionDeRango(int error){

	if (error>=(range+1)/2){
		error=error -range;
		if (debug) cout<<"error + predicted>255"<<endl;
	} else if (error<-((range)/2)){
	 		error=range+error;
	 		if (debug) cout<<"error + predicted<0"<<endl;
	} else {
		if (debug) cout<<"return error"<<endl;
	 		return error;
	}

	return error;
}

int Coder::getKPrime(Racha &r){
	int T_racha, K_racha;

	T_racha=cntx[r.contexto].A_racha+(cntx[r.contexto].N_racha>>1)*r.contexto;
	if (debug) cout<<"T_racha: "<<T_racha<<endl;
	for(K_racha=0; (cntx[r.contexto].N_racha<<K_racha)<T_racha; K_racha++);
	
	return K_racha;
}

void Coder::encodeMuestraInterrupcion(Racha &racha, int siguiente, int prox_, ofstream &salida,int cantidad_unos){
	int error=0, error_=0, kPrime=1000;
	int test;

	if (!racha.interruption){
		if (racha.contexto==0){
			error_=(siguiente-getPixels3D(0,prox_).b)*(-1);
			test=(siguiente-getPixels3D(0,prox_).b)*(-1);
		} else {
		error_=(siguiente-getPixels3D(0,prox_).b);
		test=(siguiente-getPixels3D(0,prox_).b);
		}

		kPrime=getKPrime(racha);
		error_=reduccionDeRango(error_);
		int map=0;
		error=rice_rachas(error_,racha.contexto,kPrime,map);
		updateContexto_(racha.contexto, error_,error,map,kPrime);
		encode(error, kPrime, salida,1,cantidad_unos);
	} else {
		writeCode(salida);
	}
}

int Coder::rice_rachas(int error,int contexto, int k, int &map){
	map=0;

	if ((k!=0)and(error<0))	map=1;

	if ((k==0)and(error<0)and((float(cntx[contexto].Nn_racha)/float(cntx[contexto].N_racha))>=0.5))	map=1;

	if ((k==0)and(error>0)and((float(cntx[contexto].Nn_racha)/float(cntx[contexto].N_racha))<0.5))	map=1;

	int retorno=2*abs(error)-contexto-map;

	return retorno;
}

int Coder::encodeRacha2(Racha &racha){
	int aux;
	int diferencia=racha.largo;
	int ajuste;

	while (diferencia >= (1 << J[RUNindex])) {
	   //Agregar un 1 al tream
		bitsToFile[bitsToFilePointer]=1;
		bitsToFilePointer++;
		diferencia = diferencia - (1 << J[RUNindex]);

		if (RUNindex < 31) {
			RUNindex++;
			if (debug) cout << "inc"<<endl;
		}
	}

	if ((diferencia!=0)and(racha.interruption)) {
		bitsToFile[bitsToFilePointer]=1;
		bitsToFilePointer++;
	} else if (!racha.interruption) {
		bitsToFile[bitsToFilePointer]=0;
		bitsToFilePointer++;

		int potencia = 1;

		for (int j=0;j<J[RUNindex];j++){
			potencia=potencia*2;
		}

		int cociente=diferencia/potencia;
		int resto=diferencia%potencia;
		potencia=potencia/2;

		for (int j=0;j<J[RUNindex];j++){
			bitsToFile[bitsToFilePointer]=resto/potencia;
			bitsToFilePointer++;
			resto=resto%potencia;
			potencia=potencia/2;
		}

		ajuste = J[RUNindex];
		if (RUNindex > 0) RUNindex--;
	}

	return (ajuste+1);
}

int Coder::fixPrediction(int predicted,int signo, int contexto){
	predicted=predicted+(contexts[contexto].C*signo);

	if (predicted< 0) predicted=0;
	if (predicted >range-1) predicted=range-1;

	return predicted;
}

void Coder::updateContexto(int contexto, int error){
	/** Actualiza los datos N y A del contexto */
	/** Actualiza B y C */

	contexts[contexto].B=contexts[contexto].B+error;
	contexts[contexto].A=contexts[contexto].A+abs(error);

	if (contexts[contexto].N==Nmax){
		/* si el valor de N para ese contexto es igual a Nmax divide N y A entre 2 */
		contexts[contexto].N=contexts[contexto].N/2;
		contexts[contexto].N_=contexts[contexto].N_/2;
		contexts[contexto].A=floor((double)contexts[contexto].A/(double)2);
		contexts[contexto].B=floor((double)contexts[contexto].B/(double)2);
	}

	contexts[contexto].N++;	//actualiza N

	if (error<0) contexts[contexto].N_++;

	/* Actualiza A sumándole el valor absoluto de este error */

	if (contexts[contexto].B<=-contexts[contexto].N){
		contexts[contexto].B=contexts[contexto].B+contexts[contexto].N;
		if (contexts[contexto].C>-128) contexts[contexto].C=contexts[contexto].C-1;
		if (contexts[contexto].B<=-contexts[contexto].N) contexts[contexto].B=-contexts[contexto].N+1;
	} else if (contexts[contexto].B>0){
		contexts[contexto].B=contexts[contexto].B-contexts[contexto].N;
		if (contexts[contexto].C<127) contexts[contexto].C=contexts[contexto].C+1;
		if (contexts[contexto].B>0) contexts[contexto].B=0;
	}
}

void Coder::flushEncoder(ofstream &salida){
	/** Completa con ceros para poder escribir los últimos bits */

	if (bitsToFilePointer>0) {
		bitset<8> temp_b;

		for(int j=0;j<bitsToFilePointer;j++) temp_b[7-j]=bitsToFile[j];
		char temp=(char)temp_b.to_ulong();
		salida.write(&temp, 1);
	}
}

void Coder::encode(int error, int k, ofstream &salida, int racha,int ajuste){
	/** Almacena en potencia el valor de 2^k
	Calcula la parte entera del cociente entre el error y 2^k y lo guarda en "cociente" para codificación binaria
	Calcula el resto de la división entera entre el error y 2^k y lo guarda en "resto" para codificación unaria */

	int qMax;

	if (racha) {
		qMax=this->qMax_;
		qMax=qMax-ajuste;
	} else {
		qMax=this->qMax;
	}

	if (k!=1000){
		int potencia = floor(pow(2,k));
		if (potencia==0)	potencia=1;
		int cociente=error/potencia;
		int resto=error%potencia;
		potencia=potencia/2;

		if (cociente>=qMax) {
			cociente=qMax;
		}

		for (int j=0;j<cociente;j++){
			bitsToFile[bitsToFilePointer]=0;
			bitsToFilePointer++;
		}

		if(cociente==qMax){
		} else {
			bitsToFile[bitsToFilePointer]=1;
			bitsToFilePointer++;
		}

		if (cociente==qMax)	{
			cociente=qMax;
			potencia=pow(2,beta-1);
			for (int j=0;j<beta;j++){
				bitsToFile[bitsToFilePointer]=error/potencia;
				bitsToFilePointer++;
				error=error%potencia;
				potencia=potencia/2;
			}
		} else {
			/*	Este loop calcula la expresión binaria del resto expresada con k bits, y lo guarda en array auxiliar bitsToFile */
			for (int j=0;j<k;j++){
				bitsToFile[bitsToFilePointer]=resto/potencia;
				bitsToFilePointer++;
				resto=resto%potencia;
				potencia=potencia/2;
			}
			/* Este loop calcula la expresión unaria del cociente, con tantos ceros como la variable "cociente"
			y lo guarda en array auxiliar bitsToFile */
		}
	}
	writeCode(salida);
}

void Coder::writeCode(ofstream &salida){
	/** Si hay al menos un byte para escribir en bitsToFile se escriben tantos bytes como es posible,
	esto es, el cociente de la división entera entre bitsToFilePointer y 8 */

	if (bitsToFilePointer>7){
		for(int k=0;k<(bitsToFilePointer/8);k++){
			/* Para escribir el byte, se usa una estructura auxiliar, bitset,
			se guarda en un bitset los próximos 8 bits a ser escritos,
			luego se pasa este bitset a char, y por último se escribe el char */
			std::bitset<8> temp_b;

			for(int j=0;j<8;j++){
				temp_b[7-j]=bitsToFile[k*8+j];
			}

			char temp=(char)temp_b.to_ulong();
			salida.write(&temp, 1);
		}
		/* Todos los bits de la cola que no pudieron escribirse,
		esto es, los últimos bitsToFilePointer%8 bits válidos, son corridos al principio de la fila,
		para ser los primeros en escribirse cuando vuelva a invocarse este método */

		for(int k=8*(bitsToFilePointer/8);k<bitsToFilePointer;k++){
			bitsToFile[k-8*(bitsToFilePointer/8)]=bitsToFile[k];
		}

		/* Se actualiza el valor del puntero */
		bitsToFilePointer=bitsToFilePointer%8;
	}//if
}

int Coder::rice(int error, float s, int k){
	/** Mapeo de rice del error */
	/** falta reducción de rango para el error ***/

	if ((k<=0)and(s<=-0.5)){
		return rice(-error-1,0,1);	//en este caso llama al mapeo común de rice, con otro parámetro
	} else {
		int uno =1;
		if (error>=0)uno=0;
		return (2*abs(error)-uno);
	}
}

int Coder::getK(int contexto){
	/** Calcula k según la expresión de las diapositivas del curso */

	double AdivN_=(double)contexts[contexto].A/(double)contexts[contexto].N;

	return round(log2(AdivN_));
}

int Coder::getContext(grad gradients1,grad gradients2, int &signo, bool &racha){
	/** Determina el contexto
	Todos los contextos posibles se organizan en un array, donde cada elemento del array representa un contexto,
	es posible definir un mapeo entre el espacio de todos los contextos posibles y los enteros,
	para que dado un contexto haya una relación biunívoca con un elemento del array */

	signo=1;
	int contga, contgb,contgc,contgd,contge;

	if (gradients1.ga<=-21) contga=0;
		else if (gradients1.ga<=-7) contga=1;
		else if (gradients1.ga<=-3) contga=2;
		else if (gradients1.ga<0) contga=3;
		else if (gradients1.ga==0) contga=4;
		else if (gradients1.ga<3) contga=5;
		else if (gradients1.ga<7) contga=6;
		else if (gradients1.ga<21) contga=7;
		else contga=8;

	if (gradients1.gb<=-21) contgb=0;
		else if (gradients1.gb<=-7) contgb=1;
		else if (gradients1.gb<=-3) contgb=2;
		else if (gradients1.gb<0) contgb=3;
		else if (gradients1.gb==0) contgb=4;
		else if (gradients1.gb<3) contgb=5;
		else if (gradients1.gb<7) contgb=6;
		else if (gradients1.gb<21) contgb=7;
		else contgb=8;

	if (gradients1.gc<=-21) contgc=0;
		else if (gradients1.gc<=-7) contgc=1;
		else if (gradients1.gc<=-3) contgc=2;
		else if (gradients1.gc<0) contgc=3;
		else if (gradients1.gc==0) contgc=4;
		else if (gradients1.gc<3) contgc=5;
		else if (gradients1.gc<7) contgc=6;
		else if (gradients1.gc<21) contgc=7;
		else contgc=8;

	if (gradients2.gb<=-21) contgd=0;
		else if (gradients2.gb<=-7) contgd=1;
		else if (gradients2.gb<=-3) contgd=2;
		else if (gradients2.gb<0) contgd=3;
		else if (gradients2.gb==0) contgd=4;
		else if (gradients2.gb<3) contgd=5;
		else if (gradients2.gb<7) contgd=6;
		else if (gradients2.gb<21) contgd=7;
		else contgd=8;

	if (gradients2.gc<=-21) contge=0;
		else if (gradients2.gc<=-7) contge=1;
		else if (gradients2.gc<=-3) contge=2;
		else if (gradients2.gc<0) contge=3;
		else if (gradients2.gc==0) contge=4;
		else if (gradients2.gc<3) contge=5;
		else if (gradients2.gc<7) contge=6;
		else if (gradients2.gc<21) contge=7;
		else contge=8;

	if(contga<4){
		contga=8-contga;
		contgb=8-contgb;
		contgc=8-contgc;
		contgd=8-contgd;
		contge=8-contge;

		signo=-1;
	} else if((contga==4)and(contgb<4)) {
		contga=8-contga;
		contgb=8-contgb;
		contgc=8-contgc;
		contgd=8-contgd;
		contge=8-contge;

		signo=-1;
	} else if((contga==4)and(contgb==4)and(contgc<4)) {
		contga=8-contga;
		contgb=8-contgb;
		contgc=8-contgc;
		contgd=8-contgd;
		contge=8-contge;

		signo=-1;
	}

	else if((contga==4)and(contgb==4)and(contgc==4)and(contgd<4)) {
		contga=8-contga;
		contgb=8-contgb;
		contgc=8-contgc;
		contgd=8-contgd;
		contge=8-contge;

		signo=-1;
	}

	else if((contga==4)and(contgb==4)and(contgc==4)and(contgd==4)and(contge<4)) {
		contga=8-contga;
		contgb=8-contgb;
		contgc=8-contgc;
		contgd=8-contgd;
		contge=8-contge;

		signo=-1;
	}
	//mapeo elegido para representar los contextos
	if ((9*9*9*9*contga)+(9*9*9*contgb)+(9*9*contgc)==29484) racha=true;
		else racha=false;

	return (9*9*9*9*contga)+(9*9*9*contgb)+(9*9*contgc)+(9*contgd)+contge;
}

void Coder::setContextsArray(){
	/** Forma el array con todos los contextos posibles */

	int indice=0;

	for (int k=-4;k<5;k++){
		for (int j=-4;j<5;j++){
			for (int i=-4;i<5;i++){
				for (int l=-4;l<5;l++){
					for (int m=-4;m<5;m++){
						Context contexto(k,j,i,l,m,image.white);
						contexts[indice]=contexto;
						indice++;
					}
				}
			}
		}
	}
}

Coder::grad Coder::setGradients(pixels pxls){
	/** Dado p y los píxeles a, b, c y d de la vecindad,
	forma el vector de gradientes */

	grad gradients={pxls.d-pxls.b,pxls.b-pxls.c,pxls.c-pxls.a};

	return gradients;
}

Coder::pixels3D Coder::getPixels3D(int current, int current2){
	/** Devuelve los píxeles de la vecindad: a, b, c, d, a_, b_, c_, d_, e_, f_ y g_ */
    // A mi por lo menos sin (double) y con (>> 1) me corre mucho mas rapido, para stacks grandes (200+) se empieza a notar.
	/**  arreglar criterio para los píxeles que caen fuera de la imagen*/

	int a=-1;
	int b=-1;
	int c=-1;
	int d=-1;
	int a_=-1;
	int b_=-1;
	int c_=-1;
	int d_=-1;
	int e_=-1;
	int f_=-1;
	int g_=-1;

	if ((current2%image2.width)==0){
		/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
		o la mitad del valor de blanco de la imagen */
		a = 1 + (image2.white >> 1);
		c = 1 + (image2.white >> 1);
	}

	if ((current2%image2.width)==image.width-1){
		/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
		o la mitad del valor de blanco de la imagen */

		d = 1 + (image2.white >> 1);
	}

	if (current2<image2.width){
		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */

		if(b == -1) b = 1 + (image2.white >> 1);
		if(c == -1) c = 1 + (image2.white >> 1);
		if(d == -1) d = 1 + (image2.white >> 1);
	}

	if ((current%image.width)==0){
		/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
		o la mitad del valor de blanco de la imagen */

		a_ = 1 + (image.white >> 1);
		c_ = 1 + (image.white >> 1);
	}

	if ((current%image.width)==image.width-1){
		/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
		o la mitad del valor de blanco de la imagen */

		d_ = 1 + (image.white >> 1);
		f_ = 1 + (image.white >> 1);
	}

	if (current<image.width){
		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */
			
			if(b_ == -1) b_ = 1 + (image.white >> 1);
			if(c_ == -1) c_ = 1 + (image.white >> 1);
			if(d_ == -1) d_ = 1 + (image.white >> 1);
	}

	if (current>(image.heigth-2)*image.width){
		/* Si estoy en la última o penúltima fila, g debe ser "128"
		o la mitad del valor de blanco de la imagen */
			
		g_ = 1 + (image.white >> 1);
	}

	/* Para cada a, b,c y d, si no se cumple una condición de borde, y por lo tanto no hubo asignación en los if que preceden,
	se traen los valores de a, b,c y d de la imagen */
	if (a==-1)  a=image2.image[current2-1];
	if (b==-1)  b=image2.image[current2-image.width];
	if (c==-1)  c=image2.image[current2-image.width-1];
	if (d==-1)  d=image2.image[current2-image.width+1];
	if (a_==-1) a_=image.image[current-1];
	if (b_==-1) b_=image.image[current-image.width];
	if (c_==-1) c_=image.image[current-image.width-1];
	if (d_==-1) d_=image.image[current-image.width+1];
	if (e_==-1) e_=image.image[current];
	if (f_==-1) f_=image.image[current+1];
	if (g_==-1) g_=image.image[current+image.width];
	
	return {a,  b,  c,  d,
		    a_, b_, c_, d_,
			e_, f_, g_};
}

Coder::pixels Coder::getPixels_(int current){
	/** Devuelve los píxeles de la vecindad: a, b y c */

	int a=-1;
	int b=-1;
	int c=-1;
	int d=-1;

	if (current==0){
		//primer píxel
		a=0;
		b=0;
		c=0;
		d=0;
	} else if ((current%image2.width)==0){
		/* columna izquierda */
		a=image2.image[current-image2.width];
		c=getPixels_(current-image2.width).a;
	}

	if (current<image2.width){
		//primer fila
		if (b==-1) b=0;
		if (c==-1) c=0;
		if (d==-1) d=0;
	} else if ((current%image2.width)==image2.width-1){
		/* columna derecha */
		d=image2.image[current-image.width];
	}
	/* Para cada a, b,c y d, si no se cumple una condición de borde, y por lo tanto no hubo asignación en los if que preceden,
	se traen los valores de a, b,c y d de la imagen */

	if (a==-1) a=image2.image[current-1];
	if (b==-1) b=image2.image[current-image.width];
	if (c==-1) c=image2.image[current-image.width-1];
	if (d==-1) d=image2.image[current-image.width+1];

	pixels pxls={a,b,c,d};

	return pxls;
}

void Coder::writeHeader(ofstream &salida){
	/** Escribe el encabezado de la imagen codificada,
	para esto se sigue el mismo esquema presente en el archvo .pgm,
	con el agregado de escribir también el valor de Nmax
	los 5 métodos que se usan para escribir el encabezado, que se listan a continuación,
	siguen la misma estructura interna general */

	writeMagic(salida);
	writeWidth(salida,false);
	writeHeigth(salida,false);
	writeWhite(salida,false);
	writeNmax(salida);
	writeCantidadImagenes(salida);
	writeCompMov(salida,activarCompMov);
}

void Coder::writeCantidadImagenes(ofstream &salida){
	/** Se lleva el valor de Nmax a un double de la forma 0,Nmax
	luego se multiplica entre 10 y se redondea para quedarse
	con cada digito de Nmax y poder escribirlos como chars */

	int nmax =cantidad_imagenes;
	double aux=(double)nmax;
	int potencia=1;

	while(aux>=1){
		aux=aux/10;
		potencia=potencia*10;
	}	//calcula cuál es el orden de Nmax, 10, 100, 1000, etc... y deja a aux (Nmax) en un valor entre 0 y 1

	char temp_;
	int temp=0;

	while(potencia>1){
		aux=aux-(double)temp;	//luego de que ya fue escrito el digito anterior,
								//se hace esta resta para eliminar del decimal el valor que ya fue escrito,
								//por ejemplo, si de 0.256 pasamos a 2.56, luego de la resta se tiene el
								//número 0.56, para que pueda volver a ser multiplicado por 10, escribir el 5 y así siguiendo...
		aux=aux*double(10);
		if (double(ceil(aux))-(double)aux<(double)0.00001)
						temp=ceil(aux);			//parche artesanal, algunos números uno los ve como
												//cierto valor, pero al tomar el floor te da el entero
												//anterior, suponemos que si bien uno lo ve como el número n
												//para la máquina es (n-1),9999999999
		else temp=floor(aux);

		temp_=temp+'0';	//pasa el entero a char para escribirlo
		salida.write(&temp_,1);
		potencia=potencia/10;
	}
	temp_='\n';	//por último escribe un salto de línea
	salida.write(&temp_,1);
}

void Coder::writeNmax(ofstream &salida){
	/** Se lleva el valor de Nmax a un double de la forma 0,Nmax
	luego se multiplica entre 10 y se redondea para quedarse
	con cada digito de Nmax y poder escribirlos como chars */

	int nmax =Nmax;
	double aux=(double)nmax;
	int potencia=1;

	while(aux>1){
		aux=aux/10;
		potencia=potencia*10;
	}	//calcula cuál es el orden de Nmax, 10, 100, 1000, etc... y deja a aux (Nmax) en un valor entre 0 y 1

	char temp_;
	int temp=0;

	while(potencia>1){
		aux=aux-(double)temp;	//luego de que ya fue escrito el digito anterior,
								//se hace esta resta para eliminar del decimal el valor que ya fue escrito,
								//por ejemplo, si de 0.256 pasamos a 2.56, luego de la resta se tiene el
								//número 0.56, para que pueda volver a ser multiplicado por 10, escribir el 5 y así siguiendo...
		aux=aux*double(10);

		if (double(ceil(aux))-(double)aux<(double)0.00001)
			temp=ceil(aux);			//parche artesanal, algunos números uno los ve como
								//cierto valor, pero al tomar el floor te da el entero
								//anterior, suponemos que si bien uno lo ve como el número n
								//para la máquina es (n-1),9999999999
		else temp=floor(aux);

		temp_=temp+'0';	//pasa el entero a char para escribirlo
		salida.write(&temp_,1);
		potencia=potencia/10;
	}

	temp_='\n';	//por último escribe un salto de línea
	salida.write(&temp_,1);
}

void Coder::writeMagic(ofstream &salida){
	/** Como solo se trabaja con imagenes tipo P5,
	directamente se escriben estos caracteres*/

	char temp='P';
	salida.write(&temp,1);
	temp='5';
	salida.write(&temp,1);
	temp='\n';
	salida.write(&temp,1);
}

void Coder::writeCompMov(ofstream &salida, bool compMov){
	/** Como solo se trabaja con imagenes tipo P5,
	directamente se escriben estos caracteres*/
	char temp;
	if (compMov) {
		temp='1';
		salida.write(&temp,1);
		temp='\n';
		salida.write(&temp,1);
		writeWidth(salida,compMov);
		writeHeigth(salida,compMov);
		writeWhite(salida,compMov);
	} else {
		temp='0';
		salida.write(&temp,1);
		temp='\n';
		salida.write(&temp,1);
	}
}

void Coder::writeWidth(ofstream &salida,bool vector){
	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int ancho (vector ? v_ancho : image.width);
	double aux=(double)ancho;
	int potencia=1;

	while(aux>1){
		aux=aux/10;
		potencia=potencia*10;
	}

	char temp_;
	int temp=0;

	while(potencia>1){
		aux=aux-(double)temp;
		aux=aux*double(10);
		if (double(ceil(aux))-(double)aux<(double)0.00001)
			temp=ceil(aux);			//parche artesanal, algunos números uno los ve como
									//cierto valor, pero al tomar el floor te da el entero
									//anterior, suponemos que si bien uno lo ve como el número n
									//para la máquina es (n-1),9999999999
		else temp=floor(aux);
		temp_=temp+'0';
		salida.write(&temp_,1);
		potencia=potencia/10;
	}

	temp_=' ';
	salida.write(&temp_,1);
}
void Coder::writeHeigth(ofstream &salida, bool vector){
	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int alto (vector ? v_alto : image.heigth);
	double aux=(double)alto;
	int potencia=1;

	while(aux>1){
		aux=aux/10;
		potencia=potencia*10;
	}

	char temp_;
	int temp=0;

	while(potencia>1){
		aux=aux-(double)temp;
		aux=aux*double(10);
		if (double(ceil(aux))-(double)aux<(double)0.00001)
				temp=ceil(aux);			//parche artesanal, algunos números uno los ve como
										//cierto valor, pero al tomar el floor te da el entero
										//anterior, suponemos que si bien uno lo ve como el número n
										//para la máquina es (n-1),9999999999
		else temp=floor(aux);

		temp_=temp+'0';
		salida.write(&temp_,1);
		potencia=potencia/10;
	}

	temp_='\n';
	salida.write(&temp_,1);
}

void Coder::writeWhite(ofstream &salida, bool vector){
	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int blanco (vector ? v_blanco : image.white);
	double aux=(double)blanco;
	int potencia=1;

	while(aux>1){
		aux=aux/10;
		potencia=potencia*10;
	}

	char temp_;
	int temp=0;

	while(potencia>1){
		aux=aux-(double)temp;
		aux=aux*double(10);
		if (double(ceil(aux))-(double)aux<(double)0.00001)
			temp=ceil(aux);			//parche artesanal, algunos números uno los ve como
									//cierto valor, pero al tomar el floor te da el entero
									//anterior, suponemos que si bien uno lo ve como el número n
									//para la máquina es (n-1),9999999999
		else temp=floor(aux);

		temp_=temp+'0';
		salida.write(&temp_,1);
		potencia=potencia/10;
	}

	temp_='\n';
	salida.write(&temp_,1);
}

int Coder::correctPredictedValue(int pred, int contexto){

	return pred + contexts[contexto].C;
}

int Coder::getContext_(int pos, int lar){

	return (getPixels3D(0,pos).a==getPixels3D(0,pos+lar).b);
}

void Coder::updateContexto_(int c, int err,int err_,int map,int k){
	cntx[c].updateA((err_+1-c)>>1);
	cntx[c].updateNn(err);

	if(cntx[c].N_racha==RESET) {
		cntx[c].reset();
	}

	cntx[c].updateN();
}

int Coder::selectMED(grad gradients){
	int gz = abs(gradients.ga);
	int gx = abs(gradients.gb);
	int gy = abs(gradients.gc);
	int modo = 3;

	if ((gz>=gx)&&(gz>=gy)){
		modo = 0;
	}
	if ((gx>=gz)&&(gx>=gy)){
		modo = 2;
	}

	return modo;
}

Coder::grad Coder::getGradients3D(int modo, pixels3D pxls){
	/** Devuelve los gradientes según el modo de funcionamiento elegido:
	 * Modos:
	 * 		1 = Devuelve los gradientes de elección de predictor
	 * 		2 = Devuelve los gradientes del predictor filas-tiempo
	 * 		3 = Devuelve los gradientes del predictor columnas-tiempo
	 * 		0 o cualquier otros = Devuelve los gradientes del predictor filas-columnas
	 *  */

	grad gradients={pxls.d-pxls.b,pxls.b-pxls.c,pxls.c-pxls.a};

	if (modo == 1){
			gradients={pxls.a-pxls.a_,pxls.b-pxls.c,pxls.c-pxls.a};
	}
	if (modo == 2){
			gradients={pxls.g_-pxls.e_,pxls.e_-pxls.b_,pxls.b_-pxls.b};
	}
	if (modo == 3){
			gradients={pxls.f_-pxls.e_,pxls.e_-pxls.a_,pxls.a_-pxls.a};
	}
	if (modo == 4){
				gradients={0,pxls.b-pxls.b_,pxls.a-pxls.a_};
	}

	return gradients;
}


int Coder::getPredictedValue(int modo, pixels3D pxls){
	/** Calcula el valor predicho según expresión de las diapositivas del curso */

	int pred;
	int error;

	if (modo == 0){
		if ((pxls.c>=pxls.a)&&(pxls.c>=pxls.b)){
			if (pxls.a>pxls.b)
				pred = pxls.b;
			else pred = pxls.a;
		} else if ((pxls.c<=pxls.a)&&(pxls.c<=pxls.b)){
			if (pxls.a>pxls.b)
				pred = pxls.a;
			else pred = pxls.b;
		} else pred = (pxls.a+pxls.b-pxls.c);
	}

	if (modo == 2) {
		if ((pxls.b_>=pxls.e_)&&(pxls.b_>=pxls.b)){
			if (pxls.e_>pxls.b)
				pred = pxls.b;
			else pred = pxls.e_;
		}else if ((pxls.b_<=pxls.e_)&&(pxls.b_<=pxls.b)){
			if (pxls.e_>pxls.b)
				pred = pxls.e_;
			else pred = pxls.b;
		}else pred = (pxls.e_+pxls.b-pxls.b_);
	}

	if (modo == 3){
		if ((pxls.a_>=pxls.e_)&&(pxls.a_>=pxls.a)){
				if (pxls.e_>pxls.a)
					pred = pxls.a;
				else pred = pxls.e_;
			}else if ((pxls.a_<=pxls.e_)&&(pxls.a_<=pxls.a)){
				if (pxls.e_>pxls.a)
					pred = pxls.e_;
				else pred = pxls.a;
			}else pred = (pxls.e_+pxls.a-pxls.a_);
	}

	return pred;
}

Coder::~Coder() {

}

} /* namespace std */
