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

namespace std {


// PRUEBA !


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
	this->qMax_=Lmax-beta-1;

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

	cout << "// START CODER" << endl;
	//cout << "Tam: " << image.heigth*image.width << endl;
	for(int prox=0;prox<image.heigth*image.width;prox++){
		if (debug2) cout<<endl<<"fila: ["<<prox/image.width<<"]";
		if (debug2) cout<<"x: ["<<prox%image.width<<"]"<<endl;

		//if ((prox>-1)&&(prox<2000)) {debug2=true;
		//debug=false;
		//}else {debug=false;
		//debug2=false;
		//}

		//if (prox>22000) debug=true;
/*
		if (prox==1350) {
			cout <<"imagen: ";
			for (int aux_cont=0;aux_cont<2025;aux_cont++)
					cout <<image.image[aux_cont]<<" ";
			cout <<endl;
		}

*/
		if (debug){
			cout<<"bitsToFilePointer: "<<bitsToFilePointer<<endl;
			cout<<"bitsToFile: ";
			contador=0;
			while (contador<800){
			cout<<bitsToFile[contador];
			contador++;
		}cout<<endl;}


							//bucle principal que recorre la imagen y va codificando cada pixel

		int currentPixel=image.image[prox]; //valor del pixel actual

		pixels pxls = getPixels_(prox); //obtiene los píxeles de la vecindad: a,b y c

		//if (prox==267870) debug=true; else debug=false;

		if (debug || debug2) cout<<currentPixel<<" "<<prox<<" " << pxls.a<<" "<< pxls.b<<" "<< pxls.c<<" "<< pxls.d<<endl;
		if (debug) cout<<"[" << prox << "] " << pxls.a << " " << pxls.b << " " << pxls.c << " " << pxls.d << endl;



		//cout<<prox<<" " << pxls.a<<" "<< pxls.b<<" "<< pxls.c<<" "<< pxls.d<<endl;


		// int p = getP(pxls);	//calcula p

		grad gradients=setGradients(pxls); //calcula los gradientes

		//cout<<prox<<" " << gradients.ga<<" "<< gradients.gb<<" "<< gradients.gc<<" "<<endl;

		int signo;

		int contexto = getContext(gradients, signo);	//trae el contexto asociado a ese gradiente

		if (debug) cout<<"signo: "<<signo<<endl;
		//signo=1;

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
		//cout<<"X";
		int predicted = getPredictedValue(pxls);	//calcula el valor pixel predicho

		if (debug) cout<<predicted<<endl;

		predicted=fixPrediction(predicted,signo, contexto);

		if (debug) cout<<predicted<<endl;

		int error_s= currentPixel-predicted;	//calcula el error como la resta entre el valor actual y el valor predicho

		int error_=error_s*signo;

		if (debug) cout<<"error_= "<<error_<<endl;

		int k= getK(contexto);	//calcula k para ese contexto

		if (debug) cout<<"k= "<<k<<endl;



		int error__=reduccionDeRango(error_);

		if (debug) cout<<"signo: "<<signo<<" reduccion: "<<(error__==error_)<<endl;

	//	int error__=error_;

		if (debug) cout<<"error con reducción de rango= "<<error__<<endl;

		int error =rice(error__, get_s(contexto),k);	//devuelve mapeo de rice del error

		maximo_=max(maximo_,error);



		//if (maximo_==258) cout<<prox<<endl;

		if (debug) cout<<"error= "<<error<<endl;

		encode(error,k, salida,0,0);	//codifica el error

		updateContexto(contexto, error_);	//actualiza los valores para el contexto

		}

		else {

			int interruption=0;
			int largo= getRachaParams2(image, prox, pxls.a, interruption);
			//cout<<"largo= "<<largo<<endl;
//			int contexto=(pxls.a==pxls.b);
			int contexto=getContext_(prox, largo);

			Racha racha(largo, interruption, pxls.a,contexto);
			if (debug2) cout<<"> RACHA: largo:   "<<racha.largo<<". Codifico el largo (parte unaria de la racha). Inicio con kr="<<RUNindex<<endl;
			if (debug) cout<<prox<<" "<<largo<<" "<<interruption<<" "<<pxls.a<<" "<<contexto<<endl;
			if (debug) cout<<"actual: "<<image.image[prox]<<endl;

			int cantidad_unos=encodeRacha2(racha);

			if (debug){
				cout<<"bitsToFilePointer: "<<bitsToFilePointer<<endl;
				cout<<"bitsToFile: ";
				contador=0;
				while (contador<800){
				cout<<bitsToFile[contador];
				contador++;
			}cout<<endl;}

			encodeMuestraInterrupcion(racha, image.image[prox+racha.largo], prox+racha.largo,salida,cantidad_unos);

			racha.updateContexto();

			prox=prox+largo;

			if (debug || debug2) cout<<"prox= "<<prox<<endl;

			//if ((racha.interruption))	prox--;	// Esto ya no sería válido


		}

