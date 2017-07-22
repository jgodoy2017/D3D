 /**
  @file Image.cpp
  @brief Se encarga de cargar y preparar la imagen a comprimir

  Lee la imagen completamente, separa el encabezado de los pixeles
  y prepara los datos necesarios para que pueda funcionar Coder.cpp.

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#include "Image.h"

using namespace std;

Image::Image(){

}

Image::Image(int heigth, int width) {

	//constructor

this->heigth=heigth;
this->width=width;

/* Reserva memoria para guardar toda la imagen */
image=(int*)malloc(this->width*this->heigth*sizeof(int));

}

Image::Image(string path){

	//constructor

this->path=path;
this->name="";

loadImage();

}

Image::Image(string path, string name){

	//constructor

this->path=path;
this->name=name;

loadImage();

}

void Image::setImage(){

	/** Reserva memoria para guardar toda la imagen */
	image=(int*)malloc(this->width*this->heigth*sizeof(int));
}

void Image::loadImage(){

			/** Este método carga la imagen entera en un array

			Es un array de una sola dimensión, donde se concatenan todas las filas de la imagen */

			string absolute_path=path+name;

			ifstream in;
			in.open(absolute_path.c_str(), ios::binary);

			/* Contadores para llevar la cuenta del valor de fila, columna y total de la imagen */
			int contadorW=1;
			int contadorH=1;
			int contador=0;

			char temp='1';


			setMagic(in,temp);	//recupera el valor de magic de la imagen y lo guarda en la variable "magic"
								//procedimiento análogo para el width, heigth y white, cada uno es guardado
								//en la variable correspondiente

			setWidth(in,temp);

			setHeigth(in,temp);

			setWhite(in,temp);

			image=(int*)malloc(this->width*this->heigth*sizeof(int)); //reserva memoria para guardar toda la imagen

			if(image){
				while (contadorH!=this->heigth+1){
					contadorW=1;
					while (contadorW!=this->width+1){
						in.read(&temp,1);

						string ascii="P3";

						if (this->magic.compare("P3")==0){ //comprobación innecesaria, ya se nos dice por letra
															//que todas las imágenes tendrán magic P5
							int temp_=(int)temp;
							image[contador]=temp_;

						}else{

							int temp_ =binaryToInt(temp);	//convierte a entero el valor binario de cada pixel leido
							image[contador]=temp_;	//guarda valor entero de pixel, en la correspondiente entrada del array que representa la imagen
						}

						contadorW++;
						contador++;

					}

					contadorH++;

				}
			}else{
				cout<<">> ERROR: No se pudo cargar la imagen en memoria."<<endl;
				cout<<">> Cantidad de memoria necesaria: "<<this->width*this->heigth*sizeof(int)<<" = "<<this->width<<"x"<<this->heigth<<"x"<<sizeof(int)<<" bytes."<<endl;
			}



			in.close();


}

int Image::binaryToInt(char temp){

	/** Convierte el valor binario de temp, en un entero,
	Por ejemplo, si el byte ingresado es 01010101,
	este método devuelve el entero 1+4+16+64 = 85 */

	bitset<8> temp_(temp);

	int indice=0;
	int suma=0;
	int potencia=1;

	while (indice!=8){

		suma=suma+temp_[indice]*potencia;

		potencia=potencia*2;
		indice++;
	}


	return suma;

}

void Image::setMagic(ifstream &in,char &temp){

	/** Lee el valor de magic de la imagen, en donde debería estar según el formato de las imágenes .pgm
	esto es innecesario ya que se nos dice que todas serán P5 */

		string magic="";

		in.read(&temp,1);

		magic=magic +temp;

		in.read(&temp,1);

		magic=magic+temp;

				this->magic=magic;

		in.read(&temp,1);


}

void Image::setWidth(ifstream &in,char &temp){

	/** Lee el valor de width de la imagen, en donde debería estar según el formato de las imágenes .pgm
	Va leyendo cada número que forma el width formando un decimal, luego multiplica por el orden del número
	para tener el width, por ejemplo si el valor de ancho es 512, va leyendo el 5, el 1 y el 2,
	formando primero el número 0,512, y luego multiplica por 1000, y guarda el número 512 en la variable width */

		int contador=0;
		double resultado=0.0;

		int potencia=10;

		in.read(&temp,1);

		while (temp!=' '){

			int temp_=temp-'0';

		resultado = double(resultado)+(double)temp_/(double)potencia;

		in.read(&temp,1);

		contador++;
		potencia=potencia*10;

		}

		resultado=(double)resultado*(double)potencia/10;

		this->width=round(resultado);


}void Image::setHeigth(ifstream &in,char &temp){

	/** Obtiene el Height en el encabezado.
	Funcionamiento análogo a setWidth */

	int contador=0;
	double resultado=0.0;

	int potencia=10;

	in.read(&temp,1);

	while (temp!='\n'){

		int temp_=temp-'0';

	resultado = double(resultado)+(double)temp_/(double)potencia;

	in.read(&temp,1);

	contador++;
	potencia=potencia*10;

	}

	resultado=(double)resultado*(double)potencia/10;

	this->heigth=round(resultado);


}

void Image::setWhite(ifstream &in,char &temp){

	/** Obtiene el Height en el encabezado.
	Funcionamiento análogo a setWidth */

	int contador=0;
	double resultado=0.0;

	int potencia=10;

	in.read(&temp,1);

	while (temp!='\n'){

		int temp_=temp-'0';

	resultado = double(resultado)+(double)temp_/(double)potencia;

	in.read(&temp,1);

	contador++;
	potencia=potencia*10;

	}

	resultado=(double)resultado*(double)(potencia/10);

	this->white=round(resultado);


}

bool Image::is16bit(){
	return (this->white==MAXVAL16BIT);
}

Image::~Image() {

}
