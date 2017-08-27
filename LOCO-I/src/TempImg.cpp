/**
*  @file TempImg.cpp
*  @brief Realiza el trabajo de comprimir la imagen
*
*  @author Felipe Tambasco, Mauro Barbosa
*  @date Feb, 2017
*
*/

#include "TempImg.h"
#include <sstream>

namespace std {

TempImg::TempImg() {
}

TempImg::TempImg(Image image, Image image2) {
		//constructor
	this->image=image;
	this->image2=image2;

}

 void TempImg::makeImg(){
     int bsize = 10;
     int bh = bsize;
     int bv = bsize;
     int search = 12;
	 int sIzq = 0;
	 int sDer = 0;
	 int sArr = 0;
	 int sAba = 0;
	 int vector_ind = 0;

     tempimage=(int*)malloc(this->image.width*this->image.heigth*sizeof(int));
     h_vector=(int*)malloc(this->image.width*this->image.heigth*sizeof(int));
     v_vector=(int*)malloc(this->image.width*this->image.heigth*sizeof(int));

	initTemp();

    string path_salida=image.path+image.name+"_Temp.pgm";
	ofstream salida;
	salida.open(path_salida.c_str(), ios::binary);

	writeHeader(salida);
	int s = 0;
	//cout<<"p:"<<endl;

	for(int bloqueV=0;(bloqueV)*bsize<image2.heigth;bloqueV++){
		for(int bloqueH=0;(bloqueH)*bsize<image2.width;bloqueH++){

			int smin = 429496729;
			int hmin = 0;
			int vmin = 0;
			restoV = max((bloqueV+1)*bsize-image2.heigth, 0);
			restoH = max((bloqueH+1)*bsize-image2.width, 0);
			derecha = abs((bloqueH+1)*bsize - image2.width);
			abajo = abs((bloqueV+1)*bsize - image2.heigth);

			//cout<<"H: "<<bloqueH<<" V: "<<bloqueV<<" restoV: "<<restoV<<" restoH "<<restoH<<" derecha: "<<derecha<<" abajo: "<<abajo<<endl;

			sIzq = min(search,bloqueH*bsize);
			sDer = min(search,derecha - restoH);
			sArr = min(search,bloqueV*bsize);
			sAba = min(search,abajo - restoV);
			//cout<<"H: "<<bloqueH<<" V: "<<bloqueV<<" sIzq: "<<sIzq<<" sDer: "<<sDer<<" sArr: "<<sArr<<" sAba: "<<sAba<<endl;
			bh = bsize - restoH;
			bv = bsize - restoV;
			if (debug) cout<<"H: "<<bloqueH<<" V: "<<bloqueV<<endl;
			if (debug) cout<<"restoV: "<<restoV<<" restoH: "<<restoH<<" sIzq: "<<sIzq<<" sDer: "<<sDer<<" sArr: "<<sArr<<" sAba: "<<sAba<<endl;
			if (debug) cout<<"bh: "<<bh<<" bv: "<<bv<<endl;

			for(int v=-sArr;v<sAba+1;v++){
						for(int h=-sIzq;h<sDer+1;h++){
							s = 0;
							//cout<<v<<" "<<h<<endl;

							for(int j=0;j<bv;j++){
									for(int i=0;i<bh;i++){

										int pix = (i + j*image.width) + (h + v*image.width) + (bloqueH*bsize + bloqueV*bsize*(image.width));
										int pix2 = (i + j*image2.width) + (bloqueH*bsize + bloqueV*bsize*(image2.width));
										//cout<<"p:"<<pix<<" - p2:"<<pix2<<endl;

										itera("MED3D",s,pix,pix2);

									}
							}
							//cout<<"S:"<<s<<" Smin: "<<smin<<endl;
							if (s < smin){
								//cout<<"Entro "<<s<<endl;
								smin = s;
								hmin = h;
								vmin = v;
							}
						}
					}
					h_vector[vector_ind] = hmin;
					v_vector[vector_ind] = vmin;
					vector_ind++;
					if (debug) cout<<"v:"<<vmin<<" - h:"<<hmin<<endl<<endl;
					for(int j=0;j<bv;j++){
							for(int i=0;i<bh;i++){
								int pix = (i + j*image.width) + (hmin + vmin*image.width) + (bloqueH*bsize + bloqueV*bsize*(image.width));
								int pix2 = (i + j*image2.width) + (bloqueH*bsize + bloqueV*bsize*(image2.width));
								tempimage[pix2]=image.image[pix]; // Normal
								//int c = pix2%image.width; // para imprimir laplaciano
								//int f = ceil(pix2/image.width); // para imprimir laplaciano
								//tempimage[pix2] = abs(image2.image[min(pix2+1,(f+1)*image2.width-1)]+image2.image[max(pix2-1,f*image2.width)]+image2.image[max(pix2-image2.width,c)]+image2.image[min(pix2+image2.width,(image2.heigth-1)*image2.width+c)]-4*image2.image[pix2]); // para imprimir laplaciano
								//cout<<hmin<<","<<vmin<<"";
							}
							//cout<<endl;
					}
					drawLine(bloqueH, bloqueV, bsize, hmin, vmin, image.white);
				//}

			//cout<<endl;
		}
	}
	for(int i=0;i<vector_ind;i++){
		cout<<h_vector[i]<<","<<v_vector[i]<<endl;
	}
	for(int pix=0;pix<image.width*image.heigth;pix++){
			//tempimage[pix]=image.image[pix];
			int temp=tempimage[pix]; //valor del pixel actual
			char temp_=(char)temp;
			salida.write(&temp_,1);
		}
	salida.close();

}

void TempImg::itera(string function, int &s, int pix, int pix2){
	/* Valores válidos para Function:
	 * 	"MED3D": Utilizando el predictor 3D
	 * 	"ERROR": Utilizando la diferencia entre pixeles
	 */
	if (function == "MED3D"){
		pixelsTemp pxls = getPixels(pix, pix2);
		gradientes = setGradientsTemp(1,pxls);
		s = s + abs(getPredictedValue(selectMED(gradientes),pxls)-image2.image[pix2]);
	}
	if (function == "ERROR"){
		s = s + abs(image.image[pix]-image2.image[pix2]);
	}
	if (function == "LAPLACE"){
		int c = pix%image.width;
		int f = ceil(pix/image.width);
		s = s + abs(abs(image.image[min(pix+1,(f+1)*image.width-1)]+image.image[max(pix-1,f*image.width)]+image.image[max(pix-image.width,c)]+image.image[min(pix+image.width,(image.heigth-1)*image.width+c)]-4*image.image[pix])-abs(image2.image[min(pix+1,(f+1)*image2.width-1)]+image2.image[max(pix-1,f*image2.width)]+image2.image[max(pix-image2.width,c)]+image2.image[min(pix+image2.width,(image2.heigth-1)*image2.width+c)]-4*image2.image[pix]));
	}

}

void TempImg::initTemp() {
	for(int pix=0;pix<image.width*image.heigth;pix++){
		//cout<<"entro"<<endl;
		tempimage[pix]=0;
	}
}

void TempImg::drawImage(int x, int y, int value){
	image.image[x + y*image.width] = value;
}

void TempImg::drawImage2(int x, int y, int value){
	image2.image[x + y*image2.width] = value;
}

void TempImg::drawTemp(int x, int y, int value){
	tempimage[x + y*image.width] = value;
}


void TempImg::drawLine(int bloqueH, int bloqueV, int bsize, int h, int v, int value){

	if (h==0 && v==0) {
		drawTemp(bloqueH*bsize, bloqueV*bsize, image.white);
	} else {
	double length = sqrt( h*h + v*v );

	double addh = h / length;
	double addv = v / length;

	double x = bloqueH*bsize;
	double y = bloqueV*bsize;

	for(int i = 0; i < length; i += 1)
	{
	  drawTemp(floor(x), floor(y), image.white-10*i);
	  x += addh;
	  y += addv;
	}
	}

}
void TempImg::encode(int error, int k, ofstream &salida){

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

void TempImg::writeCode(ofstream &salida){

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

void TempImg::writeHeader(ofstream &salida){

	/** Escribe el encabezado de la imagen codificada,
	para esto se sigue el mismo esquema presente en el archvo .pgm,
	con el agregado de escribir también el valor de Nmax

	los 5 métodos que se usan para escribir el encabezado, que se listan a continuación,
	siguen la misma estructura interna general */

	writeMagic(salida);
	writeWidth(salida);
	writeHeigth(salida);
	writeWhite(salida);

}

void TempImg::writeMagic(ofstream &salida){

	/** Como solo se trabaja con imagenes tipo P5,
	directamente se escriben estos caracteres*/

	char temp='P';
	salida.write(&temp,1);

	temp='5';
	salida.write(&temp,1);

	temp='\n';
	salida.write(&temp,1);
}

void TempImg::writeWidth(ofstream &salida){

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

void TempImg::writeHeigth(ofstream &salida){

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

void TempImg::writeWhite(ofstream &salida){

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

int TempImg::selectMED(gradTemp gradients){
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

int TempImg::getPredictedValue(int modo, pixelsTemp pxls){

	/** Calcula el valor predicho según expresión de las diapositivas del curso */
	int pred;

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

TempImg::pixelsTemp TempImg::getPixels(int current, int current2){

	/** Devuelve los píxeles de la vecindad: a, b, c, d, a_, b_, c_, d_, e_, f_ y g_ */

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
		a=ceil((double)image2.white/(double)2);
		c=ceil((double)image2.white/(double)2);

	}

	if ((current2%image2.width)==image.width-1){

		/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
		o la mitad del valor de blanco de la imagen */
		d=ceil((double)image2.white/(double)2);

	}

	if (current2<image2.width){

		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */
		if (b==-1) b=ceil((double)image2.white/(double)2);
		if (c==-1) c=ceil((double)image2.white/(double)2);
		if (d==-1) d=ceil((double)image2.white/(double)2);

	}

	if ((current%image.width)==0){

			/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
			o la mitad del valor de blanco de la imagen */
			a_=ceil((double)image.white/(double)2);
			c_=ceil((double)image.white/(double)2);

		}

		if ((current%image.width)==image.width-1){

			/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
			o la mitad del valor de blanco de la imagen */
			d_=ceil((double)image.white/(double)2);
			f_=ceil((double)image.white/(double)2);

		}

		if (current<image.width){

			/* Si estoy en la primer fila, b y c deben ser "128"
			o la mitad del valor de blanco de la imagen */
			if (b_==-1) b_=ceil((double)image.white/(double)2);
			if (c_==-1) c_=ceil((double)image.white/(double)2);
			if (d_==-1) d_=ceil((double)image.white/(double)2);
		}

		if (current>(image.heigth-2)*image.width){

			/* Si estoy en la última o penúltima fila, g debe ser "128"
			o la mitad del valor de blanco de la imagen */
			g_=ceil((double)image.white/(double)2);
		}

	/* Para cada a, b,c y d, si no se cumple una condición de borde, y por lo tanto no hubo asignación en los if que preceden,
	se traen los valores de a, b,c y d de la imagen */
	if (a==-1) a=image2.image[current2-1];
	if (b==-1) b=image2.image[current2-image.width];
	if (c==-1) c=image2.image[current2-image.width-1];
	if (d==-1) d=image2.image[current2-image.width+1];
	if (a_==-1) a_=image.image[current-1];
	if (b_==-1) b_=image.image[current-image.width];
	if (c_==-1) c_=image.image[current-image.width-1];
	if (d_==-1) d_=image.image[current-image.width+1];
	if (e_==-1) e_=image.image[current];
	if (f_==-1) f_=image.image[current+1];
	if (g_==-1) g_=image.image[current+image.width];
	pixelsTemp pxls={a,b,c,d,a_,b_,c_,d_,e_,f_,g_};

		return pxls;
}

TempImg::gradTemp TempImg::setGradientsTemp(int modo, pixelsTemp pxls){

	/** Devuelve los gradientes según el modo de funcionamiento elegido:
	 * Modos:
	 * 		1 = Devuelve los gradientes de elección de predictor
	 * 		2 = Devuelve los gradientes del predictor filas-tiempo
	 * 		3 = Devuelve los gradientes del predictor columnas-tiempo
	 * 		0 o cualquier otros = Devuelve los gradientes del predictor filas-columnas
	 *  */

	gradTemp gradients={pxls.d-pxls.b,pxls.b-pxls.c,pxls.c-pxls.a};

	if (modo == 1){
			gradients={pxls.a-pxls.a_,pxls.b-pxls.c,pxls.c-pxls.a};
	}
	if (modo == 2){
			gradients={pxls.g_-pxls.e_,pxls.e_-pxls.b_,pxls.b_-pxls.b};
	}
	if (modo == 3){
			gradients={pxls.f_-pxls.e_,pxls.e_-pxls.a_,pxls.a_-pxls.a};
	}

	return gradients;
}

TempImg::~TempImg() {

}

} /* namespace std */
