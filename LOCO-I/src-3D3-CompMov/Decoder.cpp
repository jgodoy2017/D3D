 /**
  @file Decoder.cpp
  @brief Realiza el trabajo de descomprimir la imagen

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#include "Decoder.h"
#include "ContextRun.h"
#include "math.h"
#include <sstream>

namespace std {

Decoder::~Decoder() {
	// TODO Auto-generated destructor stub
}

Decoder::Decoder(CodedImage &ci, bool vector) {

	this->codedImage=ci;
	
	string tipo = (vector ? "_vector_" : "_decoded_");
	this->file = ci.path + ci.name + tipo;

	ancho  = (vector ? ci.v_width  : ci.width);
	alto   = (vector ? ci.v_heigth : ci.heigth);
	blanco = (vector ? ci.v_white  : ci.white);

	Nmax=ci.Nmax;

	if (2>ceil(log2(blanco+1))) this->beta=2;
	else this->beta=ceil(log2(blanco+1));

	if (2>ceil(log2(blanco+1))) {
		this->Lmax=2*(2+8);
	}
	else {
		if (8>ceil(log2(blanco+1))) {
			this->Lmax=2*(ceil(log2(blanco+1))+8);
		}
		else{
			this->Lmax=2*(ceil(log2(blanco+1))+ceil(log2(blanco+1)));
		}
	}

	this->qMax=Lmax-beta-1;
	this->qMax_=Lmax-beta-1;
	this->range=blanco+1;
	this->cantidad_imagenes=(vector ? 2 : ci.cantidad_imagenes);
	this->activarCompMov=ci.activarCompMov;
	this->images = new Image[this->cantidad_imagenes];
}


void Decoder::decode(bool vector, Image &previa, int imgActual){
	
		if(primeraImagen){
			setContextsArray();
			prev=setInitialImage();
		}else{
			prev = previa;			
		}

		cout << "// START DECODER" << endl;
		if (vector) cout << "decode(): Modo VECTORES" << endl; else cout << "decode(): Modo IMAGEN" << endl;
		
		cout << "decode(): 1 fileToBits: ";
		for(int i=codedImage.fileToBitsPointer-25; i<codedImage.fileToBitsPointer+40; i++){
			if(i==codedImage.fileToBitsPointer) cout << " >";
			cout << codedImage.fileToBits[i];
			if(i==codedImage.fileToBitsPointer) cout << "< ";
		}
		cout << endl << "decode(): 1 fileToBitsPointer: " << codedImage.fileToBitsPointer << endl;
		
		
		cout << "decode(): cant_imagenes_decoder: "<< cantidad_imagenes<<endl;
		cout << "decode(): 1 CodedImage::codedImagePointer = " << CodedImage::codedImagePointer << endl;
		cout << "decode(): actual: " << imgActual << " final: " << imgActual + cantidad_imagenes - 1 << endl;;
		cout << "decode(): alto = " << alto << " ancho = " << ancho << " blanco = " << blanco << endl;

		for (int imagen=imgActual; imagen < imgActual + 1; imagen++){
			if (vector) cout << "decode(): imagen vector: " << imagen << endl; else cout << "decode(): imagen deco: " << imagen << endl;

			int contadorH=1,contadorW=1,contador=0;

			ofstream salida;

			string nombre = file + str_(CodedImage::codedImagePointer) + "_" + str_(imagen);
			salida.open(nombre.c_str(), ios::binary);
			cout << "decode(): nombre = " << nombre << endl;

			if(!vector) writeHeader(salida);	//escribe encabezado en el archivo de salida

			Image image(alto,ancho);
			image.white=blanco;
			
			while (contadorH < alto + 1){
					contadorW=1;
					
					while (contadorW < ancho + 1){// había un !=
						int signo;
						bool esRacha;
						//cout << alto << " " << ancho << contador << endl;
//						cout <<"puntero: " <<CodedImage::codedImagePointer<<endl;

						int prox_image_anterior=getProxImageAnterior(contador,vector);
						pixels3D pxls = getPixels3D(prox_image_anterior,contador,image);

						if (debug) cout<<contador<<" "  << pxls.a<<" "<< pxls.b<<" "<< pxls.c<<" "<< pxls.d<<endl;
						grad gradients = getGradients3D(1,pxls);

						int contexto = getContext(getGradients3D(0,pxls), getGradients3D(4,pxls), signo, esRacha);
						if (debug) cout<<"signo: "<<signo<<endl;
						if (debug) cout<<contexto<<endl;
						if (contador > ancho*alto ) cout<<contador<<"#########"<<contadorW<<" "<<contadorH<<endl;
						if (!esRacha){
							int predicted = getPredictedValue(selectMED(gradients),pxls);	//calcula el valor pixel predicho
							if (debug) cout<<predicted<<endl;

							predicted=fixPrediction(predicted,signo, contexto);
							if (debug) cout<<predicted<<endl;

							int k= getK(contexto);	//calcula k
							int error_=getError(k,0,0);	//lee el archivo para tener el valor del error codificado
							int error=unRice(error_,get_s(contexto),k);	//deshace el mapeo de rice para recuperar el error real
							error=reduccionDeRango(error,signo,predicted);

							if (debug) cout<<"error= "<<error<<endl;
							if (debug) cout<<"k= "<<k<<endl;
							if (debug) cout<<"error_= "<<error_<<endl;

							int pixel=predicted+error;	//calcula el pixel como la suma entre el predicho y el error
							updateImage(pixel,contador,image);	//va formando el array que representa la imagen con cada pixel decodificado

							char pixel_ =pixel+'\0';
							salida.write(&pixel_,1);	//escribe el pixel en el archivo
							updateContexto(contexto,error*signo);	//actualiza A y N del contexto
						}else{
							int interruption=0;
							int cantidad_unos=0;

							int largo=getRachaParams2(contadorW, interruption,cantidad_unos);
							int contexto=getContext_(contador, largo,image);

							Racha racha(largo, interruption, pxls.a,contexto);
							//cout<<contador<<" "<<largo<<" "<<interruption<<" "<<pxls.a<<" "<<contexto<<endl;

							updateImageRacha(racha, contador, salida,image);
							if(contador+largo<alto*ancho) updateImageInterruption(racha, contador, contador+largo, salida, cantidad_unos, image);

							contadorW=contadorW+largo;
							contador=contador+largo;

							if (racha.interruption)	{
								contadorW--;
								contador--;
							}

							if (debug) cout<<contadorW<<" "<<contadorH<<endl;
							if (debug) cout<<endl;
						}
					contador++;
					contadorW++;
				}
				contadorH++;
			}
		
			salida.close();
//			images[imagen]=image;

			previa=image;
			
		
			if(vector && imgActual == 0){
				for(int i=0; i<ancho*alto; i++) {
				codedImage.vector_ancho[i] = previa.image[i];
				}
			}
			if(vector && imgActual == 1){
				for(int i=0; i<ancho*alto; i++) {
				codedImage.vector_alto[i] = previa.image[i];
				}
			}

			cout << "-----------" << endl;
			for(int i=0; i<26*26; i++) cout << codedImage.vector_ancho[i]  << " ";
			cout << endl << endl;
			for(int i=0; i<26*26; i++) cout << codedImage.vector_alto[i]  << " ";
			cout << endl;
			cout << "-----------" << endl;
//			if(!vector) cout << "decode(): Ultimo: " << (previa.image[0]+'\0') << endl;
			cout << "decode(): 2 fileToBits: ";
			for(int i=codedImage.fileToBitsPointer-25; i<codedImage.fileToBitsPointer+40; i++){
				if(i==codedImage.fileToBitsPointer) cout << " >";
				cout << codedImage.fileToBits[i];
				if(i==codedImage.fileToBitsPointer) cout << "< ";
			}
			cout << endl << "decode(): 2 fileToBitsPointer: " << codedImage.fileToBitsPointer << endl;

			if(primeraImagen) primeraImagen=false;
		
			cout << "decode(): 2 CodedImage::codedImagePointer = " << CodedImage::codedImagePointer << endl;
			
//			cout << "decode(): codedImage.image: " << endl;
//			for(int i=0; i<50; i++) cout << (codedImage.image[i]+'\0') << " ";
//			cout << endl;
	}
	
	cout << "// END DECODER." << endl;
}

string Decoder::str_(int n){


	stringstream ss1;
	ss1 << n;
	string n_ = ss1.str();

	return n_;
}

int Decoder::getProxImageAnterior(int prox, bool vector){

	int proxAnt=prox;
	
	if (activarCompMov && !vector){
 		int bloqueV = (prox / ancho) / bsize;
 	 	int bloqueH = (prox % ancho) / bsize;

		int ind = bloqueH + bloqueV * (1 + ancho / bsize);
//		proxAnt = min((prox + codedImage.vector_ancho[ind] + codedImage.vector_alto[ind] * ancho), ancho * alto - 1);
		proxAnt = prox + codedImage.vector_ancho[ind] + codedImage.vector_alto[ind] * ancho;
	}
	
	return proxAnt;
}

Decoder::pixels3D Decoder::getPixels3D(int current, int current2, Image &image2){

	/** Devuelve los píxeles de la vecindad: a, b, c, d, a_, b_, c_, d_, e_, f_ y g_ */


	/**  arreglar criterio para los píxeles que caen fuera de la imagen*/

	Image image1=prev;

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
	//	a=ceil((double)image2.white/(double)2);
	//	c=ceil((double)image2.white/(double)2);
		
		a = 1 + (image2.white >> 1);
		c = 1 + (image2.white >> 1);
	}

	if ((current2%image2.width)==image1.width-1){

		/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
		o la mitad del valor de blanco de la imagen */
	//	d=ceil((double)image2.white/(double)2);
		
		d = 1 + (image2.white >> 1);
		
	}

	if (current2<image2.width){

		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */
//		if (b==-1) b=ceil((double)image2.white/(double)2);
//		if (c==-1) c=ceil((double)image2.white/(double)2);
//		if (d==-1) d=ceil((double)image2.white/(double)2);
		
		if(b == -1) b = 1 + (image2.white >> 1);
		if(c == -1) c = 1 + (image2.white >> 1);
		if(d == -1) d = 1 + (image2.white >> 1);
	}

	if ((current%image1.width)==0){

			/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
			o la mitad del valor de blanco de la imagen */
//			a_=ceil((double)image1.white/(double)2);
//			c_=ceil((double)image1.white/(double)2);
			
			a_ = 1 + (image1.white >> 1);
			c_ = 1 + (image1.white >> 1);
		}

		if ((current%image1.width)==image1.width-1){

			/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
			o la mitad del valor de blanco de la imagen */
//			d_=ceil((double)image1.white/(double)2);
//			f_=ceil((double)image1.white/(double)2);
			
			d_ = 1 + (image1.white >> 1);
			f_ = 1 + (image1.white >> 1);
		}

		if (current<image1.width){

			/* Si estoy en la primer fila, b y c deben ser "128"
			o la mitad del valor de blanco de la imagen */
//			if (b_==-1) b_=ceil((double)image1.white/(double)2);
//			if (c_==-1) c_=ceil((double)image1.white/(double)2);
//			if (d_==-1) d_=ceil((double)image1.white/(double)2);
			
			if(b_ == -1) b_ = 1 + (image1.white >> 1);
			if(c_ == -1) c_ = 1 + (image1.white >> 1);
			if(d_ == -1) d_ = 1 + (image1.white >> 1);
		}

		if (current>(image1.heigth-2)*image1.width){

			/* Si estoy en la última o penúltima fila, g debe ser "128"
			o la mitad del valor de blanco de la imagen */
//			g_=ceil((double)image1.white/(double)2);
			
			g_ = 1 + (image1.white >> 1);
		}

	/* Para cada a, b,c y d, si no se cumple una condición de borde, y por lo tanto no hubo asignación en los if que preceden,
	se traen los valores de a, b,c y d de la imagen */
	if (a==-1)  a=image2.image[current2-1];
	if (b==-1)  b=image2.image[current2-image1.width];
	if (c==-1)  c=image2.image[current2-image1.width-1];
	if (d==-1)  d=image2.image[current2-image1.width+1];
	if (a_==-1) a_=image1.image[current-1];
	if (b_==-1) b_=image1.image[current-image1.width];
	if (c_==-1) c_=image1.image[current-image1.width-1];
	if (d_==-1) d_=image1.image[current-image1.width+1];
	if (e_==-1) e_=image1.image[current];
	if (f_==-1) f_=image1.image[current+1];
	if (g_==-1) g_=image1.image[current+image1.width];
	