		if (debug) cout<<endl;
	}

	flushEncoder(salida);	//termina de escribir los últimos bits que hayan quedado en el array de bits

	salida.close();

	cout<<maximo_<<endl;

	 ifstream in1(path_salida.c_str(), std::ifstream::ate | std::ifstream::binary);
	 ifstream in2(image.path.c_str(), std::ifstream::ate | std::ifstream::binary);

	 cout<<"nmax= "<<nmax<<" entrada: "<< in2.tellg()<<" salida: "<< in1.tellg()<<" ratio: "<< float(in1.tellg())/float(in2.tellg())<<endl;

	 in1.close();
	 in2.close();

}

 int Coder::max(int uno, int dos){

	 if (uno<dos) return dos;
	 else return uno;

 }float Coder::get_s(int contexto){

		return float(float(contexts[contexto].B)/float(contexts[contexto].N)); //es N o N_?
	}

 int Coder::reduccionDeRango(int error){

	 if (error>127){

		 		error=error -256;
		 		if (debug) cout<<"error + predicted>255"<<endl;

		 	}else if (error<-128){

		 		error=256+error;
		 		if (debug) cout<<"error + predicted<0"<<endl;

		 	}else {
		 		if (debug) cout<<"return error"<<endl;
		 		return error;
		 	}

 	return error;
 }



int Coder::getKPrime(Racha &r){
	int T_racha, K_racha;

	//cout<<">> RACHA CNTX="<<r.contexto<<" A="<<cntx[r.contexto].A_racha<<" N="<<cntx[r.contexto].N_racha<<" Nn="<<cntx[r.contexto].Nn_racha;

	// Calculo la variable T para determinar k de Golomb.
	T_racha=((r.contexto==1) ? cntx[r.contexto].A_racha : cntx[r.contexto].A_racha + cntx[r.contexto].N_racha/2);   
	
	if (debug) cout<<"T_racha: "<<T_racha<<endl;

	//for(K_racha=0; (cntx[r.contexto].N_racha<<K_racha)<T_racha; K_racha++){
	for(K_racha=0; (cntx[r.contexto].N_racha*pow(2,K_racha))<T_racha; K_racha++){
/*
		if (debug){


			cout<<"K_racha: "<<K_racha<<endl;
			cout<<"cntx[r.contexto].N_racha: "<<cntx[r.contexto].N_racha<<endl;
			cout<<"cntx[r.contexto].N_racha*pow(2,K_racha): "<<cntx[r.contexto].N_racha*pow(2,K_racha)<<endl;



		}
*/
		// k = min{k' / 2^(k')*N >= T}
	}//if (debug)		cout<<"K_racha_sale: "<<K_racha<<endl;


	//cout<<" T="<<T_racha<<" K="<<K_racha<<endl;
	
	return K_racha;
}
void Coder::encodeMuestraInterrupcion(Racha &racha, int siguiente, int prox_, ofstream &salida,int cantidad_unos){
	//racha, image.image[prox+racha.largo], prox+racha.largo,salida,cantidad_unos

	int error=0, error_=0, kPrime=1000;


	if (!racha.interruption){
	if (debug2) cout<<"GOLOMB - PIXEL INTERRUPCION"<<endl;
	error_=siguiente-getPixels_(prox_).b;

	kPrime=getKPrime(racha);

	if (debug || debug2) cout<<"k: "<<kPrime<<endl;

	error_=reduccionDeRango(error_);

	error=rice_rachas(error_,racha.contexto,kPrime);
	//cout<<"I"<<endl;

	updateContexto_(racha.contexto, error_);
	encode(error, kPrime, salida,1,cantidad_unos);

	//encode(error, kPrime, salida,0,cantidad_unos);
	}
	else{

		writeCode(salida);
	}

	if (debug) cout<<"error: "<<error<<" "<<error_<<endl;


}

