#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "X11/X11Operations.h"       // Version para incluir en DCMTK (!)

namespace std{

void X11Operations::drawImage(Display* dpy, Window w, GC gc, int lWidth, int lHeight, int depth, int* image, int lMinVal, int lMaxVal, int lWhite){
	// Para acelerar el renderizado, voy al driver de pantalla (!)
	
	
	// Cargo la informacion cruda de los pixeles.
	unsigned char* rawData = new unsigned char[4 * lWidth * lHeight];
	for(int px = 0; px < lWidth * lHeight; px++){
		
		// Interpolacion lineal + escalado (por temas visuales). -- En DCMTK este mapeo visual ya viene hecho, si lo hacemos de nuevo, se rompe la imagen (!)
		//float mappedValue = (lWhite/(lMaxVal-lMinVal)) * (image[px]-lMinVal);  
		
		// La version para DCMTK va sin mapeo.
		float mappedValue = static_cast<float>(image[px] % lWhite);
		
		unsigned char clippedValue = static_cast<unsigned char>(mappedValue);

		rawData[4*px]   = clippedValue;  // R    +-----------+-----------+-----------+-----------+    Estructura del pixel para video.
		rawData[4*px+1] = clippedValue;  // G    |    RED    |   GREEN   |    BLUE   |   ALPHA   |    TrueColor: RGBA 8+8+8+8
		rawData[4*px+2] = clippedValue;  // B    +-----------+-----------+-----------+-----------+    X11.
		rawData[4*px+3] = 0x00;          // A    <- 8 bits -><- 8 bits -><- 8 bits -><- 8 bits ->
	}
	
	// Creo la estructura de imagen, del lado del cliente.
	// ZPixmap indica que es interleaved: [... | R G B A | R G B A | R G B A | ...]
	// XYPixmap indica que es planar:     [... | R R R R ... G G G G ... B B B B ... A A A A ...]
	// Usamos reinterpret_cast porque necesitamos ignorar el bit de signo, pero aun asi guardar el rawData como char* (asi esta en la API).
	// lWidth y lHeight estan en pixeles.
	// depth esta en bits para todos los canales que soporte la ventana donde se va a renderizar: bpp=depth/8.
	// 32 es el largo de la estructura de datos del pixel, en bits.
	XImage *img = XCreateImage(dpy, NULL, depth, ZPixmap, 0, reinterpret_cast<char*>(rawData), lWidth, lHeight, 32, 0);
	
	// Creo la estructuda del lado del server.
	Pixmap pixmap_id = XCreatePixmap(dpy, w, lWidth, lHeight, depth);
	
	// Transfiero la imagen del cliente al server.
	// Las caracteristicas de video estan agrupadas en el contexto gc.
	XPutImage(dpy, pixmap_id, gc, img, 0, 0, 0, 0, lWidth, lHeight);
	
	// Libero la memoria del lado del cliente.
	XDestroyImage(img);
				
	// Despliego en la ventana la copia de la imagen que esta en el server.
	XCopyArea(dpy, pixmap_id, w, gc, 0, 0, lWidth, lHeight, 0, 0);
	
	// Habilito la interrupcion de exposure (pedido de renderizado al driver).
	XSelectInput(dpy, w, ExposureMask);
	
	// Disparo el exposure. Escribe en el area de memoria mapeada de la pantalla.
	XMapWindow(dpy, w);
	
	// Fuerzo una sincronizacion inmediata de la pantalla fisica.
	XSync(dpy, 0);
}

void X11Operations::drawText(Display* dpy, Window w, GC gc, const char* font_name, const char* text_string, XColor color, int lWidth, int lHeight){	
	XFontStruct *font_info = XLoadQueryFont(dpy, font_name);
	XSetFont(dpy, gc, font_info->fid);

	// Espacio vertical que ocupa el tipo de letra. Independiente del mensaje.
	int font_height = font_info->ascent + font_info->descent;
	
	// Espacio horizontal que ocupa el mensaje.
	int string_width = XTextWidth(font_info, text_string, strlen(text_string));

	// Escribo el mensaje en pantalla.
	XSetForeground(dpy, gc, color.pixel);
	XDrawString(dpy, w, gc, lWidth, lHeight, text_string, strlen(text_string));
	
	// Actualizo la pantalla.
	XMapWindow(dpy, w);			
	XSync(dpy, 0);
}

X11Operations::X11Operations(){}
X11Operations::~X11Operations(){}

}