//	pixels3D pxls={a,b,c,d,a_,b_,c_,d_,e_,f_,g_};

		return {a,b,c,d,a_,b_,c_,d_,e_,f_,g_};
}

Image Decoder::setInitialImage(){

Image aux=Image();

aux.image=(int*)malloc(ancho*alto*sizeof(int));

for (int k=0;k<ancho*alto;k++)
	aux.image[k]=0;
	/*	definir algún criterio para esta imagen */

aux.white=this->blanco;
aux.width=this->ancho;
aux.heigth=this->alto;

return aux;

}

float Decoder::get_s(int contexto){


	return float(float(contexts[contexto].B)/float(contexts[contexto].N)); //es N o N_?
}

int Decoder::reduccionDeRango(int error, int signo,int predicted){

	//IF(J3<0,J3+$A$3,IF(J3>$A$3,J3-$A$3,J3))
	error=error*signo;

	if ((error+predicted)<0){

		error=error+range;
	}else if ((error+predicted)>range-1){

		error=error-range;
	}


	if (debug) cout<<"return error"<<endl;

	return error;


	/*
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
*/
	return error;
}

int Decoder::getKPrime(Racha &r){
	int T_racha, K_racha;

		T_racha=cntx[r.contexto].A_racha+(cntx[r.contexto].N_racha>>1)*r.contexto;
		if (debug) cout<<"T_racha: "<<T_racha<<endl;
		for(K_racha=0; (cntx[r.contexto].N_racha<<K_racha)<T_racha; K_racha++);

	
	return K_racha;
}
int Decoder::unrice_rachas(int error,int contexto, int k){

	int map=0;

	int retorno=(error+contexto+1)>> 1;

	if ((error+contexto+1)%2==0) map=1;

	if ((k==0)and(map==1)and(2*cntx[contexto].Nn_racha<cntx[contexto].N_racha)){

	}else if ((map==1)and(2*cntx[contexto].Nn_racha>=cntx[contexto].N_racha)){

		retorno=-retorno;
	}else if ((map==1)and(k!=0)){

		retorno=-retorno;
	}else{

		if ((k!=0)or(2*cntx[contexto].Nn_racha>=cntx[contexto].N_racha)){


		}else retorno=-retorno;


	}

	 return retorno;

}



