#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "X11/WaitForKey.h"

namespace std{

int WaitForKey::now(){
	
	/*
	  Codigo oscuro. BeWaRe.  (--__--)
	  Intento capturar el las flechas para avanzar/retroceder de frame.
	  
	  Devuelve:
	     ARRIBA    = 1
	     ABAJO     = 2
	     DERECHA   = 3
	     IZQUIERDA = 4
	     Otro      = codigo ASCII correspondiente
	*/
	
	int ch;
	
	// Anulo el buffer de entrada, leo directo del hardware (teclado). (!)
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
	
	while(1){
		ch = getchar();
		if(ch == 27){
			if((ch=getchar()) == 91){
				ch=getchar();
				switch(ch){
					case 65:
						cout << "\b\b\b\b    \b\b\b\b";
						return(1);
						break;
					case 66:
						cout << "\b\b\b\b    \b\b\b\b";
						return(2);
						break;
					case 67:
						cout << "\b\b\b\b    \b\b\b\b";
						return(3);
						break;
					case 68:
						cout << "\b\b\b\b    \b\b\b\b";
						return(4);
						break;
				}
			}
		} else {
			cout << "\b \b";
			return ch;
		}
	}
	
	// Restauro el buffer de entrada.
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


WaitForKey::WaitForKey(){}
WaitForKey::~WaitForKey(){}

}
