#ifndef X11OPERATIONS
#define X11OPERATIONS

#include <iostream>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace std {
	
	class X11Operations{
		
	public:
		X11Operations();
		~X11Operations();
		
		static void drawImage(Display*, Window, GC, int, int, int, int*, int, int, int);
		static void drawText(Display*, Window, GC, const char*, const char*, XColor, int, int);
	};

}

#endif