void Decoder::updateImageInterruption(Racha &racha, int contador,int prox_, ofstream &salida,int cantidad_unos, Image &image){

	if (!racha.interruption){

		int signo=1;

		if (racha.contexto==0) signo=-1;

	int kPrime=getKPrime(racha);
	int error_=getError(kPrime,1,cantidad_unos);


	//int error = unRice(error_,0,1);

	int error = unrice_rachas(error_,racha.contexto,kPrime);

	error=reduccionDeRango(error*signo,1,getPixels3D(0,prox_, image).b);
	int errorEstadisticos=clipErrorEstadisticos(error);

	if (debug) cout<<"error: "<<error_<<" "<<error<<" "<<unrice_rachas(error_,racha.contexto,kPrime)<<endl;

	image.image[contador+racha.largo]=getPixels3D(0,prox_, image).b+error;

	char pixel_ =getPixels3D(0,prox_, image).b+error+'\0';

	salida.write(&pixel_,1);	//escribe el pixel en el archivo
	updateContexto_(racha.contexto, unrice_rachas(error_,racha.contexto,kPrime),error_);

	if (debug) cout<<"actual: "<<image.image[contador+racha.largo]<<endl;

	}

}

void Decoder::updateImageRacha(Racha &racha, int contador, ofstream &salida, Image &image){

	for (int k=0;k<racha.largo;k++){
		image.image[contador+k]=racha.pixel;

		char pixel_ =racha.pixel+'\0';

		salida.write(&pixel_,1);	//escribe el pixel en el archivo
	}


}

