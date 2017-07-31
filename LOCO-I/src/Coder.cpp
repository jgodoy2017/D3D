/**
*  @file Coder.cpp
*  @brief Realiza el trabajo de comprimir la imagen
*
*  @author Felipe Tambasco, Mauro Barbosa
*  @date Feb, 2017
*
*/

#include "Coder.h"
#include <sstream>
#include <math.h>

namespace std {



Coder::Coder() {


}

Coder::Coder(Image image, int Nmax) {

		//constructor

	this->Nmax=Nmax;

	this->image=image;




}

Coder::Coder(Image image, int Nmax, int aux) {

		//constructor

	this->Nmax=Nmax;

	this->image=image;

	this->aux=(aux==1);

	/**
	 *
	 * CAMBIOS !!
	 */

	this->beta=8;
	this->Lmax=4*beta;
	this->qMax=Lmax-beta-1;

}

 void Coder::code(){

	stringstream ss1;

	ss1 << Nmax;
	string nmax = ss1.str();

	string aux_="";

	if (aux) aux_="1";
	else aux_="0";

	string path_salida=image.path+image.name+"_coded_Nmax_"+nmax+"_"+aux_;
	ofstream salida;
	salida.open(path_salida.c_str(), ios::binary);

	writeHeader(salida);

	setContextsArray();

	int maximo_=0;

	for(int prox=0;prox<image.heigth*image.width;prox++){



							//bucle principal que recorre la imagen y va codificando cada pixel

		int currentPixel=image.image[prox]; //valor del pixel actual

		pixels pxls = getPixels(prox); //obtiene los píxeles de la vecindad: a,b y c

		//if (prox==267870) debug=true; else debug=false;

		if (debug) cout<<currentPixel<<" "<<prox<<" " << pxls.a<<" "<< pxls.b<<" "<< pxls.c<<" "<< pxls.d<<endl;



		//cout<<prox<<" " << pxls.a<<" "<< pxls.b<<" "<< pxls.c<<" "<< pxls.d<<endl;


		// int p = getP(pxls);	//calcula p

		grad gradients=setGradients(pxls); //calcula los gradientes

		//cout<<prox<<" " << gradients.ga<<" "<< gradients.gb<<" "<< gradients.gc<<" "<<endl;

		int contexto = getContext(gradients);	//trae el contexto asociado a ese gradiente

		if (debug )cout<<contexto<<endl;

		if (aux){

		if (contexto==364) {
			racha=true;
			if (debug) cout<<"RACHA!"<<endl;
		}
		else racha =false;

		}

		if (!racha){
		//if (true){

		int predicted = getPredictedValue(pxls);	//calcula el valor pixel predicho

		if (debug) cout<<predicted<<endl;

		predicted=fixPrediction(predicted, contexto);

		if (debug) cout<<predicted<<endl;

		int error_= currentPixel-predicted;	//calcula el error como la resta entre el valor actual y el valor predicho

		if (debug) cout<<"error_= "<<error_<<endl;

		int k= getK(contexto);	//calcula k para ese contexto

		if (debug) cout<<"k= "<<k<<endl;

		int error__=reduccionDeRango(error_);

		if (debug) cout<<"error con reducción de rango= "<<error_<<endl;

		int error =rice(error__);	//devuelve mapeo de rice del error

		maximo_=max(maximo_,error);



		//if (maximo_==258) cout<<prox<<endl;

		if (debug) cout<<"error= "<<error<<endl;

		encode(error,k, salida);	//codifica el error

		updateContexto(contexto, error_);	//actualiza los valores para el contexto

		}

		else {

			int interruption=0;
			int largo= getRachaParams(image, prox, pxls.a, interruption);

			int contexto=(pxls.a==pxls.b);

			Racha racha(largo, interruption, pxls.a,contexto);

			if (debug) cout<<prox<<" "<<largo<<" "<<interruption<<" "<<pxls.a<<" "<<contexto<<endl;

			if (debug) cout<<"actual: "<<image.image[prox]<<endl;

			encodeRacha(racha);

			encodeMuestraInterrupcion(racha, image.image[prox+largo],salida);

			racha.updateContexto();

			prox=prox+largo;

			if (debug) cout<<"prox= "<<prox<<endl;

			if (racha.interruption)	prox--;


		}

		if (debug) cout<<endl;
	}

	flushEncoder(salida);	//termina de escribir los últimos bits que hayan quedado en el array de bits

	salida.close();

	cout<<maximo_<<endl;

	 ifstream in(path_salida.c_str(), std::ifstream::ate | std::ifstream::binary);
	 cout<<"nmax= "<<nmax<<" rachas: "<<aux<<" tamaño: "<< in.tellg()<<endl;

}

