#include <iostream>
#include <sstream>
#include <cstring>
#include <ctime>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>  
#include <stdio.h>
#include "Reader.h"
#include "MiniReader.h"
#include "WaitForKey.h"
#include "X11Operations.h"


using namespace std;

int main(int argc, char* argv[]){
	int lWidth, lHeight, lWhite, lnBits, lMinVal, lMaxVal, minFrame, maxFrame, frame, numFrames, oldFrame=0, command=0;
	char *text_string = new char[100];
	char lFileName[1000];
	clock_t begin, end;
	Window w;
	GC gc;
	MiniReader *miniReader;

	minFrame = atoi(argv[2]);
	maxFrame = atoi(argv[3]);	
	frame = minFrame;
	numFrames = maxFrame + 1 - minFrame;
	
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

	
	while(command != 'q'){   // Salimos con "q".
		sprintf(lFileName, "%s%d.pgm", argv[1], frame);
		miniReader = new MiniReader(lFileName, &lWidth, &lHeight, &lWhite, &lnBits);
		int *image = new int[lWidth * lHeight];
	
		if(!command){
			std::cout << "Dimensiones del stack: " << lWidth << "x" << lHeight << "x" << numFrames << " frames." << std::endl;
			std::cout << "BPP del stack: " << lnBits << " (blanco=" << lWhite << ")" << endl;			
		}
	
		image = miniReader->loadImage();
		miniReader->getMinMax(&lMinVal, &lMaxVal);
	
		if(frame != oldFrame){   // Si hay cambio de frame...
			// Creo la ventana. Los numeros son: (x, y) de la esquina superior izquierda, (ancho, largo), ancho del borde.
			w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), (width - lWidth)/2, (height-lHeight)/2, lWidth, lHeight, 4, whiteColor, blackColor);
												   
			// Necesito capturar el "evento" que indica que se termino de dibujar la ventana ("MapNotify").
			// Para eso, hay que "registrar" el evento primero.
			XSelectInput(dpy, w, StructureNotifyMask);

			// "Mapeo" la ventana, es decir, la hago aparecer en pantalla.
			XMapWindow(dpy, w);

			// "Graphics Context". Objeto para mantener los parametros graficos en uso.
			gc = XCreateGC(dpy, w, 0, 0);

			int depth = DefaultDepth(dpy, DefaultScreen(dpy));
			if(!command) std::cout << "Profundidad de bits RGB: " << depth << endl;

			// Dibujo el frame actual en pantalla.
			begin = clock();
			X11Operations::drawImage(dpy, w, gc, lWidth, lHeight, depth, image, lMinVal, lMaxVal, lWhite);
			end = clock();
			
			double millis = (static_cast<double>(1000 * (end - begin))) / CLOCKS_PER_SEC;
			std::cout << "Frame " << frame << " @ " << millis << "ms" << std::endl;
		}
		
		// Estampo el numero de frame y el total de frames sobre la imagen.
		sprintf(text_string, "[%d/%d]", frame, numFrames);
		X11Operations::drawText(dpy, w, gc, "*-helvetica-*-20-*", text_string, red_col, lWidth - 80, lHeight - 20);
		
		// Espero la respuesta del usuario desde el teclado y decido que hacer.
		oldFrame = frame;
		command = WaitForKey::now();
		switch(command){
			case 3:   // Flecha derecha
				if(frame < maxFrame){
					frame++;
					std::cout << "[+] ";
				}
				break;
			case 4:   // Flecha izquierda
				if(frame > minFrame){
					frame--;
					std::cout << "[-] ";
				}
				break;
		}
	}
		
	std::cout << "Goodbye my friend." << std::endl;	
	return(0);
}