int Decoder::getRachaParams2(int contadorW, int &interruption_, int &cantidad_unos){

	if (debug4) cout<<" Decodificación de la racha: "<<endl;



	int largo=0;
	int bit;

	int ajuste=0;

	interruption_=1;

	bool finDeRacha=false;


	while (true){

		bit=codedImage.getBit();

		if (bit==1){


			if ((1 << J[RUNindex])>(-largo+ancho-(contadorW-1))){

				largo=ancho-(contadorW-1);

				if (debug) cout<<"1- largo: "<<largo<<endl;



						}

						else {
							largo=largo + (1 << J[RUNindex]);
							if (RUNindex<31) RUNindex++;

							if (debug) cout<<"2- largo: "<<largo<<endl;
						}

			if (largo==ancho-(contadorW-1)) break;

		}else{

			break;
		}
	}
		if (bit==0)

			{if (debug4) cout<<endl;
						if (debug4) cout<<" Codificación largo faltante: "<<endl;

									interruption_=0;

									for (int j=0;j<J[RUNindex];j++){

										largo=largo+(pow(2,J[RUNindex]-j-1))*codedImage.getBit();


									}
									if (debug4) cout<<endl;

									ajuste = J[RUNindex];
									if (RUNindex>0) RUNindex--;


									finDeRacha=true;

		}


	cantidad_unos=ajuste+1;

	if (debug) cout<<"RUNindex: "<<RUNindex<<endl;

	return largo;

}
int Decoder::getRachaParams(int contadorW, int &interruption_, int &cantidad_unos){

	//kr=0;

	int bit=1;

	int largo=0, interruption;


	bool finDeFila=false;

	cantidad_unos=0;



	while ((!finDeFila)&&(bit=codedImage.getBit())){


		cantidad_unos++;

		m_r=pow(2,J[RUNindex]);


		largo=largo+m_r;

		if (debug) cout<<"RUNindex= "<<RUNindex<<endl;

		finDeFila=(largo+contadorW-1>=codedImage.width); //mayor o mayor o igual?

		if ((largo+contadorW-1<=codedImage.width)and(RUNindex<31)){

			RUNindex++;

		}
	}

	interruption=bit;

	if ((largo+contadorW-1>codedImage.width)) cantidad_unos--;

	RUNindex--;





	if (bit)	largo=codedImage.width-contadorW+1;
	else {

		int pot;

		if (debug) cout<<"ok"<<kr;

		for (int j=kr;j>=0;j--){

			pot=(pow(2,j))*codedImage.getBit();

			largo=largo+pot;

		}

	}if (debug) cout<<endl;

	kr++;

	cantidad_unos=J[RUNindex]+1;
	if (debug) cout<<"J[RUNindex]+1= "<<cantidad_unos<<endl;

	interruption_=interruption;

	return largo;
}

