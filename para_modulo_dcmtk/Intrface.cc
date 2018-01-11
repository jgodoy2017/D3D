#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <bitset>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <dirent.h>


#include "Coder.h"
#include "Decoder.h"
#include "Image.h"
#include "Writer2.h"

#include "util.h"
#include "intrface.h"




extern "C"
{

int *image_anterior=NULL;
int *image_anterior_deco=NULL;

bool primeraImagen=true;
bool primeraImagenDeco=true;



CHARLS_IMEXPORT(JLS_ERROR) JpegLsEncode(BYTE **buf, size_t *buf_size, size_t* pcbyteWritten, const void* uncompressedData, size_t uncompressedLength, struct JlsParameters* pparams)
{

	//VERSIÓN SÚPER SIMPLIFICADA DEL ENCODER
	
	*pcbyteWritten = 0;

	JlsParameters info = *pparams;

	Writer2* writer = new Writer2(buf);



	Coder * coder = new Coder();
	Image image_anterior_(info.height,info.width,255,0);
	image_anterior_.image=image_anterior;
	
	//cout<<uncompressedLength<<endl;

	*pcbyteWritten+=coder->code_dcmtk(*writer,uncompressedData,image_anterior_,&*pparams,primeraImagen);
	primeraImagen=false;


	const Uint8* imagen = static_cast<const Uint8*>(uncompressedData);
	int *image_tmp=(int*)malloc(uncompressedLength*sizeof(int));

		for (int variable=0; (variable<uncompressedLength); variable++){

			image_tmp[variable]=(int)imagen[variable];

		}

	image_anterior=image_tmp;

	//cout << *pcbyteWritten<< endl;
	return OK;

	
}

CHARLS_IMEXPORT(JLS_ERROR) JpegLsDecode(void* uncompressedData, size_t uncompressedLength, const void* compressedData, size_t compressedLength,JlsParameters* info)
{
	


	//VERSIÓN SÚPER SIMPLIFICADA DEL DECODER

	//JlsParameters info = *info;

	Writer2* writer = new Writer2(uncompressedData,1);

	Decoder * decoder = new Decoder();

	Image image_anterior_(info->height,info->width,255,0);
	image_anterior_.image=image_anterior_deco;

	
	decoder->decode_dcmtk(*writer,compressedData,image_anterior_,compressedLength,info,primeraImagenDeco);

	if (primeraImagenDeco) primeraImagenDeco=false;


	const Uint8* imagen = static_cast<const Uint8*>(uncompressedData);
	int *image_tmp=(int*)malloc(info->height*info->width*sizeof(int));

		for (int variable=0; (variable<info->height*info->width); variable++){

			image_tmp[variable]=(int)imagen[variable];

		}

	image_anterior_deco=image_tmp;


	return OK;

}


CHARLS_IMEXPORT(JLS_ERROR) JpegLsReadHeader(const void* compressedData, size_t compressedLength, JlsParameters* pparams)
{
/*
 * primeras tres filas de la imagen son el encabezado,
 * las leo, formo pparams, y muevo el puntero a la imagen
 *
 */

	int contador=0;
	const Uint8* metadata = static_cast<const Uint8*>(compressedData);

	int* entrada=(int*)malloc(10*sizeof(int));
	for (int e=0;e<10;e++)	entrada[e]=0;
	int pEntrada=0;




	int int_leido=(int)metadata[contador];
	char char_leido=(char)(int_leido);
	int int_leido_d=char_leido-'0';



	//cout <<char_leido<<" "<<int_leido_d;
	//cout <<"("<< int_leido<<","<<char_leido<<")";
	contador++;

	while(char_leido!='\n'){

		while(char_leido!=' '){

		entrada[pEntrada]=entrada[pEntrada]*10+int_leido_d;


		int_leido=(int)metadata[contador];
		char_leido=(char)int_leido;
		int_leido_d=char_leido-'0';

		contador++;


		}//cout <<entrada[pEntrada]<<endl;
		//cout <<"("<< int_leido<<","<<char_leido<<")";
		pEntrada++;

		int_leido=(int)metadata[contador];
		char_leido=(char)int_leido;
		int_leido_d=char_leido-'0';

		contador++;


	}



	pparams->width=entrada[0];
	pparams->height=entrada[1];
	pparams->components=entrada[2];
	pparams->bitspersample=entrada[3];
	pparams->bytesperline=entrada[4];
	pparams->components=entrada[5];
	pparams->allowedlossyerror=entrada[6];
	pparams->ilv=(enum interleavemode)entrada[7];
	pparams->colorTransform=entrada[8];
	pparams->outputBgr=entrada[9]+'0';

	//for (int e=0;e<10;e++)	cout <<entrada[e]<<" ";

	contador++;
	pEntrada=0;
	for (int e=0;e<10;e++)	entrada[e]=0;

	int_leido=(int)metadata[contador];
	char_leido=(char)int_leido;
	int_leido_d=char_leido-'0';
	//cout <<"("<< int_leido<<","<<char_leido<<")";

	while(char_leido!='\n'){

		while(char_leido!=' '){


			entrada[pEntrada]=entrada[pEntrada]*10+int_leido_d;


			int_leido=(int)metadata[contador];
			char_leido=(char)int_leido;
			int_leido_d=char_leido-'0';

			contador++;

		}
		//cout <<"("<< int_leido<<","<<char_leido<<")";
		pEntrada++;

		int_leido=(int)metadata[contador];
		char_leido=(char)int_leido;
		int_leido_d=char_leido-'0';

		contador++;
	}

	//cout <<"ok"<<endl;

	//cout <<endl;

	pparams->custom.MAXVAL=entrada[0];
	pparams->custom.T1=entrada[1];
	pparams->custom.T2=entrada[2];
	pparams->custom.T3=entrada[3];
	pparams->custom.RESET=entrada[4];

	//for (int e=0;e<5;e++)	cout <<entrada[e]<<" ";

	contador++;
		pEntrada=0;
		for (int e=0;e<10;e++)	entrada[e]=0;

		int_leido=(int)metadata[contador];
		char_leido=(char)int_leido;
		int_leido_d=char_leido-'0';

		//cout <<"("<< int_leido<<","<<char_leido<<")";

	while(char_leido!='\n'){

		while(char_leido!=' '){

			entrada[pEntrada]=entrada[pEntrada]*10+int_leido_d;


			int_leido=(int)metadata[contador];
			char_leido=(char)int_leido;
			int_leido_d=char_leido-'0';

			contador++;

		}
		//cout <<"("<< int_leido<<","<<char_leido<<")";
		pEntrada++;

		int_leido=(int)metadata[contador];
		char_leido=(char)int_leido;
		int_leido_d=char_leido-'0';

		contador++;
	}

	//cout <<"ok"<<endl;
	//cout <<endl;

	pparams->jfif.Ver=entrada[0];
	pparams->jfif.units=entrada[1]+'0';
	pparams->jfif.XDensity=entrada[2];
	pparams->jfif.Xthumb=entrada[3];
	pparams->jfif.Ythumb=entrada[4];

	//for (int e=0;e<5;e++)	cout <<entrada[e]<<" ";

	//for (int e=0;e<compressedLength;e++)	cout <<(int)metadata[e]<<" ";





	return OK;
}

}