 int Coder::max(int uno, int dos){

	 if (uno<dos) return dos;
	 else return uno;

 }

 int Coder::reduccionDeRango(int error){

	 int cociente=0;

	 int mod=error%128;
	 if (error<0)
		 cociente=error/129;
	 else
		 cociente=error/128;


	 if (cociente*cociente==1){

		 if (cociente==1){

			 error=-255+error-1;

		 }else{
			 error=-error;
			 error=255-error+1;

		 }

	 }

 	return error;
 }



int Coder::getKPrime(){

	return 3;
}
void Coder::encodeMuestraInterrupcion(Racha &racha, int siguiente, ofstream &salida){


	int error=0, error_=0, kPrime=1000;

	if (!racha.interruption){

	error_=siguiente-racha.pixel;

	error_=reduccionDeRango(error_);

	error=rice(error_);

	kPrime=getKPrime();

	}

	if (debug) cout<<"error: "<<error<<" "<<error_<<endl;



	encode(error, kPrime, salida);
}

void Coder::encodeRacha(Racha &racha){

	m_r=0;
	kr=0;

	int cantidad_unos=0;
	int diferencia=racha.largo;

	//if (debug) cout<<diferencia-m_r<<endl;

	while ((diferencia=diferencia-m_r)>=0){


	m_r=pow(2,J[kr]);

	//if (debug) cout<<m_r<<endl;

	cantidad_unos++;

	kr++;
	}kr--;

	if (diferencia<0) {
		cantidad_unos--;
		diferencia=diferencia+m_r;
	}

	if (debug) cout<<"cantidad_unos: "<<cantidad_unos<<endl;

	for (int j=0;j<cantidad_unos;j++){

				bitsToFile[bitsToFilePointer]=1;
				if (debug) cout<<bitsToFile[bitsToFilePointer];

				bitsToFilePointer++;

			}

	bitsToFile[bitsToFilePointer]=racha.interruption;
	if (debug) cout<<bitsToFile[bitsToFilePointer]<<endl<<"bitsToFilePointer= "<<bitsToFilePointer<<endl;

	bitsToFilePointer++;

	if (!racha.interruption){

		//diferencia=diferencia+m_r;

		//diferencia=-diferencia;

		int potencia = 1;

			for (int j=0;j<kr;j++){

				potencia=potencia*2;
			}

			int cociente=diferencia/potencia;
			int resto=diferencia%potencia;

			if (debug)	cout<<diferencia<<" "<<cociente<<" "<<resto<<endl;

			potencia=potencia/2;

			/*	Este loop calcula la expresión binaria del resto expresada con kr+1 bits, y lo guarda en array auxiliar bitsToFile */
			for (int j=0;j<kr;j++){

					bitsToFile[bitsToFilePointer]=resto/potencia;
					if (debug) cout<<resto/potencia;

					bitsToFilePointer++;

					resto=resto%potencia;

					potencia=potencia/2;

				}



	}if (debug) cout<<endl;


}

int Coder::getRachaParams(Image &image, int prox, int anterior, int &interruption_){


	int largo=0;
	int interruption=0;
	bool igual=true;

	if ((prox%image.width)!=0){

	while ((igual=(anterior==image.image[prox+largo]))&&((prox+largo+1)%(image.width)!=0)){

		//if (debug) cout<<"anterior= "<<(anterior==image.image[prox+largo])<<endl;
		//if (debug) cout<<"findefila= "<<((prox+largo)%image.width!=0)<<endl;
		largo++;

	}

	if (debug) cout<<"(anterior==image.image[prox+largo]) "<<(anterior==image.image[prox+largo])<<endl;
	if (debug) cout<<"((prox+largo)%(image.width)==0) "<<((prox+largo)%(image.width)==0)<<endl;

	if ((igual=(anterior==image.image[prox+largo]))&&((prox+largo+1)%(image.width)==0)) {

		interruption=1;
		largo=image.width-(prox%image.width);

	}
	}else{

		while ((igual=(anterior==image.image[prox+largo]))&&(largo<image.width)){

				largo++;

			}
		if (largo==image.width)interruption=1;

	}

	interruption_=interruption;

	return largo;
}

int Coder::fixPrediction(int predicted, int contexto){

	predicted=predicted+contexts[contexto].C;


	return predicted;
}

void Coder::updateContexto(int contexto, int error){


	/** Actualiza los datos N y A del contexto */

	/** Actualiza B y C */

	if (contexts[contexto].N==Nmax){

		/* si el valor de N para ese contexto es igual a Nmax divide N y A entre 2 */
		contexts[contexto].N=contexts[contexto].N/2;
		contexts[contexto].N_=contexts[contexto].N_/2;
		contexts[contexto].A=floor((double)contexts[contexto].A/(double)2);

		contexts[contexto].B=floor((double)contexts[contexto].B/(double)2);

	}
	/* Actualiza A sumándole el valor absoluto de este error */
	contexts[contexto].B=contexts[contexto].B+error;

	contexts[contexto].A=contexts[contexto].A+abs(error);

	contexts[contexto].N++;	//actualiza N

	if (error<0) contexts[contexto].N_++;

	if (contexts[contexto].B<=-contexts[contexto].N){

		contexts[contexto].B=contexts[contexto].B+contexts[contexto].N;

		if (contexts[contexto].C>-128) contexts[contexto].C=contexts[contexto].C-1;
		if (contexts[contexto].B<=-contexts[contexto].N) contexts[contexto].B=-contexts[contexto].N+1;



	}

	else if (contexts[contexto].B>0){


		contexts[contexto].B=contexts[contexto].B-contexts[contexto].N;

				if (contexts[contexto].C<127) contexts[contexto].C=contexts[contexto].C+1;
				if (contexts[contexto].B>0) contexts[contexto].B=0;



	}
/*
	if (!(contexts[contexto].C==0)){

		cout<<"B: "<<contexts[contexto].B<<endl;
		cout<<"C: "<<contexts[contexto].C<<endl;

	}
*/
}

void Coder::flushEncoder(ofstream &salida){

	/** Completa con ceros para poder escribir los últimos bits */

	if (bitsToFilePointer>0) {

		bitset<8> temp_b;

		for(int j=0;j<bitsToFilePointer;j++){

			temp_b[7-j]=bitsToFile[j];

			}//for j

			char temp=(char)temp_b.to_ulong();

			salida.write(&temp, 1);

	}
}

void Coder::encode_(int error, int k, ofstream &salida){

		/** Almacena en potencia el valor de 2^k
		Calcula la parte entera del cociente entre el error y 2^k y lo guarda en "cociente" para codificación binaria
		Calcula el resto de la división entera entre el error y 2^k y lo guarda en "resto" para codificación unaria */

	if (k!=1000){

	int potencia = 1;

	for (int j=0;j<k;j++){

		potencia=potencia*2;
	}

	int cociente=error/potencia;
	int resto=error%potencia;

	//cout<<cociente<<endl;

	potencia=potencia/2;

	/*	Este loop calcula la expresión binaria del resto expresada con k bits, y lo guarda en array auxiliar bitsToFile */
	for (int j=0;j<k;j++){

			bitsToFile[bitsToFilePointer]=resto/potencia;
			if (debug )cout<<resto/potencia;

			bitsToFilePointer++;

			resto=resto%potencia;

			potencia=potencia/2;

		}
	/* Este loop calcula la expresión unaria del cociente, con tantos ceros como la variable "cociente"
	y lo guarda en array auxiliar bitsToFile */
	/*for (int j=0;j<cociente;j++){

		bitsToFile[bitsToFilePointer]=0;
		//cout<<0;

		bitsToFilePointer++;

	}*/

	}
	writeCode(salida);

}

void Coder::encode(int error, int k, ofstream &salida){

		/** Almacena en potencia el valor de 2^k
		Calcula la parte entera del cociente entre el error y 2^k y lo guarda en "cociente" para codificación binaria
		Calcula el resto de la división entera entre el error y 2^k y lo guarda en "resto" para codificación unaria */

	if (k!=1000){

	int potencia = floor(pow(2,k));
	if (potencia==0)	potencia=1;

	int cociente=error/potencia;
	int resto=error%potencia;

	potencia=potencia/2;

	if (cociente>=qMax)	cociente=qMax;
	if (debug) cout<<"cociente=qMax: "<<(cociente==qMax)<<endl;

	for (int j=0;j<cociente;j++){

			bitsToFile[bitsToFilePointer]=0;
			if (debug) cout<<0;

			bitsToFilePointer++;

		}
		/*	para indicar el fin del código de la parte unaria escribe un 1 al final */
		bitsToFile[bitsToFilePointer]=1;
		if (debug)cout<<1;

		bitsToFilePointer++;

	if (cociente==qMax)	{

		cociente=qMax;

		potencia=pow(2,beta-1);

		for (int j=0;j<8;j++){

					bitsToFile[bitsToFilePointer]=error/potencia;
					if (debug) cout<<error/potencia;

					bitsToFilePointer++;

					error=error%potencia;

					potencia=potencia/2;

				}
	}
	else{
	/*	Este loop calcula la expresión binaria del resto expresada con k bits, y lo guarda en array auxiliar bitsToFile */
	for (int j=0;j<k;j++){

			bitsToFile[bitsToFilePointer]=resto/potencia;
			if (debug) cout<<resto/potencia;

			bitsToFilePointer++;

			resto=resto%potencia;

			potencia=potencia/2;

		}
	/* Este loop calcula la expresión unaria del cociente, con tantos ceros como la variable "cociente"
	y lo guarda en array auxiliar bitsToFile */

	}



	}if (debug)	cout<<endl;

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

			}//for j