void Decoder::updateImage(int pixel, int contador, Image &image){

	/** Agrega el pixel decodificado al array que representa la imagen */

	image.image[contador]=pixel;

}
int Decoder::unRice(int error,float s, int k){

	/** Inverso de mapeo de rice */

	if ((k<=0)and(s<=-0.5)){

			if (error%2==0)	{
				int aux=error/2;
				return -aux-1;
			}
			else{

				int aux=((error+1)/(-2));
				return -aux-1;
			}
		}

	else{

	if (error%2==0)	return error/2;
	else	return ((error+1)/(-2));
	}
}


int Decoder::getError_(int k){

	/** Devuelve como entero el error codificado */

	int error=0;
	int potencia=pow(2,k);

	int bit=0;

	/* Convierte los siguientes k bits de fileToBits en un entero,
	que corresponden a la parte binaria del error */
	for (int j=0;j<k;j++){

		bit=codedImage.getBit();

		potencia=potencia/2;
			error=error+bit*potencia;

	}


		return error;
}

int Decoder::getError(int k, int racha, int ajuste){

	/** Devuelve como entero el error codificado */

	int qMax;



	if (racha) {

		qMax=this->qMax_;
		if ((debug4)and(racha)) cout<<" qMax: "<<qMax<<endl;
		qMax=qMax-ajuste;
		if ((debug4)and(racha)) cout<<" qMax: "<<qMax<<endl;

	}
	else {

		qMax=this->qMax;
	}





	int error=0;
	int potencia=floor(pow(2,k));
	//if (potencia==0) potencia=1;

	int bit=0;

	int contador=0;

	/* Obtiene la cantidad de ceros que le siguen antes del primer uno,
		es la codificación unaria del cociente entre el error y 2^k */


	if ((debug4)and(racha)) cout<<" Codificación muestra de interupción: "<<endl;
	if ((debug4)and(racha)) cout<<" Parte unaria: "<<endl;

	while ((contador!=qMax)&&codedImage.getBit()!=1){
	//while (codedImage.getBit()!=1){
			contador++;

		}

	if (debug) cout<<endl;
	//if (debug) cout<<"contador=qMax: "<<(contador==qMax)<<endl;

	if (contador!=qMax){

	/* Convierte los siguientes k bits de fileToBits en un entero,
	que corresponden a la parte binaria del error */
		if ((debug4)and(racha)) cout<<" Parte binaria: "<<endl;

	for (int j=0;j<k;j++){



		bit=codedImage.getBit();

		potencia=potencia/2;
			error=error+bit*potencia;

	}
	if ((debug4)and(racha)) cout<<endl;
	}
	else{

		if ((debug4)and(racha)) cout<<" Byte por código de escape: "<<endl;

		potencia=pow(2,beta);

		for (int j=0;j<beta;j++){

				bit=codedImage.getBit();

				potencia=potencia/2;
					error=error+bit*potencia;

			}
		if ((debug4)and(racha)) cout<<endl;

	}

	int pot_aux=1;

	if (k>=0) pot_aux=pow(2,k); //cuando k negativo pow(2,k) es 0, y necesitamos que sea 1


		/* Sumando los dos valores decodificados (cociente y resto entre 2^k) resulta el mapeo de rice
		del error codificado */
		if (contador!=qMax) error=error+contador*pot_aux;

		if (debug) cout<<endl;



		return error;
}

