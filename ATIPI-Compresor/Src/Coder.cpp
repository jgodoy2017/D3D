/**
*  @file Coder.cpp
*  @brief Realiza el trabajo de comprimir la imagen
*
*  @author Felipe Tambasco, Mauro Barbosa
*  @date Feb, 2017
*
*/

#include "Coder.h"
#include "Rachas.h"
#include <sstream>

using namespace std;




Coder::Coder() {

}

Coder::Coder(Image image, int Nmax) {

		//constructor

	this->Nmax=Nmax;

	this->image=image;



}

 void Coder::code(){

	stringstream ss1;

	ss1 << Nmax;
	string nmax = ss1.str();
	int prox=0, largo_racha;


	string path_salida=image.path+image.name+"_coded_Nmax_"+nmax+"_region_3";
	ofstream salida;
	salida.open(path_salida.c_str(), ios::binary);

	writeHeader(salida);

	setContextsArray();
	
	while(prox<image.heigth*image.width){       // Bucle principal que recorre la imagen y la va codificando
		pixels pxls = getPixels(prox);          // obtiene los píxeles de la vecindad: a,b y c
		int p = getP(pxls);	                    // calcula p
		grad gradients=setGradients(p,pxls);    // calcula los gradientes

		if(!gradients.ga && !gradients.gb && !gradients.gc){   // Si encuentra una racha...
			largo_racha=encodeRun(salida, prox);               // ...codifica la racha (incluye actualizacion de estadisticos)...
			prox+=largo_racha;                                 // ...y saltea los pixeles codificados.
			printf("prox=%06d largo_racha=%d %03d %03d %03d\n", prox, largo_racha, pxls.a, pxls.b, pxls.c);
		}else{                                                 // De lo contrario codifica en modo normal.
			int currentPixel=image.image[prox];                // valor del pixel actual
			int contexto = getContext(gradients);	           // trae el contexto asociado a ese gradiente
			int predicted = getPredictedValue(pxls);	       // calcula el valor pixel predicho
			int error_ = currentPixel-predicted;	           // calcula el error como la resta entre el valor actual y el valor predicho
			int k = getK(contexto);	                           // calcula k para ese contexto
			int error = rice(error_);	                       // devuelve mapeo de rice del error
			encode(error, k, salida);	                       // ...codifica el error...
			updateContexto(contexto, error_);	               // ...actualiza los valores para el contexto...
			prox++;                                            // ...y avanza 1 pixel.
			printf("prox=%06d %03d %03d %03d\n", prox, pxls.a, pxls.b, pxls.c);
		}
	}

	flushEncoder(salida);	//termina de escribir los últimos bits que hayan quedado en el array de bits

	salida.close();

}

