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

int main(int argc, char **argv){
	int i=0, h_pos, width, px_a, px_b, px_x, ri_type, T_racha, A_racha, N_racha, K_racha, Nn_racha,
	    prefijo, l_racha=0, l_resto, L_max, Q_max, Q_golomb=0, R_golomb, M_eps, eps_val, eps_sign, map, caso_map;
	unsigned char racha[10]={0xFF, 0xAB, 0xBF, 0xDA, 0xAA, 0xAF, 0x0F, 0xAA, 0x00, 0xFF};
//	unsigned char racha[10]={0xFF, 0xAF, 0xFF, 0xFF, 0x8A, 0xF0, 0xAF, 0xAA, 0x00, 0xFF};
	unsigned char racha_byte=0x00, resto[2]={0x00, 0x00};

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
	
	printf(">> Posicion horizontal del pixel inicial (a): ");
	scanf("%d", &h_pos);
	printf(">> Ancho de la imagen : ");
	scanf("%d", &width);
	printf(">> Valor del pixel superior (a): ");
	scanf("%d", &px_a);
	printf(">> Valor del pixel inicial (b): ");
	scanf("%d", &px_b);
	printf(">> Estadistico A: ");
	scanf("%d", &A_racha);
	printf(">> Estadistico N: ");
	scanf("%d", &N_racha);
	printf(">> Estadistico Nn: ");
	scanf("%d", &Nn_racha);
	printf("\n");
	
	printf(">> Tira de bits: ");
	for(int k=0; k<80; k++){
		PEEK_BIT(racha, k, racha_byte, 7);
		printf("%d", (racha_byte==0x01) ? 1 : 0);
		racha_byte=0x00;
	}
	printf("b\n");
	
	PEEK_BIT(racha, 0, racha_byte, 7);
	while((racha_byte==0x01) && (i<31)){   // Leo 1's de la tira de bits.
		racha_byte=0x00;
		if((h_pos+l_racha+(1<<J[i]))<=width){    // Siempre que haya espacio horizontal...
			l_racha+=(1<<J[i]);                  // ...aumento el largo de racha...
			i++;                                 // ...incremento la entrada en J...
			PEEK_BIT(racha, i, racha_byte, 7);   // ...y vuelvo a leer un bit.
		}
	}
	printf(">> Cantidad de 1's para el largo de racha: %d\n", i);
	printf(">> Parte unaria del largo de racha: %d\n", l_racha);
	
	if(h_pos+l_racha<width){
		printf(">> La racha se interrumpe por un pixel de otro color.\n");
		prefijo=0;
	}else{
		printf(">> La racha termina con la fila. Racha decodificada.\n");
		printf(">> Largo total de la racha: %d pixeles.\n", l_racha);
		prefijo=1;
	}
	
	if(!prefijo){
		racha_byte=0x00;
		PEEK_BIT(racha, i, racha_byte, 7);  // Leo el bit de prefijo.
		if((!racha_byte) && J[i]){          // Leo el resto correspondiente al largo de racha.
			printf(">> El bit de prefijo (%d) es correcto (0).\n", i+1);
			
			if(J[i]>8){  // Obtengo la representacion binaria del resto, que solamente puede tener 2 bytes...
				for(int k=0; k<8; k++)      PEEK_BIT(racha, i+1+k, resto[0], k);
				for(int k=0; k<J[i]-8; k++) PEEK_BIT(racha, i+9+k, resto[1], k+16-J[i]);
			}else{       // ...o 1 byte.
				for(int k=0; k<J[i]; k++)   PEEK_BIT(racha, i+1+k, resto[1], k+8-J[i]);
			}
			l_resto=(int)((1<<(J[i]-8))*(resto[0]&0xFF)+(resto[1]&0xFF));   // Obtengo el valor numerico del resto.
			l_racha+=l_resto;                                               // Calculo el largo total de la racha.
			printf(">> Resto del largo de la racha: %d representado con %d bits.\n", l_resto, J[i]);
			printf(">> Largo total de la racha: %d pixeles.\n\n", l_racha);
			
			ri_type=(px_a==px_b ? 1 : 0);  // Run Interrpution Type.
			
			T_racha=((ri_type==1) ? A_racha : A_racha + N_racha/2);   // Calculo la variable T para determinar k de Golomb.
			for(K_racha=0; (N_racha<<K_racha)<T_racha; K_racha++);    // k = min{k' / 2^(k') >= T}
			
			L_max=32-J[i];    // Largo maximo para el Golomb de largo limitado.                L_max = 32 - (kr + 1)	
			Q_max=L_max-9;    // Valor de q que decide si estamos leyendo un codigo de escape. q_max = (L_max - 1) - 8
			
			i+=1+J[i];  // Salteo el bit de prefijo y el resto, ya leidos.
			racha_byte=0x00;
			PEEK_BIT(racha, i, racha_byte, 7);
			while((racha_byte==0x01) && (Q_golomb<Q_max)){  // Leo la cantidad de 1's (Q_golomb) y determino si es Golomb o codigo de escape.
				Q_golomb++;
				racha_byte=0x00;
				PEEK_BIT(racha, i, racha_byte, 7);				
				i++;
			}
			i--;
			
			printf(">> A      = %d\n", A_racha);
			printf(">> N      = %d\n", N_racha);
			printf(">> Nn     = %d\n", Nn_racha);
			printf(">> RIType = %d\n", ri_type);
			printf(">> T      = %d\n", T_racha);
			printf(">> k      = %d\n", K_racha);
			printf(">> L      = %d\n", Q_golomb+1);
			printf(">> L_max  = %d\n", L_max);
			printf(">> q_max  = %d\n", Q_max);
			printf(">> m      = %d\n", 1<<K_racha);
			printf(">> q      = %d\n", Q_golomb);
			printf(">> c      = %d\n\n", K_racha);
			
			if(Q_golomb<Q_max){
				printf(">> El pixel de salida esta codificado en Golomb(2^%d).\n", K_racha);
				racha_byte=0x00;
				PEEK_BIT(racha, i, racha_byte, 7);  // Leo el bit de corte de la parte unaria (debe ser 0).
				if(!racha_byte){
					printf(">> El bit de corte de la parte unaria (%d) es correcto (0).\n\n", i+1);

					i++;               // Me posiciono al inicio del resto de Golomb, salteando el bit de prefijo.
					racha_byte=0x00;   // Obtengo la representacion binaria del resto de Golomb, representado con c=K_racha bits.
					for(int k=0; k<K_racha; k++) PEEK_BIT(racha, i+k, racha_byte, 8+k-K_racha);
					
					R_golomb=(int)(racha_byte&0xFF);               // Obtengo el valor numerico del resto.
					M_eps=(1<<K_racha)*Q_golomb+R_golomb;          // Obtengo el valor codificado.
					
					printf(">> r      = %d\n", R_golomb);
					printf(">> M(eps) = %d\n", M_eps);
					
					i+=K_racha;   // Largo total de la tira de bits leida.
				}else{
					printf(">> El bit de corte de la parte unaria (%d) es incorrecto (1). Racha invalida.\n", i+1);
				}
			}else{
				printf(">> El pixel de salida esta codificado con un codigo de escape.\n");
				
				i+=2;               // Me posiciono al inicio de la codificacion del error, salteando los bits de prefijo y corte.
				racha_byte=0x00;    // Obtengo la representacion binaria del (M(eps)-1).
				for(int k=0; k<8; k++) PEEK_BIT(racha, i+k, racha_byte, k);

				M_eps=1+(int)(racha_byte&0xFF);    // Obtengo el valor numerico de M(eps).

				printf(">> M(eps) = %d\n", M_eps);
				
				i+=8;    // Largo total de la tira de bits leida.
			}

			map=((M_eps+ri_type)%2 ? 1 : 0);   // Deduzco la variable "map", que solo puede ser 0 o 1.
			eps_val=(M_eps+ri_type+map)>>1;    // Obtengo el valor absoluto del error codificado.					
			
			// Decisiones para determinar el signo del error codificado. Invierto la tabla del codificador.
			if(  K_racha  &&   map  && (2*Nn_racha>=N_racha)) {eps_sign=-1; caso_map=1;}
			if(  K_racha  &&   map  && (2*Nn_racha <N_racha)) {eps_sign=-1; caso_map=2;}
			if(  K_racha  && (!map) && (2*Nn_racha>=N_racha)) {eps_sign=(eps_val ? 1 : 0); caso_map=3;}
			if(  K_racha  && (!map) && (2*Nn_racha <N_racha)) {eps_sign=(eps_val ? 1 : 0); caso_map=4;}
			if((!K_racha) &&   map  && (2*Nn_racha>=N_racha)) {eps_sign=-1; caso_map=5;}
			if((!K_racha) &&   map  && (2*Nn_racha <N_racha)) {eps_sign=1; caso_map=6;}
			if((!K_racha) && (!map) && (2*Nn_racha>=N_racha)) {eps_sign=(eps_val ? 1 : 0); caso_map=7;}
			if((!K_racha) && (!map) && (2*Nn_racha <N_racha)) {eps_sign=-1; caso_map=8;}
			
			eps_val*=eps_sign;   // Obtengo el valor numerico del error codificado.
			
			printf(">> map    = %d\n", map);
			printf(">> caso   = %d\n", caso_map);
			printf(">> S(eps) = %d\n", eps_sign);
			printf(">> eps    = %d\n\n", eps_val);
			
			px_x=px_b+eps_val;   // Calculo el valor numerico del pixel de salida.
			
			// Invierto la reduccion de rango para el pixel de salida ([-128, 127] --> [0, 255]).
			if(px_x<0)    px_x+=256;
			if(px_x>=256) px_x-=256;
			if(px_x<0)    px_x=0;
			if(px_x>255)  px_x=255;
			
			printf(">> El pixel inicial (%03d) se repite %d veces. Valor de pixel de salida: %03d.\n>> Racha decodificada.\n\n", px_b, l_racha, px_x);

			// Actualizacion y reset de estadisticos.
			A_racha+=abs(eps_val);
			if(eps_val<0) Nn_racha++;
			
			if(A_racha > 64){
				A_racha/=2;
				N_racha/=2;
				Nn_racha/=2;		
				printf(">> RESET de estadisticos: A=%d - N=%d - Nn=%d\n", A_racha, N_racha+1, Nn_racha);
			}
			
			N_racha++; // N_racha se actualiza siempre despues de un posible reset.
		}else{
			printf(">> El bit de prefijo (%d) no coincide con el prefijo esperado (0). Racha invalida\n", i+1);
		}
	}
}
