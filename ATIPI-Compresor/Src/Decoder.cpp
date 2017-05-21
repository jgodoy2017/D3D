 /**
  @file Decoder.cpp
  @brief Realiza el trabajo de descomprimir la imagen

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#include "Decoder.h"
#include "math.h"

namespace std {

Decoder::Decoder(CodedImage codedImage) {

	//constructor

	this->file=codedImage.path+codedImage.name+"_decoded_.pgm";

		this->codedImage=codedImage;

		image.heigth=codedImage.heigth;

		image.width=codedImage.width;

		image.setImage();

		Nmax=codedImage.Nmax;


}

Decoder::~Decoder() {
	// TODO Auto-generated destructor stub
}

void Decoder::decode(){

	/** como puede verse el funcionamiento general del decodificador es bastante simétrico al codificador

	Por una descripción de los métodos en común con la clase Coder, recurrir a las descripciones disponibles en Coder.cpp */

int contadorH=1,contadorW=1,contador=0;
		ofstream salida;
		salida.open(file.c_str(), ios::binary);

		setContextsArray();

		writeHeader(salida);	//escribe encabezado en el archivo de salida

		codedImagePointer=0;

			while (contadorH!=codedImage.heigth+1){

				contadorW=1;

				while (contadorW!=codedImage.width+1){

				pixels pxls = getPixels(contador);	//trae el vecindario a, b y c de cada pixel a decodificar

				int p = getP(pxls);	//calcula p

				grad gradients=setGradients(p,pxls);	//calcula el vector de gradientes

				int contexto = getContext(gradients);	//trae el contexto que corresponde a estte pixel

				int predicted = getPredictedValue(pxls);	//calcula el valor predicho

				int k= getK(contexto);	//calcula k

				int error_=getError(k);	//lee el archivo para tener el valor del error codificado

				int error=unRice(error_);	//deshace el mapeo de rice para recuperar el error real

				int pixel=predicted+error;	//calcula el pixel como la suma entre el predicho y el error

				updateImage(pixel,contador);	//va formando el array que representa la imagen con cada pixel decodificado

				char pixel_ =pixel+'\0';

				salida.write(&pixel_,1);	//escribe el pixel en el archivo

				updateContexto(contexto,error);	//actualiza A y N del contexto

				contadorW++;contador++;
				}contadorH++;


			}
		salida.close();
}


void Decoder::updateImage(int pixel, int contador){

	/** Agrega el pixel decodificado al array que representa la imagen */

	image.image[contador]=pixel;

}
int Decoder::unRice(int error){

	/** Inverso de mapeo de rice */

	if (error%2==0)	return error/2;
	else	return ((error+1)/(-2));

}

void Decoder::completaArray(){

	/** Cuando se lee el último bit disponible del array, este método vuelve a completarlo. */

	char temp;

	int contador=0;

	while ((contador<100)&&((codedImagePointer<(codedImage.heigth*codedImage.width)))){ //hasta leer 100 bytes o que se termine la imagen

	temp=codedImage.image[codedImagePointer];
	codedImagePointer++;

	std::bitset<8> temp_b(temp);

				for(int j=0;j<8;j++){

				fileToBits[contador*8+j]=temp_b[7-j];

				}//for j

				contador++;
	}

}

int Decoder::getBit(){

	/** Esta función devuelve el próximo bit de fileToBits.
	Cuando llega al último elemento del array, vuelve a llenar el array con los valores de la imagen
	y empieza desde el lugar 0 */

	if (fileToBitsPointer==0){

		completaArray();

	}int retorno = fileToBits[fileToBitsPointer];
	fileToBitsPointer=((fileToBitsPointer+1)%800);//actualiza el puntero al array de manera circular
	return retorno;

}

int Decoder::getError(int k){

	/** Devuelve como entero el error codificado */

	int error=0;
	int potencia=pow(2,k);

	int bit=0;

	/* Convierte los siguientes k bits de fileToBits en un entero,
	que corresponden a la parte binaria del error */
	for (int j=0;j<k;j++){

		bit=getBit();

		potencia=potencia/2;
			error=error+bit*potencia;

	}
	int contador=0;

	/* Obtiene la cantidad de ceros que le siguen antes del primer uno,
	es la codificación unaria del cociente entre el error y 2^k */
	while (getBit()!=1){
		contador++;

	}

	int pot_aux=1;

	if (k>=0) pot_aux=pow(2,k); //cuando k negativo pow(2,k) es 0, y necesitamos que sea 1

		/* Sumando los dos valores decodificados (cociente y resto entre 2^k) resulta el mapeo de rice
		del error codificado */
		error=error+contador*pot_aux;

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

	int ancho =codedImage.width;

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
	int alto =codedImage.heigth;

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
void Decoder::writeWhite(ofstream &salida){

	int blanco =codedImage.white;

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

void Decoder::updateContexto(int contexto, int error){


	if (contexts[contexto].N==Nmax){

		contexts[contexto].N=contexts[contexto].N/2;
		contexts[contexto].A=floor((double)contexts[contexto].A/(double)2);

	}

	contexts[contexto].A=contexts[contexto].A+abs(error);

	contexts[contexto].N++;
}


int Decoder::getK(int contexto){

	double AdivN=(double)contexts[contexto].A/(double)contexts[contexto].N;

	return round(log2(AdivN));
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

int Decoder::getContext(grad gradients){

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

		if (gradients.gc<-3) contgc=0;
			else if (gradients.gc<0) contgc=1;
			else if (gradients.gc==0) contgc=2;
			else if (gradients.gc<=3) contgc=3;
			else contgc=4;

	return (5*9*contga)+(5*contgb)+(contgc);
}

void Decoder::setContextsArray(){

	int indice=0;

	for (int k=-4;k<5;k++){

		for (int j=-4;j<5;j++){

			for (int i=-2;i<3;i++){

					Context contexto(k,j,i);
					contexts[indice]=contexto;
					indice++;

			}

		}

	}

}

Decoder::grad Decoder::setGradients(int p,pixels pxls){

	grad gradients={pxls.a-p,pxls.b-p,pxls.c-p};

	return gradients;
}

int Decoder::getP(pixels pxls){

	return floor((double)(2*pxls.a+2*pxls.b+2*pxls.c+3)/(double)6);

}

Decoder::pixels Decoder::getPixels(int current){

	int a=-1;
	int b=-1;
	int c=-1;

	if ((current%codedImage.width)==0){

		a=ceil((double)codedImage.white/(double)2);
		c=ceil((double)codedImage.white/(double)2);

	}

	if (current<codedImage.width){

		if (b==-1) b=ceil((double)codedImage.white/(double)2);
		if (c==-1) c=ceil((double)codedImage.white/(double)2);
	}

	if (a==-1) a=image.image[current-1];
	if (b==-1) b=image.image[current-codedImage.width];
	if (c==-1) c=image.image[current-codedImage.width-1];

	pixels pxls={a,b,c};


		return pxls;
}


} /* namespace std */
