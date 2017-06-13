 /**
  @file Decoder.cpp
  @brief Realiza el trabajo de descomprimir la imagen

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#include "Decoder.h"
#include "Rachas.h"
#include "math.h"

using namespace std;

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

void Decoder::decodeRun(ofstream& salida){
	int i=0, h_pos, width, px_a, px_b, px_x, ri_type, T_racha, A_racha, N_racha, K_racha, Nn_racha,
	    prefijo, l_racha=0, l_resto, L_max, Q_max, Q_golomb=0, R_golomb, M_eps, eps_val, eps_sign, map, caso_map;
	unsigned char racha[10]={0xFF, 0xAB, 0xBF, 0xDA, 0xAA, 0xAF, 0x0F, 0xAA, 0x00, 0xFF};
//	unsigned char racha[10]={0xFF, 0xAF, 0xFF, 0xFF, 0x8A, 0xF0, 0xAF, 0xAA, 0x00, 0xFF};
	unsigned char racha_byte=0x00, resto[2]={0x00, 0x00};
	
	printf(">> Posicion horizontal del pixel inicial (a): ");
	scanf("%d", &h_pos);
	printf(">> Ancho de la imagen : ");
	scanf("%d", &width);
	printf(">> Valor del pixel superior (a): ");
	scanf("%d", &px_a);
	printf(">> Valor del pixel inicial (b): ");
	scanf("%d", &px_b);
	printf(">> Estadistico A: ");
	scanf("%d", &A_racha);
	printf(">> Estadistico N: ");
	scanf("%d", &N_racha);
	printf(">> Estadistico Nn: ");
	scanf("%d", &Nn_racha);
	printf("\n");
	
	printf(">> Tira de bits: ");
	for(int k=0; k<80; k++){
		PEEK_BIT(racha, k, racha_byte, 7);
		printf("%d", (racha_byte==0x01) ? 1 : 0);
		racha_byte=0x00;
	}
	printf("b\n");
	
	PEEK_BIT(racha, 0, racha_byte, 7);
	while((racha_byte==0x01) && (i<31)){   // Leo 1's de la tira de bits.
		racha_byte=0x00;
		if((h_pos+l_racha+(1<<J[i]))<=width){    // Siempre que haya espacio horizontal...
			l_racha+=(1<<J[i]);                  // ...aumento el largo de racha...
			i++;                                 // ...incremento la entrada en J...
			PEEK_BIT(racha, i, racha_byte, 7);   // ...y vuelvo a leer un bit.
		}
	}
	printf(">> Cantidad de 1's para el largo de racha: %d\n", i);
	printf(">> Parte unaria del largo de racha: %d\n", l_racha);
	
	if(h_pos+l_racha<width){
		printf(">> La racha se interrumpe por un pixel de otro color.\n");
		prefijo=0;
	}else{
		printf(">> La racha termina con la fila. Racha decodificada.\n");
		printf(">> Largo total de la racha: %d pixeles.\n", l_racha);
		prefijo=1;
	}
	
	if(!prefijo){
		racha_byte=0x00;
		PEEK_BIT(racha, i, racha_byte, 7);  // Leo el bit de prefijo.
		if((!racha_byte) && J[i]){          // Leo el resto correspondiente al largo de racha.
			printf(">> El bit de prefijo (%d) es correcto (0).\n", i+1);
			
			if(J[i]>8){  // Obtengo la representacion binaria del resto, que solamente puede tener 2 bytes...
				for(int k=0; k<8; k++)      PEEK_BIT(racha, i+1+k, resto[0], k);
				for(int k=0; k<J[i]-8; k++) PEEK_BIT(racha, i+9+k, resto[1], k+16-J[i]);
			}else{       // ...o 1 byte.
				for(int k=0; k<J[i]; k++)   PEEK_BIT(racha, i+1+k, resto[1], k+8-J[i]);
			}
			l_resto=(int)((1<<(J[i]-8))*(resto[0]&0xFF)+(resto[1]&0xFF));   // Obtengo el valor numerico del resto.
			l_racha+=l_resto;                                               // Calculo el largo total de la racha.
			printf(">> Resto del largo de la racha: %d representado con %d bits.\n", l_resto, J[i]);
			printf(">> Largo total de la racha: %d pixeles.\n\n", l_racha);
			
			ri_type=(px_a==px_b ? 1 : 0);  // Run Interrpution Type.
			
			T_racha=((ri_type==1) ? A_racha : A_racha + N_racha/2);   // Calculo la variable T para determinar k de Golomb.
			for(K_racha=0; (N_racha<<K_racha)<T_racha; K_racha++);    // k = min{k' / 2^(k') >= T}
			
			L_max=32-J[i];    // Largo maximo para el Golomb de largo limitado.                L_max = 32 - (kr + 1)	
			Q_max=L_max-9;    // Valor de q que decide si estamos leyendo un codigo de escape. q_max = (L_max - 1) - 8
			
			i+=1+J[i];  // Salteo el bit de prefijo y el resto, ya leidos.
			racha_byte=0x00;
			PEEK_BIT(racha, i, racha_byte, 7);
			while((racha_byte==0x01) && (Q_golomb<Q_max)){  // Leo la cantidad de 1's (Q_golomb) y determino si es Golomb o codigo de escape.
				Q_golomb++;
				racha_byte=0x00;
				PEEK_BIT(racha, i, racha_byte, 7);				
				i++;
			}
			i--;
			
			printf(">> A      = %d\n", A_racha);
			printf(">> N      = %d\n", N_racha);
			printf(">> Nn     = %d\n", Nn_racha);
			printf(">> RIType = %d\n", ri_type);
			printf(">> T      = %d\n", T_racha);
			printf(">> k      = %d\n", K_racha);
			printf(">> L      = %d\n", Q_golomb+1);
			printf(">> L_max  = %d\n", L_max);
			printf(">> q_max  = %d\n", Q_max);
			printf(">> m      = %d\n", 1<<K_racha);
			printf(">> q      = %d\n", Q_golomb);
			printf(">> c      = %d\n\n", K_racha);
			
			if(Q_golomb<Q_max){
				printf(">> El pixel de salida esta codificado en Golomb(2^%d).\n", K_racha);
				racha_byte=0x00;
				PEEK_BIT(racha, i, racha_byte, 7);  // Leo el bit de corte de la parte unaria (debe ser 0).
				if(!racha_byte){
					printf(">> El bit de corte de la parte unaria (%d) es correcto (0).\n\n", i+1);

					i++;               // Me posiciono al inicio del resto de Golomb, salteando el bit de prefijo.
					racha_byte=0x00;   // Obtengo la representacion binaria del resto de Golomb, representado con c=K_racha bits.
					for(int k=0; k<K_racha; k++) PEEK_BIT(racha, i+k, racha_byte, 8+k-K_racha);
					
					R_golomb=(int)(racha_byte&0xFF);               // Obtengo el valor numerico del resto.
					M_eps=(1<<K_racha)*Q_golomb+R_golomb;          // Obtengo el valor codificado.
					
					printf(">> r      = %d\n", R_golomb);
					printf(">> M(eps) = %d\n", M_eps);
					
					i+=K_racha;   // Largo total de la tira de bits leida.
				}else{
					printf(">> El bit de corte de la parte unaria (%d) es incorrecto (1). Racha invalida.\n", i+1);
				}
			}else{
				printf(">> El pixel de salida esta codificado con un codigo de escape.\n");
				
				i+=2;               // Me posiciono al inicio de la codificacion del error, salteando los bits de prefijo y corte.
				racha_byte=0x00;    // Obtengo la representacion binaria del (M(eps)-1).
				for(int k=0; k<8; k++) PEEK_BIT(racha, i+k, racha_byte, k);

				M_eps=1+(int)(racha_byte&0xFF);    // Obtengo el valor numerico de M(eps).

				printf(">> M(eps) = %d\n", M_eps);
				
				i+=8;    // Largo total de la tira de bits leida.
			}

			map=((M_eps+ri_type)%2 ? 1 : 0);   // Deduzco la variable "map", que solo puede ser 0 o 1.
			eps_val=(M_eps+ri_type+map)>>1;    // Obtengo el valor absoluto del error codificado.					
			
			// Decisiones para determinar el signo del error codificado. Invierto la tabla del codificador.
			if(  K_racha  &&   map  && (2*Nn_racha>=N_racha)) {eps_sign=-1; caso_map=1;}
			if(  K_racha  &&   map  && (2*Nn_racha <N_racha)) {eps_sign=-1; caso_map=2;}
			if(  K_racha  && (!map) && (2*Nn_racha>=N_racha)) {eps_sign=(eps_val ? 1 : 0); caso_map=3;}
			if(  K_racha  && (!map) && (2*Nn_racha <N_racha)) {eps_sign=(eps_val ? 1 : 0); caso_map=4;}
			if((!K_racha) &&   map  && (2*Nn_racha>=N_racha)) {eps_sign=-1; caso_map=5;}
			if((!K_racha) &&   map  && (2*Nn_racha <N_racha)) {eps_sign=1; caso_map=6;}
			if((!K_racha) && (!map) && (2*Nn_racha>=N_racha)) {eps_sign=(eps_val ? 1 : 0); caso_map=7;}
			if((!K_racha) && (!map) && (2*Nn_racha <N_racha)) {eps_sign=-1; caso_map=8;}
			
			eps_val*=eps_sign;   // Obtengo el valor numerico del error codificado.
			
			printf(">> map    = %d\n", map);
			printf(">> caso   = %d\n", caso_map);
			printf(">> S(eps) = %d\n", eps_sign);
			printf(">> eps    = %d\n\n", eps_val);
			
			px_x=px_b+eps_val;   // Calculo el valor numerico del pixel de salida.
			
			// Invierto la reduccion de rango para el pixel de salida ([-128, 127] --> [0, 255]).
			if(px_x<0)    px_x+=256;
			if(px_x>=256) px_x-=256;
			if(px_x<0)    px_x=0;
			if(px_x>255)  px_x=255;
			
			printf(">> El pixel inicial (%03d) se repite %d veces. Valor de pixel de salida: %03d.\n>> Racha decodificada.\n\n", px_b, l_racha, px_x);

			// Actualizacion y reset de estadisticos.
			A_racha+=abs(eps_val);
			if(eps_val<0) Nn_racha++;
			
			if(A_racha > 64){
				A_racha/=2;
				N_racha/=2;
				Nn_racha/=2;		
				printf(">> RESET de estadisticos: A=%d - N=%d - Nn=%d\n", A_racha, N_racha+1, Nn_racha);
			}
			
			N_racha++; // N_racha se actualiza siempre despues de un posible reset.
		}else{
			printf(">> El bit de prefijo (%d) no coincide con el prefijo esperado (0). Racha invalida\n", i+1);
		}
	}
}