int Coder::rice_rachas(int error,int contexto, int k){

//cntx[contexto]

	int map=0;

	if ((k!=0)and(error<0))	map=1;

	if ((k==0)and(error<0)and((float(cntx[contexto].Nn_racha)/float(cntx[contexto].N_racha))>=0.5))	map=1;

	if ((k==0)and(error>0)and((float(cntx[contexto].Nn_racha)/float(cntx[contexto].N_racha))<0.5))	map=1;

	int retorno=2*abs(error)-contexto-map;

	 return retorno;

}

int Coder::encodeRacha2(Racha &racha){
	int cantidad_unos=0;
	int diferencia=racha.largo;
	int ajuste=RUNindex;

	if (debug2) cout<<" Codificacion de la racha: ";
	while (diferencia >= (1 << J[RUNindex])) {
	   //Agregar un 1 al tream
		cantidad_unos++;
		if (debug2) cout<<"1";
		bitsToFile[bitsToFilePointer]=1;
		bitsToFilePointer++;
	   //RUNcnt = RUNcnt - 2^J[RUNIndex]
		diferencia = diferencia - (1 << J[RUNindex]);
	   //Si RUNindex < 31, RUNindex++
		if (RUNindex < 31) {
			RUNindex++;
		}
	}
	ajuste = RUNindex;
	if (racha.interruption) {
		bitsToFile[bitsToFilePointer]=1;
		bitsToFilePointer++;
		if (debug2) cout<<"-1-";
		cantidad_unos++;
	} else {
		bitsToFile[bitsToFilePointer]=0;
		if (debug2) cout<<"-0-";
		bitsToFilePointer++;
		int potencia = 1;
		for (int j=0;j<J[RUNindex];j++){
			potencia=potencia*2;
		}
		int cociente=diferencia/potencia;
		int resto=diferencia%potencia;
		if (debug)	cout<<diferencia<<" "<<cociente<<" "<<resto<<endl;
		potencia=potencia/2;
		for (int j=0;j<J[RUNindex];j++){
			bitsToFile[bitsToFilePointer]=resto/potencia;
			if (debug2) cout<<resto/potencia;
			if (debug) cout<<resto/potencia;
			bitsToFilePointer++;
			resto=resto%potencia;
			potencia=potencia/2;
		}
		ajuste = RUNindex;
		if (RUNindex > 0) RUNindex--;
	}
	if (debug2) cout<<" Valores finales para la racha: kr:   "<<RUNindex<<" J[kr]:   "<<J[RUNindex]<<endl;


	return (ajuste+1);
}

