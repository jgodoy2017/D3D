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

CHARLS_IMEXPORT(JLS_ERROR) JpegLsEncode(BYTE **buf, size_t *buf_size, size_t* pcbyteWritten, const void* uncompressedData, size_t uncompressedLength, struct JlsParameters* pparams)
{

	//VERSIÓN SÚPER SIMPLIFICADA DEL ENCODER
	
	*pcbyteWritten = 0;

	JlsParameters info = *pparams;

	Writer2* writer = new Writer2(buf);

	Coder * coder = new Coder();
	Image image_anterior_(info.heigth,info.width,255,0);
	image_anterior_.image=image_anterior;
	


	*pcbyteWritten+=coder.code_dcmtk(writer,uncompressedData,image_anterior_,&info)


	const Uint8* imagen = static_cast<const Uint8*>(uncompressedData);
	int *image_tmp=(int*)malloc(uncompressedLength*sizeof(int));

		for (int variable=0; (variable<uncompressedLength); variable++){

			image_tmp[variable]=(int)imagen[variable];

		}

	image_anterior=image_tmp;


	return OK;

	
}

}
