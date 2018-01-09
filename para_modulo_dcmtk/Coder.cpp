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
#include "Coder.h"
#include "Decoder.h"
#include "Writer.h"
#include "Writer2.h"
#include "Reader.h"
#include <sstream>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <bitset>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <iomanip>
#include <dirent.h>


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

void Coder::cargar_imagen_actual(const void* uncompressedData,struct JlsParameters info){

	const Uint8* imagen = static_cast<const Uint8*>(uncompressedData);

	image2=Image(info.height,info.width,255);


	for (int variable=0; (variable<info.height*info.width); variable++){


		image2.image[variable]=(int)imagen[variable];


			}


}



int Coder::code_dcmtk(Writer2 &writer,
			const void* uncompressedData, Image &image, struct JlsParameters info){

		//dónde se seteaba la cantidad de bits por pixel?

		//método funcionando para imágenes de 8 bits por píxel, cuando se adapte, ver bien cómo se combina con
						//los parámetros info.bytesperline, width y heigth

		//sin chequeos sobre tamaños de imagen, largos, etc.

		cargar_imagen_actual(uncompressedData,info);	//carga uncompressedData en imagen2
		this->image=image;

		cout << "// START CODER" << endl;

		setContextsArray();


			for(int y = 0; y<image.heigth;y++){
				for(int x = 0; x<image.width;x++){
					//bucle principal que recorre la imagen y va codificando cada pixel

					int signo;
					bool esRacha;
					int currentPixel=image2.getPixel(x,y); //valor del pixel actual


					pixels3D pxls = getPixels3D(x,y,x,y);
					grad gradients=getGradients3D(1,pxls); //calcula los gradientes
					int contexto = getContext(getGradients3D(0,pxls),getGradients3D(4,pxls), signo, esRacha);

					if (!esRacha){
						int predicted = getPredictedValue(selectMED(gradients),pxls);	//calcula el valor pixel predicho
						predicted     = fixPrediction(predicted,signo, contexto);

						int error_s = currentPixel - predicted;         //calcula el error como la resta entre el valor actual y el valor predicho
						int error_  = error_s*signo;
						int k       = getK(contexto);	                //calcula k para ese contexto
						int error__ = reduccionDeRango(error_);
						int error   = rice(error__, get_s(contexto),k);	//devuelve mapeo de rice del error
						encode(error, k, writer, 0, 0);	                //codifica el error
						updateContexto(contexto, error_);	            //actualiza los valores para el contexto
					}
					else {
						int interruption=0;
						int largo    = getRachaParams2(image2,x,y,pxls.a,interruption);
						int contexto = getContext_(x,y,largo);

						Racha racha(largo, interruption, pxls.a, contexto);

						int cantidad_unos = encodeRacha2(racha, writer);
						if(x + y*width + largo < width*heigth) encodeMuestraInterrupcion(racha, image2.getPixel(x + largo, y), x + largo, y, writer, cantidad_unos);
						racha.updateContexto();

						x += largo;
						if(racha.interruption) x--;
					}
				}
			} //pixeles



			writer.close();

			return (cantidad_bits/8)+1;	//le sumo 1 por el último flush
}


 void Coder::code(bool vector, Writer &writer){
	//v_ancho = 1+(image.width/bsize);
	//v_alto = 1+(image.heigth/bsize);

	v_ancho = ((image.width%bsize==0) ?(v_ancho = image.width/bsize) : (v_ancho = image.width/bsize + 1));
	v_alto = ((image.heigth%bsize==0) ?(v_alto = image.heigth/bsize) : (v_alto = image.heigth/bsize + 1));

	v_blanco = 2*search;

	stringstream ss1;
	ss1 << Nmax;
	string nmax = ss1.str();
	Image* imageH = new Image();
	Image* imageV = new Image();

	cout << "// START CODER" << endl;

	if (!vector) writeHeader(writer);
	setContextsArray();

	for (int imagen=0;imagen<cantidad_imagenes;imagen++){
		//cout << (vector ? "vector " : "imagen ") << "coder: " << imagen << endl;

		image2 = images[imagen];
		if (dibujarVectores) setTempImage();
		if (imagen > 0) image = images[imagen-1];

		//if(!vector) cout << "Procesando imagen >> " << image2.path << endl;

		// ######## Compensación de movimiento
		if (activarCompMov && !vector){
			//imageH->image=(int*)malloc(this->image.width*this->image.heigth*sizeof(int));
			//imageV->image=(int*)malloc(this->image.width*this->image.heigth*sizeof(int));
			imageH->image=(int*)malloc(v_ancho*v_alto*sizeof(int));
			imageV->image=(int*)malloc(v_ancho*v_alto*sizeof(int));
			imageH->width=v_ancho;
			imageV->width=v_ancho;
			imageH->heigth=v_alto;
			imageV->heigth=v_alto;
			imageH->white=v_blanco;
			imageV->white=v_blanco;

			CompMov(*imageH, *imageV);

			Coder* coderVec = new Coder(*imageH, *imageV, Nmax);
			coderVec->code(1, writer);
		}

		for(int y = 0; y<image.heigth;y++){
			for(int x = 0; x<image.width;x++){
				//bucle principal que recorre la imagen y va codificando cada pixel
				int x_prev,y_prev;
				int signo;
				bool esRacha;
				int currentPixel=image2.getPixel(x,y); //valor del pixel actual
				
				getProxImageAnterior(x,y,x_prev,y_prev,vector,*imageH,*imageV);
				pixels3D pxls = getPixels3D(x_prev,y_prev,x,y);
				grad gradients=getGradients3D(1,pxls); //calcula los gradientes
				int contexto = getContext(getGradients3D(0,pxls),getGradients3D(4,pxls), signo, esRacha);

				if (!esRacha){
					int predicted = getPredictedValue(selectMED(gradients),pxls);	//calcula el valor pixel predicho
					predicted     = fixPrediction(predicted,signo, contexto);
					
					int error_s = currentPixel - predicted;         //calcula el error como la resta entre el valor actual y el valor predicho
					int error_  = error_s*signo;
					int k       = getK(contexto);	                //calcula k para ese contexto
					int error__ = reduccionDeRango(error_);
					int error   = rice(error__, get_s(contexto),k);	//devuelve mapeo de rice del error
					encode(error, k, writer, 0, 0);	                //codifica el error
					updateContexto(contexto, error_);	            //actualiza los valores para el contexto
				}
				else {
					int interruption=0;
					int largo    = getRachaParams2(image2,x,y,pxls.a,interruption);
					int contexto = getContext_(x,y,largo);
					
					Racha racha(largo, interruption, pxls.a, contexto);
					
					int cantidad_unos = encodeRacha2(racha, writer);
					if(x + y*width + largo < width*heigth) encodeMuestraInterrupcion(racha, image2.getPixel(x + largo, y), x + largo, y, writer, cantidad_unos);
					racha.updateContexto();
					
					x += largo;
					if(racha.interruption) x--;
				}
			}
		} //pixeles

		cout<<endl;

		for (int val=0;val<image2.heigth*image2.width;val++){

			if (val%image2.width==0)cout<<endl;
			cout<<image2.image[val]<<" ";

		}

	}//imagenes
}

 void Coder::CompMov(Image &imageH, Image &imageV){
 	int bh; 	// Aquí se guardará el ancho del Macrobloque cuando sea menor al especificado
     int bv ; 	// Aquí se guardará el alto del Macrobloque cuando sea menor al especificado
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
     int cant;
     int media;
     float var;

     for(int bloqueV=0;(bloqueV)*bsize<image2.heigth;bloqueV++){
     	for(int bloqueH=0;(bloqueH)*bsize<image2.width;bloqueH++){
     		// Con estos dos FOR se recorren todos los Macrobloques
   			// Su posición es dada por (bloqueH, bloqueV)
   			// Se inicializan valores en cada Macrobloque
   			// -------------------------
   			int smin = 429496729;
   			int hmin = 0;
   			int vmin = 0;
   			media=0;
   			var=0;
   			s = 0;
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
   			cant = bh*bv;
   			int lista[cant];
   			int h = -sIzq;
   			int v = -sArr;
   			// ------------------------------

 			for(int j=0;j<bv;j++){
 				for(int i=0;i<bh;i++){
 					// Estos FOR son para moverse por todos los pixeles del primer Macrobloque
 					int x1 = i + h + bloqueH*bsize;
 					int y1 = j + v + bloqueV*bsize;

 					int x2 = i + bloqueH*bsize;
 					int y2 = j + bloqueV*bsize;

 					itera("MED3D",s,x1,y1,x2,y2);

 					media = media + image2.getPixel(x2,y2)/cant;
 					lista[i+j*bh] = image2.getPixel(x2,y2);
 				}
 			}

 			var = varianza(media,cant,lista);

 			if (var > 500 || !activarVarianza) {
 				for(v=-sArr;v<sAba+1;v++){
 					for(h=-sIzq+1;h<sDer+1;h++){
 						// Estos FOR son para mover el macrobloque
 						s = 0; // Se inicializa la variable sobre la que se va a iterar en cada Matching
 						for(int j=0;j<bv;j++){
 							for(int i=0;i<bh;i++){
 								// Estos FOR son para moverse por todos los pixeles del Macrobloque
 								int x1 = i + h + bloqueH*bsize;
 								int y1 = j + v + bloqueV*bsize;

 								int x2 = i + bloqueH*bsize;
 								int y2 = j + bloqueV*bsize;

 								itera("MED3D",s,x1,y1,x2,y2);
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
 			} else {
 				hmin = 0;
 				vmin = 0;
 			}
  			imageH.setPixel(hmin+search,bloqueH,bloqueV);
   			imageV.setPixel(vmin+search,bloqueH,bloqueV);
   			if (dibujarVectores) drawLine(bloqueH, bloqueV, bsize, hmin, vmin, white);
 		}
   	}
  	cout <<"Salgo CompMov"<< endl;
}

float Coder::varianza(float media,int cant,int lista[]){
  	float varianza = 0;
  	for(int i=0;i<cant;i++){
  		varianza = varianza + pow(lista[i]-media,2);
  	}

  	return varianza;
}
 void Coder::itera(string function, int &s, int x1, int y1, int x2, int y2){
  	/* Valores válidos para Function:
  	 * 	"MED3D": Utilizando el predictor 3D
  	 * 	"ERROR": Utilizando la diferencia entre pixeles
  	 * 	"LAPLACE": Utilizando laplaciano
  	 */

	if (function == "MED3D"){
  		pixels3D pxls = getPixels3D(x1,y1,x2,y2);
  		grad gradientes = getGradients3D(1,pxls);
  		s = s + abs(getPredictedValue(selectMED(gradientes),pxls)-image2.getPixel(x2,y2));
  	}

	if (function == "ERROR"){
  		s = s + abs(image.getPixel(x1,y1)-image2.getPixel(x2,y2));
  	}

	if (function == "LAPLACE"){
		s = s + abs(image.getPixel(min(x1+1,image.width-1),y1)+image.getPixel(max(x1-1,0),y1)+image.getPixel(x1,max(y1-1,0))+image.getPixel(x1,min(y1+1,image.heigth-1))-4*image.getPixel(x1,y1) - (image2.getPixel(min(x2+1,image2.width-1),y2)+image2.getPixel(max(x2-1,0),y2)+image2.getPixel(x2,max(y2-1,0))+image2.getPixel(x2,min(y2+1,image2.heigth-1))-4*image2.getPixel(x2,y2)));
	}
}

 void Coder::drawTemp(int x, int y, int value){
	 tempimage[x + y*image.width] = value;
 }


 void Coder::drawLine(int bloqueH, int bloqueV, int bsize, int h, int v, int value){

 	if (h==0 && v==0) {
 		drawTemp(bloqueH*bsize, bloqueV*bsize, image2.white);
 	} else {
 	double length = sqrt( h*h + v*v );

 	double addh = h / length;
 	double addv = v / length;

 	double x = bloqueH*bsize;
 	double y = bloqueV*bsize;

 	for(int i = 0; i < length; i += 1)
 	{
 	  drawTemp(floor(x), floor(y), image2.white-10*i);
 	  x += addh;
 	  y += addv;
 	}
 	}

 }

 void Coder::setTempImage(){
	 	tempimage = image2.image;//	definir algún criterio para esta imagen
}

void Coder::getProxImageAnterior(int x, int y, int &x_prev, int &y_prev, bool vector, Image imagenH, Image imagenV){
	x_prev = x;
	y_prev = y;

	if (activarCompMov && !vector){
 		int bloqueV = y / bsize;
 	 	int bloqueH = x / bsize;
		x_prev = x + imagenH.getPixel(bloqueH,bloqueV)-search;
		y_prev = y + imagenV.getPixel(bloqueH,bloqueV)-search;
	}
}

int Coder::max(int uno, int dos){
	 if (uno<dos) return dos;
	 else return uno;
}

float Coder::get_s(int contexto){
	 return float(float(contexts[contexto].B)/float(contexts[contexto].N));
}

int Coder::getRachaParams2(Image &image, int x, int y, int anterior, int &interruption_){
 	int largo=0;
 	interruption_=0;

 	while(anterior==image.getPixel(x+largo,y)){
 		largo++;
 		if((x+largo)==image.width) {
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

void Coder::encodeMuestraInterrupcion(Racha &racha, int siguiente, int x, int y, Writer2 &writer,int cantidad_unos){
	int error=0, error_=0, kPrime=1000;

	if (!racha.interruption){
		if (racha.contexto==0){
			error_=(siguiente-getPixels3D(0,0,x,y).b)*(-1);
		} else {
		error_=(siguiente-getPixels3D(0,0,x,y).b);
		}

		kPrime=getKPrime(racha);
		error_=reduccionDeRango(error_);
		int map=0;
		error=rice_rachas(error_,racha.contexto,kPrime,map);
		updateContexto_(racha.contexto, error_,error,map,kPrime);
		encode(error, kPrime, writer,1,cantidad_unos);
	}
}

void Coder::encodeMuestraInterrupcion(Racha &racha, int siguiente, int x, int y, Writer &writer,int cantidad_unos){
	int error=0, error_=0, kPrime=1000;

	if (!racha.interruption){
		if (racha.contexto==0){
			error_=(siguiente-getPixels3D(0,0,x,y).b)*(-1);
		} else {
		error_=(siguiente-getPixels3D(0,0,x,y).b);
		}

		kPrime=getKPrime(racha);
		error_=reduccionDeRango(error_);
		int map=0;
		error=rice_rachas(error_,racha.contexto,kPrime,map);
		updateContexto_(racha.contexto, error_,error,map,kPrime);
		encode(error, kPrime, writer,1,cantidad_unos);
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

/*
int Coder::encodeRacha2(Racha &racha, Writer &writer){
	int diferencia=racha.largo;
	int ajuste;

	while (diferencia >= (1 << J[RUNindex])) {
	   //Agregar un 1 al stream
		writer.write(1, 1);

		diferencia = diferencia - (1 << J[RUNindex]);

		if (RUNindex < 31) {
			RUNindex++;
			if (debug) cout << "inc"<<endl;
		}
	}

	if ((diferencia!=0)and(racha.interruption)) {
		writer.write(1, 1);
	} else if (!racha.interruption) {
		writer.write(0, 1);

		int potencia = 1;

		for (int j=0;j<J[RUNindex];j++){
			potencia=potencia*2;
		}

		int resto=diferencia%potencia;
		potencia=potencia/2;

		for (int j=0;j<J[RUNindex];j++){
			writer.write(resto/potencia, 1);

			resto=resto%potencia;
			potencia=potencia/2;
		}

		ajuste = J[RUNindex];
		if (RUNindex > 0) RUNindex--;
	}

	return (ajuste+1);
}
*/

int Coder::encodeRacha2(Racha &racha, Writer &writer){
	int diferencia=racha.largo;
	int ajuste;

	while (diferencia >= (1 << J[RUNindex])){
		writer.write(1, 1);
		cantidad_bits++;
		diferencia = diferencia - (1 << J[RUNindex]);
		if (RUNindex < 31) RUNindex++;
	}

	if ((diferencia != 0) && (racha.interruption)){
		writer.write(1, 1);
		cantidad_bits++;
	}else if (!racha.interruption){
		writer.write(0, 1);
		cantidad_bits++;

		
		int resto = diferencia % (1 << J[RUNindex]);
		writer.write(resto, J[RUNindex]);
		cantidad_bits+=J[RUNindex];
		
		ajuste = J[RUNindex];
		if(RUNindex > 0) RUNindex--;
	}

	return (ajuste+1);
}

int Coder::encodeRacha2(Racha &racha, Writer2 &writer){
	int diferencia=racha.largo;
	int ajuste;

	while (diferencia >= (1 << J[RUNindex])){
		writer.write(1, 1);
		cantidad_bits++;
		diferencia = diferencia - (1 << J[RUNindex]);
		if (RUNindex < 31) RUNindex++;
	}

	if ((diferencia != 0) && (racha.interruption)){
		writer.write(1, 1);
		cantidad_bits++;
	}else if (!racha.interruption){
		writer.write(0, 1);
		cantidad_bits++;


		int resto = diferencia % (1 << J[RUNindex]);
		writer.write(resto, J[RUNindex]);
		cantidad_bits+=J[RUNindex];

		ajuste = J[RUNindex];
		if(RUNindex > 0) RUNindex--;
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
		if (contexts[contexto].C >  -(white+1)/2) contexts[contexto].C=contexts[contexto].C-1;
		if (contexts[contexto].B <= -contexts[contexto].N) contexts[contexto].B=-contexts[contexto].N+1;
	} else if (contexts[contexto].B > 0){
		contexts[contexto].B=contexts[contexto].B-contexts[contexto].N;
		if (contexts[contexto].C < (white/2)) contexts[contexto].C=contexts[contexto].C+1;
		if (contexts[contexto].B > 0) contexts[contexto].B=0;
	}
}

/*
void Coder::encode(int error, int k, Writer &writer, int racha,int ajuste){
	// Almacena en potencia el valor de 2^k
	// Calcula la parte entera del cociente entre el error y 2^k y lo guarda en "cociente" para codificación binaria
	// Calcula el resto de la división entera entre el error y 2^k y lo guarda en "resto" para codificación unaria

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
			writer.write(0, 1);
		}

		if(cociente==qMax){
		} else {
			writer.write(1, 1);
		}

		if (cociente==qMax)	{
			cociente=qMax;
			potencia=pow(2,beta-1);
			for (int j=0;j<beta;j++){
				writer.write(error/potencia, 1);
				error=error%potencia;
				potencia=potencia/2;
			}
		} else {
			//	Este loop calcula la expresión binaria del resto expresada con k bits, y lo guarda en array auxiliar bitsToFile
			for (int j=0;j<k;j++){
				writer.write(resto/potencia, 1);
				resto=resto%potencia;
				potencia=potencia/2;
			}
			// Este loop calcula la expresión unaria del cociente, con tantos ceros como la variable "cociente"
			//y lo guarda en array auxiliar bitsToFile
		}
	}
}
*/

void Coder::encode(int error, int k, Writer &writer, int racha, int ajuste){
	int qMax = (racha ? (this->qMax_ - ajuste) : (this->qMax));
	if(k<0) k=0;
	
	int cociente = error / (1 << k);
	int resto    = error % (1 << k);

	if (cociente > qMax) cociente = qMax;

	for (int j=0; j<cociente; j++) {
		writer.write(0, 1);
		cantidad_bits++;
	}
	if(cociente != qMax)           {
		writer.write(1, 1);
		cantidad_bits++;
	}

	if(cociente == qMax) {
		writer.write(error, beta);
		cantidad_bits+=beta;
	}
	else                 {
		writer.write(resto, k);
		cantidad_bits+=k;
	}
}

void Coder::encode(int error, int k, Writer2 &writer, int racha, int ajuste){
	int qMax = (racha ? (this->qMax_ - ajuste) : (this->qMax));
	if(k<0) k=0;

	int cociente = error / (1 << k);
	int resto    = error % (1 << k);

	if (cociente > qMax) cociente = qMax;

	for (int j=0; j<cociente; j++) {
		writer.write(0, 1);
		cantidad_bits++;
	}
	if(cociente != qMax)           {
		writer.write(1, 1);
		cantidad_bits++;
	}

	if(cociente == qMax) {
		writer.write(error, beta);
		cantidad_bits+=beta;
	}
	else                 {
		writer.write(resto, k);
		cantidad_bits+=k;
	}
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

Coder::pixels3D Coder::getPixels3D(int x_prev, int y_prev, int x, int y){
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

	if (x==0){
		/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
		o la mitad del valor de blanco de la imagen */
		a = 1 + (image2.white >> 1);
		c = 1 + (image2.white >> 1);
	}

	if (x==image.width-1){
		/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
		o la mitad del valor de blanco de la imagen */

		d = 1 + (image2.white >> 1);
	}

	if (y==0){
		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */

		if(b == -1) b = 1 + (image2.white >> 1);
		if(c == -1) c = 1 + (image2.white >> 1);
		if(d == -1) d = 1 + (image2.white >> 1);
	}

	if (x_prev==0){
		/* Si estoy parado en un borde izquierdo, el valor de a y c tienen que ser "128",
		o la mitad del valor de blanco de la imagen */

		a_ = 1 + (image.white >> 1);
		c_ = 1 + (image.white >> 1);
	}

	if (x_prev==image.width-1){
		/* Si estoy parado en un borde derecho, el valor de d tiene que ser "128",
		o la mitad del valor de blanco de la imagen */

		d_ = 1 + (image.white >> 1);
		f_ = 1 + (image.white >> 1);
	}

	if (y_prev==0){
		/* Si estoy en la primer fila, b y c deben ser "128"
		o la mitad del valor de blanco de la imagen */
			
			if(b_ == -1) b_ = 1 + (image.white >> 1);
			if(c_ == -1) c_ = 1 + (image.white >> 1);
			if(d_ == -1) d_ = 1 + (image.white >> 1);
	}

	if (y_prev>image.heigth-2){
		/* Si estoy en la última o penúltima fila, g debe ser "128"
		o la mitad del valor de blanco de la imagen */
			
		g_ = 1 + (image.white >> 1);
	}

	/* Para cada a, b,c y d, si no se cumple una condición de borde, y por lo tanto no hubo asignación en los if que preceden,
	se traen los valores de a, b,c y d de la imagen */

	if (a==-1)  a=image2.getPixel(x-1,y);
	if (b==-1)  b=image2.getPixel(x,y-1);
	if (c==-1)  c=image2.getPixel(x-1,y-1);
	if (d==-1)  d=image2.getPixel(x+1,y-1);
	if (a_==-1) a_=image.getPixel(x_prev-1,y_prev);
	if (b_==-1) b_=image.getPixel(x_prev,y_prev-1);
	if (c_==-1) c_=image.getPixel(x_prev-1,y_prev-1);
	if (d_==-1) d_=image.getPixel(x_prev+1,y_prev-1);
	if (e_==-1) e_=image.getPixel(x_prev,y_prev);
	if (f_==-1) f_=image.getPixel(x_prev+1,y_prev);
	if (g_==-1) g_=image.getPixel(x_prev,y_prev+1);

	
	return {a,  b,  c,  d,
		    a_, b_, c_, d_,
			e_, f_, g_};
}

void Coder::writeHeader(Writer &writer){
	/** Escribe el encabezado de la imagen codificada,
	para esto se sigue el mismo esquema presente en el archvo .pgm,
	con el agregado de escribir también el valor de Nmax
	los 5 métodos que se usan para escribir el encabezado, que se listan a continuación,
	siguen la misma estructura interna general */

	writeMagic(writer);
	writeWidth(writer,false);
	writeHeigth(writer,false);
	writeWhite(writer,false);
	writeNmax(writer);
	writeCantidadImagenes(writer);
	writeCompMov(writer,activarCompMov);
}

void Coder::writeCantidadImagenes(Writer &writer){
	/** Se lleva el valor de Nmax a un double de la forma 0,Nmax
	luego se multiplica entre 10 y se redondea para quedarse
	con cada digito de Nmax y poder escribirlos como chars */

	int nmax =cantidad_imagenes;

	string temp = str_(nmax)+"\n";
	int temp_l = temp.length();

	writer.writeString(temp.c_str(),temp_l);
	cantidad_bits+=temp_l;
}

void Coder::writeNmax(Writer &writer){
	/** Se lleva el valor de Nmax a un double de la forma 0,Nmax
	luego se multiplica entre 10 y se redondea para quedarse
	con cada digito de Nmax y poder escribirlos como chars */

	int nmax =Nmax;

	string temp = str_(nmax)+"\n";
	int temp_l = temp.length();

	writer.writeString(temp.c_str(),temp_l);
	cantidad_bits+=temp_l;
}

void Coder::writeMagic(Writer &writer){
	/** Como solo se trabaja con imagenes tipo P5,
	directamente se escriben estos caracteres*/

	writer.writeString("P5\n", 3);
	cantidad_bits+=3;
}

void Coder::writeCompMov(Writer &writer, bool compMov){
	/** Como solo se trabaja con imagenes tipo P5,
	directamente se escriben estos caracteres*/

	if (compMov) {
		writer.writeString("1\n", 2);
		cantidad_bits+=2;

		writeWidth(writer,compMov);
		writeHeigth(writer,compMov);
		writeWhite(writer,compMov);
		writeBSize(writer);
	} else {
		writer.writeString("0\n", 2);
		cantidad_bits+=2;
	}
}

void Coder::writeBSize(Writer &writer){
	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */


	string temp = str_(bsize)+"\n";
	int temp_l = temp.length();

	writer.writeString(temp.c_str(),temp_l);
	cantidad_bits+=temp_l;

}

void Coder::writeWidth(Writer &writer, bool vector){
	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int ancho (vector ? v_ancho : image.width);
	string temp = str_(ancho)+" ";
	int temp_l = temp.length();

	writer.writeString(temp.c_str(),temp_l);
	cantidad_bits+=temp_l;
}

void Coder::writeHeigth(Writer &writer, bool vector){
	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int alto (vector ? v_alto : image.heigth);

	string temp = str_(alto)+"\n";
	int temp_l = temp.length();

	writer.writeString(temp.c_str(),temp_l);
	cantidad_bits+=temp_l;
}

void Coder::writeWhite(Writer &writer, bool vector){
	/** Por descripción sobre el funcionamiento recurrir a writeNmax, es exactamente igual */

	int blanco (vector ? v_blanco : image.white);

	string temp = str_(blanco)+"\n";
	int temp_l = temp.length();

	writer.writeString(temp.c_str(),temp_l);
	cantidad_bits+=temp_l;
}

int Coder::correctPredictedValue(int pred, int contexto){

	return pred + contexts[contexto].C;
}

int Coder::getContext_(int x, int y, int lar){

	return (getPixels3D(0,0,x,y).a==getPixels3D(0,0,x+lar,y).b);
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

string Coder::str_(int n){
	stringstream ss1;
	ss1 << n;
	string n_ = ss1.str();

	return n_;
}

Coder::~Coder() {

}

} /* namespace std */