			char temp=(char)temp_b.to_ulong();

			salida.write(&temp, 1);

			}//for k

			/* Todos los bits de la cola que no pudieron escribirse,
			esto es, los últimos bitsToFilePointer%8 bits válidos, son corridos al principio de la fila,
			para ser los primeros en escribirse cuando vuelva a invocarse este método */
			for(int k=8*(bitsToFilePointer/8);k<bitsToFilePointer;k++){

				bitsToFile[k-8*(bitsToFilePointer/8)]=bitsToFile[k];

			}//for k

			/* Se actualiza el valor del puntero */
			bitsToFilePointer=bitsToFilePointer%8;

			}//if

}


int Coder::rice(int error){

	/** Mapeo de rice del error */

	/** falta reducción de rango para el error ***/

	int uno =1;

	if (error>=0)uno=0;

	return (2*abs(error)-uno);
}

int Coder::getK(int contexto){

	/** Calcula k según la expresión de las diapositivas del curso */

	double AdivN_=(double)contexts[contexto].A/(double)contexts[contexto].N_;

	return round(log2(AdivN_));
}

int Coder::getPredictedValue(pixels pxls){

	/** Calcula el valor predicho según expresión de las diapositivas del curso */

	if ((pxls.c>=pxls.a)&&(pxls.c>=pxls.b)){

		if (pxls.a>pxls.b)
				return pxls.b;
		else return pxls.a;

	}else if ((pxls.c<=pxls.a)&&(pxls.c<=pxls.b)){

		if (pxls.a>pxls.b)
				return pxls.a;
		else return pxls.b;

	}else return (pxls.a+pxls.b-pxls.c);



}

