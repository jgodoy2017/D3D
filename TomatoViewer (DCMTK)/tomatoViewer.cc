#include "dcmtk/config/osconfig.h"    

#include <iostream>
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING

// Includes para DCMTK
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      
#include "dcmtk/dcmimgle/dcmimage.h"    // Para DicomImage  
#include "dcmtk/dcmimage/diregist.h"    // Creo que lo podria sacar, ya que solo vamos a trabajar con monocromos.
#include "dcmtk/dcmd3d/djlsutil.h"   
#include "dcmtk/dcmd3d/djdecode.h"

// Includes para X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "X11/WaitForKey.h"
#include "X11/X11Operations.h"

#ifdef WITH_ZLIB
#include <zlib.h>      
#endif

#ifdef USE_LICENSE_FILE
#include "oflice.h"
#endif

#ifndef OFFIS_CONSOLE_APPLICATION
#define OFFIS_CONSOLE_APPLICATION "dcmpd3d"
#endif

static OFLogger dcmdd3dLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[]){
	const char *opt_ifname = argv[1];

	E_TransferSyntax    opt_oxfer                = EXS_LittleEndianExplicit;
	E_GrpLenEncoding    opt_oglenc               = EGL_recalcGL;
	E_EncodingType      opt_oenctype             = EET_ExplicitLength;
	E_PaddingEncoding   opt_opadenc              = EPD_noChange;
	OFCmdUnsignedInt    opt_filepad              = 0;
	OFCmdUnsignedInt    opt_itempad              = 0;
	E_FileReadMode      opt_readMode             = ERM_autoDetect;
	E_FileWriteMode     opt_writeMode            = EWM_fileformat;
	E_TransferSyntax    opt_ixfer                = EXS_Unknown;
	JLS_UIDCreation     opt_uidcreation          = EJLSUC_default;
	OFBool              opt_ignoreOffsetTable    = OFFalse;

#ifdef USE_LICENSE_FILE
LICENSE_FILE_DECLARATIONS
#endif

	int command = 0, currentFrame = 0, oldFrame = -1, bpp, lWhite, lMinVal, lMaxVal;
	double minVal, maxVal;
	double &rMinVal = minVal, &rMaxVal = maxVal;
	char *text_string = new char[100];
	char *font_name   = new char[100];
	
	Window w;
	GC gc;
	Display *dpy = XOpenDisplay(0);
	Colormap colormap = DefaultColormap(dpy, 0);

	// Obtengo la resolucion de la pantalla.
	Screen *scrn = DefaultScreenOfDisplay(dpy);
	int width  = scrn->width;
	int height = scrn->height;
	std::cout << "Resolucion: " << width << "x" << height << std::endl;
	
	// Obtengo los unicos dos colores predefinidos en X11, blanco y negro.
	int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
	int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	
	// Defino algunos colores que voy a usar manualmente.
	XColor black_col, white_col, red_col;

	char black_bits[] = "#000000";
	char white_bits[] = "#FFFFFF";
	char red_bits[]   = "#FF0000";

	XParseColor(dpy, colormap, black_bits, &black_col);
	XParseColor(dpy, colormap, white_bits, &white_col);
	XParseColor(dpy, colormap, red_bits,   &red_col  );

	XAllocColor(dpy, colormap, &black_col);
	XAllocColor(dpy, colormap, &white_col);
	XAllocColor(dpy, colormap, &red_col  );
	
	DD3DDecoderRegistration::registerCodecs(opt_uidcreation, opt_ignoreOffsetTable);
	DicomImage *image = new DicomImage(opt_ifname);
	
	if ((image) && (image->getStatus() == EIS_Normal)){
		unsigned long numberOfFrames = image->getNumberOfFrames();
		unsigned long bytesOfFrame   = image->getOutputDataSize(0);
		unsigned long lWidth  = image->getWidth();
		unsigned long lHeight = image->getHeight();
		
		printf("Dimensiones del stack: %lux%lux%lu frames.\n", lWidth, lHeight, numberOfFrames);
				
		while(command != 'q'){   // Salimos con "q".
			if(currentFrame != oldFrame){
				if(image->getDepth() <= 8){
					Uint8 *pixelData = new Uint8[bytesOfFrame];

					bpp = 8;
					lWhite = (1 << bpp) - 1;
					if(!command) printf("BPP del stack: %d (blanco=%d). bytes=%d\n", bpp, lWhite, bytesOfFrame);
				
					image->getMinMaxValues(rMinVal, rMaxVal, 0);
					lMinVal = static_cast<int>(minVal);
					lMaxVal = static_cast<int>(maxVal);

					if(image->getOutputData(pixelData, bytesOfFrame, 0, currentFrame, 0)){
						// Creo la ventana. Los numeros son: (x, y) de la esquina superior derecha, (ancho, largo), ancho del borde.
						w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), (width - lWidth)/2, (height-lHeight)/2, lWidth, lHeight, 4, whiteColor, blackColor);
												   
						// Necesito capturar el "evento" que indica que se termino de dibujar la ventana ("MapNotify").
						// Para eso, hay que "registrar" el evento primero.
						XSelectInput(dpy, w, StructureNotifyMask);

						// "Mapeo" la ventana, es decir, la hago aparecer en pantalla.
						XMapWindow(dpy, w);

						// "Graphics Context". Objeto para mantener los parametros graficos en uso.
						gc = XCreateGC(dpy, w, 0, 0);

						int depth = DefaultDepth(dpy, DefaultScreen(dpy));
						if(!command) printf("Profundidad de bits RGB: %d\n", depth);
					
						int *rawData = new int[bytesOfFrame];
						for(unsigned long currentPixel = 0; currentPixel < bytesOfFrame; currentPixel++){
							// Fantastica conversion Uint8 --> int.
							rawData[currentPixel] = (pixelData[currentPixel] & 0xFF);
						}
					
						// Dibujo el frame actual en pantalla.
						std::X11Operations::drawImage(dpy, w, gc, lWidth, lHeight, depth, rawData, lMinVal, lMaxVal, lWhite);
					
						printf("Frame [%d/%d] (%d bits).\n", currentFrame + 1, numberOfFrames, bpp);
					}				
				}else{
					Uint16 *pixelData = new Uint16[bytesOfFrame];
		
					bpp = 16;
					lWhite = (1 << bpp) - 1;
					if(!command) printf("BPP del stack: %d (blanco=%d). bytes=%d\n", bpp, lWhite, bytesOfFrame);		

					image->getMinMaxValues(rMinVal, rMaxVal, 0);
					lMinVal = static_cast<int>(minVal);
					lMaxVal = static_cast<int>(maxVal);

					if(image->getOutputData(pixelData, bytesOfFrame, image->getDepth(), currentFrame, 0)){
						// Creo la ventana. Los numeros son: (x, y) de la esquina superior derecha, (ancho, largo), ancho del borde.
						w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), (width - lWidth)/2, (height-lHeight)/2, lWidth, lHeight, 4, whiteColor, blackColor);
												   
						// Necesito capturar el "evento" que indica que se termino de dibujar la ventana ("MapNotify").
						// Para eso, hay que "registrar" el evento primero.
						XSelectInput(dpy, w, StructureNotifyMask);

						// "Mapeo" la ventana, es decir, la hago aparecer en pantalla.
						XMapWindow(dpy, w);

						// "Graphics Context". Objeto para mantener los parametros graficos en uso.
						gc = XCreateGC(dpy, w, 0, 0);

						int depth = DefaultDepth(dpy, DefaultScreen(dpy));
						if(!command) printf("Profundidad de bits RGB: %d\n", depth);
						
						int *rawData = new int[lWidth * lHeight];
						for(unsigned long currentPixel = 0; currentPixel < lWidth * lHeight; currentPixel++){
							// Fantastica conversion a Uint16 --> int.
							rawData[currentPixel] = ((pixelData[currentPixel] & 0xFF00) | (pixelData[currentPixel] & 0x00FF));  
						}
								
						// Dibujo el frame actual en pantalla.
						std::X11Operations::drawImage(dpy, w, gc, lWidth, lHeight, depth, rawData, lMinVal, lMaxVal, lWhite);
					
						printf("Frame [%d/%d] (%d bits).\n", currentFrame + 1, numberOfFrames, bpp);
					}
				}
			}

			// Estampo el numero de frame y el total de frames sobre la imagen.
			sprintf(text_string, "[%d/%d]", currentFrame + 1, numberOfFrames);
			int xPos = (lWidth / 2) - 10;
			int yPos = (19 * lHeight) / 20;
			int size = ((lWidth * lHeight < 25000) ? 12 : 20);
			sprintf(font_name, "*-helvetica-*-%d-*", size);
			std::X11Operations::drawText(dpy, w, gc, font_name, text_string, red_col, xPos, yPos);

			oldFrame = currentFrame;
			command = std::WaitForKey::now();
			switch(command){
				case 3:   // Flecha derecha
					if(currentFrame + 1 < numberOfFrames){
						currentFrame++;
						printf("[+] ");
					}
					break;
				case 4:   // Flecha izquierda
					if(currentFrame > 0){
						currentFrame--;
						printf("[-] ");
					}
					break;
			}
		}
	}
	
	delete image;
	DD3DDecoderRegistration::cleanup();
	printf("Goodbye my friend.\n");	
	return 0;
}