void Decoder::writeHeader(ofstream &salida){

	writeMagic(salida);
	writeWidth(salida);
	writeHeigth(salida);
	writeWhite(salida);

}

void Decoder::writeMagic(ofstream &salida){
	char temp='P';
	salida.write(&temp,1);

	temp='5';
	salida.write(&temp,1);

	temp='\n';
	salida.write(&temp,1);
}

void Decoder::writeWidth(ofstream &salida){

	int ancho_ = codedImage.width;

	double aux=(double)ancho_;

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
												//anterior, supongo que si bien uno lo ve como el número n
												//para la máquina es (n-1),9999999999
	else temp=floor(aux);

	temp_=temp+'0';

	salida.write(&temp_,1);

	potencia=potencia/10;
	}

	temp_=' ';

	salida.write(&temp_,1);
}
void Decoder::writeHeigth(ofstream &salida){
	int alto_=codedImage.heigth;

	double aux=(double)alto_;

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
void Decoder::writeWhite(ofstream &salida){

	int blanco_=codedImage.white;

	double aux=(double)blanco_;

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
													//anterior, suuponemos que si bien uno lo ve como el número n
													//para la máquina es (n-1),9999999999
		else temp=floor(aux);

	temp_=temp+'0';

	salida.write(&temp_,1);

	potencia=potencia/10;
	}

	temp_='\n';

	salida.write(&temp_,1);

}

int Decoder::fixPrediction(int predicted,int signo, int contexto){

	predicted=predicted+(contexts[contexto].C*signo);

	if (predicted< 0) predicted=0;
	if (predicted >range-1) predicted=range-1;

	return predicted;
}

void Decoder::updateContexto(int contexto, int error){

//	cout << "updateContexto(): Contexto: " << contexto << " N: " << contexts[contexto].N << " Nn: "<<contexts[contexto].N_<<" A: "<<contexts[contexto].A<<" B: "<<contexts[contexto].B<<" C: "<<contexts[contexto].C<<endl;
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



		}

		else if (contexts[contexto].B>0){


			contexts[contexto].B=contexts[contexto].B-contexts[contexto].N;

					if (contexts[contexto].C<127) contexts[contexto].C=contexts[contexto].C+1;
					if (contexts[contexto].B>0) contexts[contexto].B=0;



		}
		//printf("> A: %3d B: %3d C: %3d N: %3d\n", contexts[contexto].A, contexts[contexto].B, contexts[contexto].C, contexts[contexto].N);
	/*
		if (!(contexts[contexto].C==0)){

			cout<<"B: "<<contexts[contexto].B<<endl;
			cout<<"C: "<<contexts[contexto].C<<endl;

		}
	*/
}