int Coder::getContext(grad gradients){

	/** Determina el contexto
	Todos los contextos posibles se organizan en un array, donde cada elemento del array representa un contexto,
	es posible definir un mapeo entre el espacio de todos los contextos posibles y los enteros,
	para que dado un contexto haya una relación biunívoca con un elemento del array */

	int contga, contgb,contgc;

	if (gradients.ga<-21) contga=0;
	else if (gradients.ga<-7) contga=1;
	else if (gradients.ga<-3) contga=2;
	else if (gradients.ga<0) contga=3;
	else if (gradients.ga==0) contga=4;
	else if (gradients.ga<=3) contga=5;
	else if (gradients.ga<=7) contga=6;
	else if (gradients.ga<=21) contga=7;
	else contga=8;

	if (gradients.gb<-21) contgb=0;
		else if (gradients.gb<-7) contgb=1;
		else if (gradients.gb<-3) contgb=2;
		else if (gradients.gb<0) contgb=3;
		else if (gradients.gb==0) contgb=4;
		else if (gradients.gb<=3) contgb=5;
		else if (gradients.gb<=7) contgb=6;
		else if (gradients.gb<=21) contgb=7;
		else contgb=8;

	if (gradients.gc<-21) contgc=0;
		else if (gradients.gc<-7) contgc=1;
		else if (gradients.gc<-3) contgc=2;
		else if (gradients.gc<0) contgc=3;
		else if (gradients.gc==0) contgc=4;
		else if (gradients.gc<=3) contgc=5;
		else if (gradients.gc<=7) contgc=6;
		else if (gradients.gc<=21) contgc=7;
		else contgc=8;

	//mapeo elegido para representar los contextos

	return (9*9*contga)+(9*contgb)+(contgc);
}

