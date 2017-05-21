#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
	  7-(m) = posicion en (b) del bit a copiar, medida desde la izquierda.

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


int main(int argc, char **argv){
	int largo, prefijo, aciertos=0, i=0, l_racha=-1;
	char tipo[10];
	unsigned char resto[2]={0x00, 0x00}, resto_bit, racha[6], racha_byte=0x00;

	int J[32]={ // Almaceno en memoria la correspondencia con los kr.
			0,  0,  0,  0,
			1,  1,  1,  1, 
			2,  2,  2,  2,
			3,  3,  3,  3,
			4,  4,  5,  5,
			6,  6,  7,  7,
			8,  9,  10, 11,
			12, 13, 14, 15
		};
	
	printf(">> Largo de la racha: ");
	scanf("%d", &largo);
	printf(">> Interrumpida por: [P]ixel distinto / [F]in de fila : ");
	scanf("%s", tipo);
	printf("\n");
	
	if(largo > 32768){ // Explota el sistema.
		printf("Are you kidding me?\n");
		exit(255);
	}
	
	for(int k=0; k<6; k++) racha[i]=0x00;
	
	if(!largo){
		l_racha=0;
		printf(">> Racha de largo nulo.\n");
	}
		
	while((largo>0) & (i<32)){ // Golomb adaptativo.
		largo-=(1<<J[i]);      // kr = 2^(J[i])
		i++;                   // "Aciertos"
		printf(">> i = %02d - kr = %05d - resto = %03d\n", i, 1<<J[i], largo);
	}
	
	if(l_racha==-1){
		if(largo) l_racha=i+1+J[i];   // Racha de "1"s + 1 bit de prefijo + largo del resto
		else      l_racha=i;          // Solamente la racha de "1"s (no hay fallos).
	}
	
	for(int k=0; k<i; k++){           // Armo la tira de "1"s
		racha_byte=racha_byte|(1<<(7-(k%8)));
		racha[k/8]=racha_byte;
		if(!((k+1)%8)) racha_byte=0x00;
	}
	
	if(largo<0){  // "Fallo"
		prefijo=(!strncmp(tipo, "P", 1) ? 0 : 1);
		printf(">> prefijo = %d\n", prefijo);

		largo*=-1;  //lo dejo positivo.
		printf(">> resto = %d = "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"b representado con %d bits = ", largo, BYTE_TO_BINARY((unsigned char)(largo/256)), BYTE_TO_BINARY((unsigned char)(largo)), J[i]);

		resto[0]=(unsigned char)(largo/256);   // El resto nunca puede
		resto[1]=(unsigned char)(largo%256);   // ocupar mas de 2 bytes.

		if(J[i]>8){  //Oscuro. Difuso. Debug interno. (!)
			for(int k=16-J[i]; k<8; k++){
				resto_bit=((resto[0]&(1<<(7-k)))>>(7-k))&0x01;
				printf("%c", resto_bit==0x01 ? '1' : '0');
			}
			for(int k=0; k<8; k++){
				resto_bit=((resto[1]&(1<<(7-k)))>>(7-k))&0x01;
				printf("%c", resto_bit==0x01 ? '1' : '0');
			}
		}else{
			for(int k=8-J[i]; k<8; k++){
				resto_bit=((resto[1]&(1<<(7-k)))>>(7-k))&0x01;
				printf("%c", resto_bit==0x01 ? '1' : '0');
			}			
		}
		printf("b\n\n");
		
		racha[i/8]=racha[i/8]|(prefijo<<(7-i%8));   // Agrego el prefijo...	
		if(J[i]>8){                                 // ...y el resto, que puede tener 2 bytes...
			for(int k=i+1; k<i+J[i]-7; k++)      POKE_BIT(racha, k, resto[0], 15-J[i]+k-i);
			for(int k=i+J[i]-7; k<i+J[i]+1; k++) POKE_BIT(racha, k, resto[1], 7-J[i]+k-i);
		}else{                                      // ...o 1 byte.
			for(int k=i+1; k<i+J[i]+1; k++)      POKE_BIT(racha, k, resto[1], 7-J[i]+k-i);
		}
	}
	
	printf(">> Tira de bits: ");
	for(int k=0; k<6; k++) printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(racha[k]));
	printf("b - Largo: %d\n", l_racha);
	
	// Los "^" que aparecen al final. El significado varia. 
	printf("                  ");
	for(int k=2; k<i; k++) printf(" ");
	if(l_racha>i){
		printf(" ^^");
		for(int k=i+3; k<l_racha; k++) printf(" ");
		if(J[i]>1) printf("^");
	}else{
		printf("^");
	}
	printf("\n");
}