int Decoder::getK(int contexto){

	double AdivN_=(double)contexts[contexto].A/(double)contexts[contexto].N;

	return round(log2(AdivN_));
}

int Decoder::getPredictedValue(pixels pxls){

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

int Decoder::selectMED(grad gradients){
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

Decoder::grad Decoder::getGradients3D(int modo, pixels3D pxls){

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

int Decoder::getPredictedValue(int modo, pixels3D pxls){

	/** Calcula el valor predicho según expresión de las diapositivas del curso */
	int pred;
	int error;

	if (modo == 0){
		if ((pxls.c>=pxls.a)&&(pxls.c>=pxls.b)){
			if (pxls.a>pxls.b)
					pred = pxls.b;
			else pred = pxls.a;
		}else if ((pxls.c<=pxls.a)&&(pxls.c<=pxls.b)){
			if (pxls.a>pxls.b)
					pred = pxls.a;
			else pred = pxls.b;
		}else pred = (pxls.a+pxls.b-pxls.c);
		//error = pred - pixel;
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
			//error = pred - pixel;
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

		//error = pred - pixel;
	}
	return pred;
}

int Decoder::getContext(grad gradients1,grad gradients2, int &signo,bool &racha){

	/*Determina el contexto
	Todos los contextos posibles se organizan en un array, donde cada elemento del array representa un contexto,
	es posible definir un mapeo entre el espacio de todos los contextos posibles y los enteros,
	para que dado un contexto haya una relación biunívoca con un elemento del arra*/

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
		}else if((contga==4)and(contgb<4)) {
			contga=8-contga;
			contgb=8-contgb;
			contgc=8-contgc;
			contgd=8-contgd;
			contge=8-contge;

			signo=-1;
		}
		else if((contga==4)and(contgb==4)and(contgc<4)) {
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

		if ((9*9*9*9*contga)+(9*9*9*contgb)+(9*9*contgc)==29484)
				racha=true;
		else racha=false;


	return (9*9*9*9*contga)+(9*9*9*contgb)+(9*9*contgc)+(9*contgd)+contge;
}


void Decoder::setContextsArray(){

	/** Forma el array con todos los contextos posibles */

	int indice=0;

		for (int k=-4;k<5;k++){

			for (int j=-4;j<5;j++){

				for (int i=-4;i<5;i++){

					for (int l=-4;l<5;l++){

						for (int m=-4;m<5;m++){


						Context contexto(k,j,i,l,m,blanco);
						contexts[indice]=contexto;
						indice++;

						}

					}

				}

			}

		}
}

Decoder::grad Decoder::setGradients(pixels pxls){

	/** Dado p y los píxeles a, b, c y d de la vecindad,
	forma el vector de gradientes */

	grad gradients={pxls.d-pxls.b,pxls.b-pxls.c,pxls.c-pxls.a};

	return gradients;
}

int Decoder::getP(pixels pxls){

	return floor((double)(2*pxls.a+2*pxls.b+2*pxls.c+3)/(double)6);

}



int Decoder::getContext_(int pos, int lar, Image &image){
	return (getPixels3D(0,pos,image).a==getPixels3D(0,pos+lar,image).b);
}

void Decoder::updateContexto_(int c, int err,int err_){

	cntx[c].updateA((err_+1-c)>>1);
	cntx[c].updateNn(err);

	if(cntx[c].N_racha==RESET) {
			cntx[c].reset();
			//cout << "RESET" << endl;
		}



	cntx[c].updateN();

	//cout << "map: "<<map<< " k: "<<k<< " A: "<<cntx[c].A_racha<< " N: "<<cntx[c].N_racha<< " Nn: "<<cntx[c].Nn_racha << " error: "<< err<< " mapeo de Rice: "<< err_<< " contexto: "<< c<<endl;

}



int Decoder::clipErrorEstadisticos(int error){
	int errorEstadisticos=error;
	if(error>128) errorEstadisticos-=256;
	else if(error<-128) errorEstadisticos+=256;
	
	return errorEstadisticos;
}

} /* namespace std */