void Coder::setContextsArray(){

	/** Forma el array con todos los contextos posibles */

	int indice=0;

	for (int k=-4;k<5;k++){

		for (int j=-4;j<5;j++){

			for (int i=-4;i<5;i++){

					Context contexto(k,j,i);
					contexts[indice]=contexto;
					indice++;

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
/**
int Coder::getP(pixels pxls){

	/** Devuelve el valor de p, según expresión de las diapositivas del curso

	return floor((double)(2*pxls.a+2*pxls.b+2*pxls.c+3)/(double)6);

}
*/
Coder::pixels Coder::getPixels(int current){

	/** Devuelve los píxeles de la vecindad: a, b y c */

	int a=-1;
	int b=-1;
	int c=-1;
	int d=-1;

	if ((current%image.width)==0){

		/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
		o la mitad del valor de blanco de la imagen */
		a=ceil((double)image.white/(double)2);
		c=ceil((double)image.white/(double)2);


	}

	if ((current%image.width)==image.width-1){

		/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
		o la mitad del valor de blanco de la imagen */
		d=ceil((double)image.white/(double)2);


	}

	if (current<image.width){

		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */
		if (b==-1) b=ceil((double)image.white/(double)2);
		if (c==-1) c=ceil((double)image.white/(double)2);
		if (d==-1) d=ceil((double)image.white/(double)2);
	}

	/* Para cada a, b,c y d, si no se cumple una condición de borde, y por lo tanto no hubo asignación en los if que preceden,
	se traen los valores de a, b,c y d de la imagen */
	if (a==-1) a=image.image[current-1];
	if (b==-1) b=image.image[current-image.width];
	if (c==-1) c=image.image[current-image.width-1];
	if (d==-1) d=image.image[current-image.width+1];

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
	writeWidth(salida);
	writeHeigth(salida);
	writeWhite(salida);
	writeNmax(salida);

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

void Coder::writeWidth(ofstream &salida){

	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int ancho =image.width;

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
void Coder::writeHeigth(ofstream &salida){

	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int alto =image.heigth;

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
void Coder::writeWhite(ofstream &salida){

	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int blanco =image.white;

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



Coder::~Coder() {

}

} /* namespace std */