int Coder::encodeRacha(Racha &racha){

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

	if ((diferencia==0)&&(racha.interruption)){
	}else{

		bitsToFile[bitsToFilePointer]=racha.interruption;
		if (debug) cout<<bitsToFile[bitsToFilePointer]<<endl<<"bitsToFilePointer= "<<bitsToFilePointer<<endl;

		bitsToFilePointer++;}

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


	if (debug) cout<<"J[kr]+1= "<<J[kr]+1<<endl;



	return (J[kr]+1);

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


	//if(prox+largo>=image.heigth*image.width) largo=image.heigth*image.width-prox-1; //Me protejo de las rachas al final de la imagen.
	return largo;
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


int Coder::fixPrediction(int predicted,int signo, int contexto){

	predicted=predicted+(contexts[contexto].C*signo);

	if (predicted< 0) predicted=0;
	if (predicted >255) predicted=255;

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

void Coder::encode(int error, int k, ofstream &salida, int racha,int ajuste){

		/** Almacena en potencia el valor de 2^k
		Calcula la parte entera del cociente entre el error y 2^k y lo guarda en "cociente" para codificación binaria
		Calcula el resto de la división entera entre el error y 2^k y lo guarda en "resto" para codificación unaria */

	int qMax;

	if (racha) {
		qMax=this->qMax_;
		qMax=qMax-ajuste;
	}
	else {
		qMax=this->qMax;
	}
	if (debug2)cout<<"ajuste: "<<ajuste<<" qMax: "<<qMax<<endl;
	if (k!=1000){

	int potencia = floor(pow(2,k));
	if (potencia==0)	potencia=1;

	int cociente=error/potencia;
	int resto=error%potencia;

	potencia=potencia/2;

	if (cociente>=qMax)	{

		cociente=qMax;
	}
	if (debug) cout<<"cociente=qMax: "<<(cociente==qMax)<<endl;

	for (int j=0;j<cociente;j++){

			bitsToFile[bitsToFilePointer]=0;
			if (debug) cout<<0;
			if (debug2) cout<<0;
			bitsToFilePointer++;

		}
		/*	para indicar el fin del código de la parte unaria escribe un 1 al final */
	if(cociente==qMax){

	}else{
	//}
			bitsToFile[bitsToFilePointer]=1;
		if (debug)cout<<1;
		if (debug2)cout<<1;
		bitsToFilePointer++;
	}
	if (cociente==qMax)	{

		cociente=qMax;

		potencia=pow(2,beta-1);

		for (int j=0;j<8;j++){

					bitsToFile[bitsToFilePointer]=error/potencia;
					if (debug) cout<<error/potencia;

					bitsToFilePointer++;
					if (debug2)cout<<error/potencia;
					error=error%potencia;

					potencia=potencia/2;

				}
	}
	else{
	/*	Este loop calcula la expresión binaria del resto expresada con k bits, y lo guarda en array auxiliar bitsToFile */
	for (int j=0;j<k;j++){

			bitsToFile[bitsToFilePointer]=resto/potencia;
			if (debug) cout<<resto/potencia;
			if (debug2) cout<<resto/potencia;
			bitsToFilePointer++;

			resto=resto%potencia;

			potencia=potencia/2;

		}
	/* Este loop calcula la expresión unaria del cociente, con tantos ceros como la variable "cociente"
	y lo guarda en array auxiliar bitsToFile */

	}



	}if (debug)	cout<<endl;

	if (debug){
		cout<<"bitsToFilePointer: "<<bitsToFilePointer<<endl;
		cout<<"bitsToFile: ";
		contador=0;
		while (contador<800){
		cout<<bitsToFile[contador];
		contador++;
	}cout<<endl;}

	writeCode(salida);

	if (debug){
		cout<<"bitsToFilePointer: "<<bitsToFilePointer<<endl;
		cout<<"bitsToFile: ";
		contador=0;
		while (contador<800){
		cout<<bitsToFile[contador];
		contador++;
	}cout<<endl;}

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
			if (debug) cout<<"salida a archivo: ";
			for(int j=0;j<8;j++){

			temp_b[7-j]=bitsToFile[k*8+j];
			if (debug)	cout<<temp_b[7-j];
			}//for j
			if (debug)cout<<" termina salida a archivo"<<endl;

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


int Coder::rice(int error, float s, int k){

	/** Mapeo de rice del error */

	/** falta reducción de rango para el error ***/

	if ((k<=0)and(s<=-0.5)){

		return rice(-error-1,0,1);	//en este caso llama al mapeo común de rice, con otro parámetro
	}
	else{

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

int Coder::getContext(grad gradients, int &signo){

	/** Determina el contexto
	Todos los contextos posibles se organizan en un array, donde cada elemento del array representa un contexto,
	es posible definir un mapeo entre el espacio de todos los contextos posibles y los enteros,
	para que dado un contexto haya una relación biunívoca con un elemento del array */

	signo=1;

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

		if(contga<4){
			contga=8-contga;
			contgb=8-contgb;
			contgc=8-contgc;

			signo=-1;
		}

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

	// Devuelve el valor de p, según expresión de las diapositivas del curso

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

	}	else if ((current%image.width)==0){

		/* columna izquierda */

		a=image.image[current-image.width];
		c=getPixels_(current-image.width).a;


	}

	if (current<image.width){

			//primer fila

			if (b==-1) b=0;
			if (c==-1) c=0;
			if (d==-1) d=0;
		}




	else if ((current%image.width)==image.width-1){

		/* columna derecha */

		d=image.image[current-image.width];


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

int Coder::getContext_(int pos, int lar){
	return (getPixels_(pos).a==getPixels_(pos+lar).b);
}

void Coder::updateContexto_(int c, int err){

	if(cntx[c].N_racha>RESET) {
			cntx[c].reset();
			//cout << "RESET" << endl;
		}

	cntx[c].updateA(err);
	cntx[c].updateNn(err);

	cntx[c].updateN();
}

Coder::~Coder() {

}

} /* namespace std */
