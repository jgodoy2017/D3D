#ifndef RACHAS_H_
#define RACHAS_H_

// Defino las 2 macros que siguen para poder visualizar los malditos bits...
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define POKE_BIT(a, n, b, m) ((a)[(n)/8] = (a)[(n)/8] | ((((b)&(1<<(7-(m))))>>(7-(m)))<<(7-(n)%8)))
/* Magia.
	  Expresion que dada una tira de bits (a), copia en el bit en la posicion (n) el valor del bit en la posicion (m) de un byte (b) de referencia.
	  Las posiciones se miden siempre de izquierda a derecha (desde el bit mas significativo al menos significativo).
	  
	  (n)/8 = byte de (a) que se debe modificar.
	  7-(m) = posicion en (b) del bit a copiar, medida desde la derecha.

	  1<<(7-(m)) = |000...010.....0|
	               |------>|       | 
	                  7-m
	  
	  (b)&(1<<(7-(m))) = |000...0x0.....0|       (x = bit 7-m de b) 
	            	  	 |------>|       | 
	        			   7-m
	  
	  (b)&(1<<(7-(m))))>>(7-(m)) = |000.......0x|
	  
	  7-(n)%8 = posicion en el byte (n)/8 de (a) que ocupa el bit (n), medida desde la derecha.
	  (((b)&(1<<(7-(m))))>>(7-(m)))<<(7-(n)%8)) = |000...0x0.....0|
	                                              |       |<------|
	                                                        7-n%8
	  
	  a[(n)/8] = byte (n)/8 de (a).
	  (a)[(n)/8] | ((((b)&(1<<(7-(m))))>>(7-(m)))<<(7-(n)%8)) = |a0a1a2...x......a7|       (an = bit n de a)
	                                                            |         |<-------|
	                                                                         7-n%8
*/
	  
#define PEEK_BIT(a, n, b, m) ((b) = (b) | ((((a)[(n)/8]&(1<<(7-(n)%8)))>>(7-(n)%8))<<(7-(m))))
/*
	  Expresion que dada una tira de bits (a), obtiene el bit en la posicion (n) y lo copia en la posicion (m) de un byte (b) de referencia.
	  Las posiciones se miden siempre de izquierda a derecha (desde el bit mas significativo al menos significativo).
	  
	  (n)/8   = byte de (a) que se debe leer.
	  7-(n)%8 = posicion en el byte (n)/8 de (a) que ocupa el bit (n), medida desde la derecha.
	  
	  1<<(7-(n)%8) = |000...010.....0|
	                 |------>|       | 
	                   7-n%8
	  
	  a[(n)/8] = byte (n)/8 de (a).
	  (a)[(n)/8]&(1<<(7-(n)%8)) = |000...0x0.....0|       (x = bit 7-n%8 de a[(n)/8])
	            	  	          |------>|       | 
	        			            7-n%8
	  
	  7-(m) = posicion en (b) del bit a escribir, medida desde la izquierda.
	  (((a)[(n)/8]&(1<<(7-(n)%8)))>>(7-(n)%8))<<(7-(m)) = |000...0x0.....0|
	                                                      |       |<------|
	                                                                 7-m
	  
	  (b) | ((((a)[(n)/8]&(1<<(7-(n)%8)))>>(7-(n)%8))<<(7-(m))) = |b0b1b2...x......b7|       (bm = bit m de b)
	                                                              |         |<-------|
	                                                                            7-m
*/

#define PEEK_BIT(a, n, b, m) ((b) = (b) | ((((a)[(n)/8]&(1<<(7-(n)%8)))>>(7-(n)%8))<<(7-(m))))
/*
	  Expresion que dada una tira de bits (a), obtiene el bit en la posicion (n) y lo copia en la posicion (m) de un byte (b) de referencia.
	  Las posiciones se miden siempre de izquierda a derecha (desde el bit mas significativo al menos significativo).
	  
	  (n)/8   = byte de (a) que se debe leer.
	  7-(n)%8 = posicion en el byte (n)/8 de (a) que ocupa el bit (n), medida desde la derecha.
	  
	  1<<(7-(n)%8) = |000...010.....0|
	                 |------>|       | 
	                   7-n%8
	  
	  a[(n)/8] = byte (n)/8 de (a).
	  (a)[(n)/8]&(1<<(7-(n)%8)) = |000...0x0.....0|       (x = bit 7-n%8 de a[(n)/8])
	            	  	          |------>|       | 
	        			            7-n%8
	  
	  7-(m) = posicion en (b) del bit a escribir, medida desde la izquierda.
	  (((a)[(n)/8]&(1<<(7-(n)%8)))>>(7-(n)%8))<<(7-(m)) = |000...0x0.....0|
	                                                      |       |<------|
	                                                                 7-m
	  
	  (b) | ((((a)[(n)/8]&(1<<(7-(n)%8)))>>(7-(n)%8))<<(7-(m))) = |b0b1b2...x......b7|       (bm = bit m de b)
	                                                              |         |<-------|
	                                                                            7-m
*/

using namespace std;

static const int J[32]={ // Tabla de correspondencia con los kr.
		0,  0,  0,  0,
		1,  1,  1,  1, 
		2,  2,  2,  2,
		3,  3,  3,  3,
		4,  4,  5,  5,
		6,  6,  7,  7,
		8,  9,  10, 11,
		12, 13, 14, 15
	};

#endif