void Coder::updateContexto(int contexto, int error){

	/** Actualiza los datos N y A del contexto */

	if (contexts[contexto].N==Nmax){

		/* si el valor de N para ese contexto es igual a Nmax divide N y A entre 2 */
		contexts[contexto].N=contexts[contexto].N/2;
		contexts[contexto].A=floor((double)contexts[contexto].A/(double)2);

	}
	/* Actualiza A sumándole el valor absoluto de este error */
	contexts[contexto].A=contexts[contexto].A+abs(error);

	contexts[contexto].N++;	//actualiza N
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

void Coder::encode(int error, int k, ofstream &salida){

		/** Almacena en potencia el valor de 2^k
		Calcula la parte entera del cociente entre el error y 2^k y lo guarda en "cociente" para codificación binaria
		Calcula el resto de la división entera entre el error y 2^k y lo guarda en "resto" para codificación unaria */

	int potencia = 1;

	for (int j=0;j<k;j++){

		potencia=potencia*2;
	}

	int cociente=error/potencia;
	int resto=error%potencia;

	potencia=potencia/2;

	/*	Este loop calcula la expresión binaria del resto expresada con k bits, y lo guarda en array auxiliar bitsToFile */
	for (int j=0;j<k;j++){

			bitsToFile[bitsToFilePointer]=resto/potencia;

			bitsToFilePointer++;

			resto=resto%potencia;

			potencia=potencia/2;

		}
	/* Este loop calcula la expresión unaria del cociente, con tantos ceros como la variable "cociente"
	y lo guarda en array auxiliar bitsToFile */
	for (int j=0;j<cociente;j++){

		bitsToFile[bitsToFilePointer]=0;

		bitsToFilePointer++;

	}
	/*	para indicar el fin del código de la parte unaria escribe un 1 al final */
	bitsToFile[bitsToFilePointer]=1;

	bitsToFilePointer++;

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

	int uno =1;

	if (error>=0)uno=0;

	return (2*abs(error)-uno);
}

int Coder::getK(int contexto){

	/** Calcula k según la expresión de las diapositivas del curso */

	double AdivN=(double)contexts[contexto].A/(double)contexts[contexto].N;

	return round(log2(AdivN));
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

	if (gradients.gc<-3) contgc=0;	//cambiar zonas
		else if (gradients.gc<0) contgc=1;
		else if (gradients.gc==0) contgc=2;
		else if (gradients.gc<=3) contgc=3;
		else contgc=4;

	//mapeo elegido para representar los contextos

	return (5*9*contga)+(5*contgb)+(contgc);
}

void Coder::setContextsArray(){

	/** Forma el array con todos los contextos posibles */

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

Coder::grad Coder::setGradients(int p,pixels pxls){

	/** Dado p y los píxeles a, b y c de la vecindad,
	forma el vector de gradientes */

	grad gradients={pxls.a-p,pxls.b-p,pxls.c-p};

	return gradients;
}

int Coder::getP(pixels pxls){

	/** Devuelve el valor de p, según expresión de las diapositivas del curso */

	return floor((double)(2*pxls.a+2*pxls.b+2*pxls.c+3)/(double)6);

}

Coder::pixels Coder::getPixels(int current){

	/** Devuelve los píxeles de la vecindad: a, b y c */

	int a=-1;
	int b=-1;
	int c=-1;

	if ((current%image.width)==0){

		/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
		o la mitad del valor de blanco de la imagen */
		a=ceil((double)image.white/(double)2);
		c=ceil((double)image.white/(double)2);

	}

	if (current<image.width){

		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */
		if (b==-1) b=ceil((double)image.white/(double)2);
		if (c==-1) c=ceil((double)image.white/(double)2);
	}

	/* Para cada a, b y c, si no se cumple una condición de borde, y por lo tanto no hubo asignación en los if que preceden,
	se traen los valores de a, b y c de la imagen */
	if (a==-1) a=image.image[current-1];
	if (b==-1) b=image.image[current-image.width];
	if (c==-1) c=image.image[current-image.width-1];

	pixels pxls={a,b,c};

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

// Actualiza los datos N, A y Nn del contexto para rachas
void Coder::updateContextoRun(int contexto, int error){
	contextsRun[contexto].updateA(error);
	contextsRun[contexto].updateNn(error);
	if(contextsRun[contexto].A_racha>RESET) contextsRun[contexto].reset();
	contextsRun[contexto].updateN();   // N_racha se actualiza siempre despues de un posible RESET.
}

int Coder::encodeRun(ofstream& salida, int pos){
	int largo=0, largo_px=0, prefijo, aciertos=0, i=0, l_racha=-1, ri_type, eps_val, T_racha, K_racha, map,
	    M_eps, caso_map, L_max, Q_max, Q_golomb, R_golomb, l_pixel, px_a, px_b, px_x;
	unsigned char resto[2]={0x00, 0x00}, resto_bit, racha[10], racha_byte=0x00, golomb[4];
	
//	printf(">> Dimensiones de la imagen: [%d]x[%d]px - Pos.: %d\n", image.width, image.heigth, pos);
	
	px_a=getPixels(pos).a;   // Pixel inicial de la racha, que se repite.
	while((image.image[pos+largo]==px_a) && ((pos+1+largo)%image.width)) largo++;   // Calculo el largo de la racha (pixeles), limitado por la fila actual.
	largo_px=1+largo;   // Largo en pixeles, incluye el pixel inicial.

	prefijo=(((pos+1+largo)%image.width) ? 0 : 1);  // Determino si la racha se corto por un pixel de otro color o porque termino la fila.
//	printf(">> prefijo = %d\n\n", prefijo);
			
	for(int k=0; k<10; k++) racha[k]=0x00;
	
	if(!largo) l_racha=1;      // Solamente el prefijo.
	
	while((largo>0) && (i<32)){  // Adaptativo.
		largo-=(1<<J[i]);        // kr = 2^(J[i])
		i++;                     // "Aciertos"
//		printf(">> i = %02d - kr = %05d - resto = %03d\n", i, 1<<J[i], largo);
	}
	
	if(l_racha==-1){
		if(largo) l_racha=i+1+J[i];   // Racha de "1"s + 1 bit de prefijo + largo del resto
		else      l_racha=i+1;        // Racha de "1"s + 1 bit de prefijo
	}
	
	for(int k=0; k<i; k++){           // Armo la tira de "1"s
		racha_byte=racha_byte|(1<<(7-(k%8)));
		racha[k/8]=racha_byte;
		if(!((k+1)%8)) racha_byte=0x00;
	}
	
	if(largo<0){    // "Fallo"
		largo*=-1;  // Lo dejo positivo.
//		printf(">> resto = %d = " BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "b representado con %d bits.\n", largo, BYTE_TO_BINARY((unsigned char)(largo/256)), BYTE_TO_BINARY((unsigned char)(largo)), J[i]);

		resto[0]=(unsigned char)(largo/256);   // El resto nunca puede
		resto[1]=(unsigned char)(largo%256);   // ocupar mas de 2 bytes.
		
		racha[i/8]=racha[i/8]|(prefijo<<(7-i%8));   // Agrego el prefijo...	
		if(J[i]>8){                                 // ...y el resto, que puede tener 2 bytes...
			for(int k=i+1; k<i+J[i]-7; k++)      POKE_BIT(racha, k, resto[0], 15-J[i]+k-i);
			for(int k=i+J[i]-7; k<i+J[i]+1; k++) POKE_BIT(racha, k, resto[1], 7-J[i]+k-i);
		}else{                                      // ...o 1 byte.
			for(int k=i+1; k<i+J[i]+1; k++)      POKE_BIT(racha, k, resto[1], 7-J[i]+k-i);
		}
	}
	
//	printf(">> Codificacion del largo de racha: ");
//	for(int k=0; k<6; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(racha[k]));
//	printf("b - Largo: %d bits.\n\n", l_racha);
	
	if(!prefijo){            // Si la racha es interrumpida por un pixel de otro color, codifico el pixel de interrupcion.
		px_x=image.image[pos+1+largo];    // Pixel x, corte de la racha.
		px_b=getPixels(pos+1+largo).b;    // Pixel b, superior al de corte.
		
		ri_type=((px_a==px_b) ? 1 : 0);   // Run Interruption Type.
		eps_val=px_x-px_b;                // Error de prediccion a codificar.
		
//		printf(">> RIType = %d\n", ri_type);	
//		printf(">> A      = %d\n", contextsRun[ri_type].A_racha);
//		printf(">> N      = %d\n", contextsRun[ri_type].N_racha);
//		printf(">> Nn     = %d\n\n", contextsRun[ri_type].Nn_racha);
			
		// Reduccion de rango para el error a codificar ([-255, 255] --> [-128, 127]).
		if(eps_val<0)    eps_val+=256;
		if(eps_val>=128) eps_val-=256;
		if(eps_val<-128) eps_val=-128;
		if(eps_val>127)  eps_val=127;

		// Calculo la variable T para determinar k de Golomb.
		T_racha=((ri_type==1) ? contextsRun[ri_type].A_racha : contextsRun[ri_type].A_racha + contextsRun[ri_type].N_racha/2);   
		for(K_racha=0; (contextsRun[ri_type].N_racha<<K_racha)<T_racha; K_racha++);    // k = min{k' / 2^(k') >= T}

		// Decisiones para hallar el valor de "map", utilizado en el mapeo que se le hace al error a codificar.
		if(  K_racha  && (eps_val<=0) && (2*contextsRun[ri_type].Nn_racha>=contextsRun[ri_type].N_racha)) {map=(eps_val ? 1 : 0); caso_map=1;}
		if(  K_racha  && (eps_val<=0) && (2*contextsRun[ri_type].Nn_racha <contextsRun[ri_type].N_racha)) {map=(eps_val ? 1 : 0); caso_map=2;}
		if(  K_racha  && (eps_val >0) && (2*contextsRun[ri_type].Nn_racha>=contextsRun[ri_type].N_racha)) {map=0; caso_map=3;}
		if(  K_racha  && (eps_val >0) && (2*contextsRun[ri_type].Nn_racha <contextsRun[ri_type].N_racha)) {map=0; caso_map=4;}
		if((!K_racha) && (eps_val<=0) && (2*contextsRun[ri_type].Nn_racha>=contextsRun[ri_type].N_racha)) {map=(eps_val ? 1 : 0); caso_map=5;}
		if((!K_racha) && (eps_val<=0) && (2*contextsRun[ri_type].Nn_racha <contextsRun[ri_type].N_racha)) {map=0; caso_map=6;}
		if((!K_racha) && (eps_val >0) && (2*contextsRun[ri_type].Nn_racha>=contextsRun[ri_type].N_racha)) {map=0; caso_map=7;}
		if((!K_racha) && (eps_val >0) && (2*contextsRun[ri_type].Nn_racha <contextsRun[ri_type].N_racha)) {map=1; caso_map=8;}
	
		M_eps=2*abs(eps_val)-ri_type-map;    // Mapeo del error a codificar por Golomb(2^k).
	
//		printf(">> T      = %d\n", T_racha);
//		printf(">> k      = %d\n", K_racha);
//		printf(">> map    = %d\n", map);
//		printf(">> caso   = %d\n", caso_map);	
//		printf(">> eps    = %d\n", eps_val);
//		printf(">> M(eps) = %d\n\n", M_eps);
	
		Q_golomb=(M_eps>>K_racha);             // q=n/m   donde n=M_eps, m=2^k, k=K_racha. Dividir entre   m es "perder" K_racha bits.
		R_golomb=(M_eps-(Q_golomb<<K_racha));  // r=n-m*k donde n=M_eps, m=2^k, k=K_racha. Multipilcar por m es "ganar"  K_racha bits.
		// Notese que la otra ecuacion, c=techo(log2(m)) con m=2^k, implica c=k. Como k=K_racha, ya lo tenemos calculado.
	
		L_max=32-J[i];    // Largo maximo para el Golomb de largo limitado.          L_max = 32 - (kr + 1)	
		Q_max=L_max-9;    // Valor de q que decide si limitamos el largo del codigo. q_max = (L_max - 1) - 8
	
//		printf(">> n     = %d\n", M_eps);
//		printf(">> m     = %d\n", 1<<K_racha);
//		printf(">> q     = %d\n", Q_golomb);
//		printf(">> r     = %d\n", R_golomb);
//		printf(">> c     = %d\n", K_racha);
//		printf(">> L     = %d\n", Q_golomb+1);
//		printf(">> L_max = %d\n", L_max);
//		printf(">> q_max = %d\n\n", Q_max);
	
		for(int k=0; k<4; k++) golomb[k]=0x00;
		if(Q_golomb<Q_max){   // No limitamos el largo del codigo. Codificamos en Golomb(2^k).
//			printf(">> Codificamos en Golomb(2^%d).\n>> El prefijo ocupa %d bits y el resto ocupa %d bits.\n\n", K_racha, Q_golomb+1, K_racha);
		
			// Parte unaria
			for(int k=0; k<Q_golomb; k++) POKE_BIT(golomb, k, 0xFF, 7);      // Agrego q 1's...
			POKE_BIT(golomb, Q_golomb, 0x00, 7);                             // ...y un 0.
		
			// Parte binaria
			racha_byte=(unsigned char)(R_golomb&0xFF);
			for(int k=Q_golomb+1; k<Q_golomb+1+K_racha; k++) POKE_BIT(golomb, k, racha_byte, 7-Q_golomb+k-K_racha);
			
//			printf(">> Codificacion del pixel de salida: ");
//			for(int k=0; k<4; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(golomb[k]));
//			printf("b - Largo: %d bits.\n", Q_golomb+1+K_racha);
		
			l_pixel=Q_golomb+1+K_racha;   // Largo total de la codificacion del pixel de salida.
		}else{                // Limitamos el largo del codigo. Enviamos "codigo de escape".
//			printf(">> Enviamos codigo de escape.\n>> Limitamos el largo del prefijo (%d bits) a %d bits, con 8 bits de resto.\n\n", Q_golomb+1, Q_max+1);
		
			// Parte unaria
			for(int k=0; k<Q_max; k++) POKE_BIT(golomb, k, 0xFF, 1);      // Agrego q_max 1's...
			POKE_BIT(golomb, Q_max, 0x00, 1);                             // ...y un 0.
		
			// Parte binaria
			racha_byte=(unsigned char)((M_eps-1)%256);   // En este caso, se codifica (M(eps) - 1) por definicion.
			for(int k=Q_max+1; k<Q_max+9; k++) POKE_BIT(golomb, k, racha_byte, k-1-Q_max);
		
//			printf(">> Codificacion del pixel de salida: ");
//			for(int k=0; k<4; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(golomb[k]));
//			printf("b representada con %d bits.\n", Q_max+9);
		
			l_pixel=Q_max+9;   // Largo total de la codificacion del pixel de salida.
		}
	
		// Agrego a la codificacion del largo la codificacion del pixel de salida.
		for(int m=0; m<4; m++){
			for(int k=l_racha%8; k<8; k++)   POKE_BIT(racha, l_racha+8*m+k-l_racha%8, golomb[m], k-l_racha%8);
			for(int k=8-l_racha%8; k<8; k++) POKE_BIT(racha, l_racha+8*m+k, golomb[m], k);	
		}
		
		updateContextoRun(ri_type, eps_val);   // Actualizacion y reset de estadisticos.
//		printf(">> Valores actualizados de los estadisticos para el contexto [%d]: A = [%d] - N = [%d] - Nn = [%d]\n\n", ri_type, contextsRun[ri_type].A_racha, contextsRun[ri_type].N_racha, contextsRun[ri_type].Nn_racha);
	}
	
//	printf(">> Tira de bits: ");
//	for(int k=0; k<10; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(racha[k]));
	
//	printf("b - Largo: %d\n", l_racha+l_pixel);
	
	uchar2bool(racha, l_racha+l_pixel);  // Cargo el array de bools.
	writeCode(salida);	                 // Escribo en el archivo.
	return(largo_px);                    // Devuelvo la cantidad de pixeles consumidos. Como minimo se lee 1 pixel.
}

void Coder::uchar2bool(unsigned char* uchar, int bits_uchar){	
	if(bits_uchar && (bitsToFilePointer+bits_uchar<800)){
		for(int k=0; k<bits_uchar; k++){
			if(((uchar[k/8]&(1<<(7-k%8)))>>(7-k%8))==0x01) bitsToFile[bitsToFilePointer+k]=true;
			else                                           bitsToFile[bitsToFilePointer+k]=false;
		}
		
		bitsToFilePointer+=bits_uchar;
	}		
}

Coder::~Coder() {}
