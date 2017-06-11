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
	int largo, prefijo, aciertos=0, i=0, l_racha=-1, ri_type, eps_val, A_racha, N_racha, Nn_racha, T_racha, K_racha, map,
	    M_eps, caso_map, L_max, Q_max, Q_golomb, R_golomb, l_pixel;
	char tipo[10];
	unsigned char resto[2]={0x00, 0x00}, resto_bit, racha[10], racha_byte=0x00, golomb[4];

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
	printf(">> Run Interruption Type: [0] a!=b / [1] a==b : ");
	scanf("%d", &ri_type);
	printf(">> Error de prediccion a codificar: ");
	scanf("%d", &eps_val);
	printf(">> Estadistico A: ");
	scanf("%d", &A_racha);
	printf(">> Estadistico N: ");
	scanf("%d", &N_racha);
	printf(">> Estadistico Nn: ");
	scanf("%d", &Nn_racha);
	printf("\n");
	
	// Reduccion de rango para el error a codificar ([-255, 255] --> [-128, 127]).
	if(eps_val<0)    eps_val+=256;
	if(eps_val>=128) eps_val-=256;
	if(eps_val<-128) eps_val=-128;
	if(eps_val>127)  eps_val=127;
	
	for(int k=0; k<10; k++) racha[k]=0x00;
	
	while((largo>0) & (i<32)){ // Adaptativo.
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

		largo*=-1;  // Lo dejo positivo.
		printf(">> resto = %d = "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"b representado con %d bits.\n", largo, BYTE_TO_BINARY((unsigned char)(largo/256)), BYTE_TO_BINARY((unsigned char)(largo)), J[i]);

		resto[0]=(unsigned char)(largo/256);   // El resto nunca puede
		resto[1]=(unsigned char)(largo%256);   // ocupar mas de 2 bytes.
		
		racha[i/8]=racha[i/8]|(prefijo<<(7-i%8));   // Agrego el prefijo...	
		if(J[i]>8){                                 // ...y el resto, que puede tener 2 bytes...
			for(int k=i+1; k<i+J[i]-7; k++)      POKE_BIT(racha, k, resto[0], 15-J[i]+k-i);
			for(int k=i+J[i]-7; k<i+J[i]+1; k++) POKE_BIT(racha, k, resto[1], 7-J[i]+k-i);
		}else{                                      // ...o 1 byte.
			for(int k=i+1; k<i+J[i]+1; k++)      POKE_BIT(racha, k, resto[1], 7-J[i]+k-i);
		}
	}
	
	printf(">> Codificacion del largo de racha: ");
	for(int k=0; k<6; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(racha[k]));
	printf("b - Largo: %d bits.\n\n", l_racha);
	
	if(!prefijo){    // Si la racha es interrumpida por un pixel de otro color, codifico el pixel de interrupcion.
		T_racha=((ri_type==1) ? A_racha : A_racha + N_racha/2);   // Calculo la variable T para determinar k de Golomb.
		for(K_racha=0; (N_racha<<K_racha)<T_racha; K_racha++);    // k = min{k' / 2^(k') >= T}

		// Decisiones para hallar el valor de "map", utilizado en el mapeo que se le hace al error a codificar.
		if(  K_racha  && (eps_val<=0) && (2*Nn_racha>=N_racha)) {map=(eps_val ? 1 : 0); caso_map=1;}
		if(  K_racha  && (eps_val<=0) && (2*Nn_racha <N_racha)) {map=(eps_val ? 1 : 0); caso_map=2;}
		if(  K_racha  && (eps_val >0) && (2*Nn_racha>=N_racha)) {map=0; caso_map=3;}
		if(  K_racha  && (eps_val >0) && (2*Nn_racha <N_racha)) {map=0; caso_map=4;}
		if((!K_racha) && (eps_val<=0) && (2*Nn_racha>=N_racha)) {map=(eps_val ? 1 : 0); caso_map=5;}
		if((!K_racha) && (eps_val<=0) && (2*Nn_racha <N_racha)) {map=0; caso_map=6;}
		if((!K_racha) && (eps_val >0) && (2*Nn_racha>=N_racha)) {map=0; caso_map=7;}
		if((!K_racha) && (eps_val >0) && (2*Nn_racha <N_racha)) {map=1; caso_map=8;}
	
		M_eps=2*abs(eps_val)-ri_type-map;    // Mapeo del error a codificar por Golomb(2^k).
	
		printf(">> A      = %d\n", A_racha);
		printf(">> N      = %d\n", N_racha);
		printf(">> Nn     = %d\n", Nn_racha);
		printf(">> RIType = %d\n", ri_type);
		printf(">> T      = %d\n", T_racha);
		printf(">> k      = %d\n", K_racha);
		printf(">> map    = %d\n", map);
		printf(">> caso   = %d\n", caso_map);	
		printf(">> eps    = %d\n", eps_val);
		printf(">> M(eps) = %d\n\n", M_eps);
	
		Q_golomb=(M_eps>>K_racha);             // q=n/m   donde n=M_eps, m=2^k, k=K_racha. Dividir entre   m es "perder" K_racha bits.
		R_golomb=(M_eps-(Q_golomb<<K_racha));  // r=n-m*k donde n=M_eps, m=2^k, k=K_racha. Multipilcar por m es "ganar"  K_racha bits.
		// Notese que la otra ecuacion, c=techo(log2(m)) con m=2^k, implica c=k. Como k=K_racha, ya lo tenemos calculado.
	
		L_max=32-J[i];    // Largo maximo para el Golomb de largo limitado.          L_max = 32 - (kr + 1)	
		Q_max=L_max-9;    // Valor de q que decide si limitamos el largo del codigo. q_max = (L_max - 1) - 8
	
		printf(">> n     = %d\n", M_eps);
		printf(">> m     = %d\n", 1<<K_racha);
		printf(">> q     = %d\n", Q_golomb);
		printf(">> r     = %d\n", R_golomb);
		printf(">> c     = %d\n", K_racha);
		printf(">> L     = %d\n", Q_golomb+1);
		printf(">> L_max = %d\n", L_max);
		printf(">> q_max = %d\n\n", Q_max);
	
		for(int k=0; k<4; k++) golomb[i]=0x00;
		if(Q_golomb<Q_max){   // No limitamos el largo del codigo. Codificamos en Golomb(2^k).
			printf(">> Codificamos en Golomb(2^%d).\n>> El prefijo ocupa %d bits y el resto ocupa %d bits.\n\n", K_racha, Q_golomb+1, K_racha);
		
			// Parte unaria
			for(int k=0; k<Q_golomb; k++) POKE_BIT(golomb, k, 0xFF, 1);      // Agrego q 1's...
			POKE_BIT(golomb, Q_golomb+1, 0x00, 1);                           // ...y un 0.
		
			// Parte binaria
			racha_byte=(unsigned char)(R_golomb%256);
			for(int k=Q_golomb+1; k<Q_golomb+1+K_racha; k++) POKE_BIT(golomb, k, racha_byte, 7-Q_golomb+k-K_racha);
			
			printf(">> Codificacion del pixel de salida: ");
			for(int k=0; k<4; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(golomb[k]));
			printf("b - Largo: %d bits.\n", Q_golomb+1+K_racha);
		
			l_pixel=Q_golomb+1+K_racha;   // Largo total de la codificacion del pixel de salida.
		}else{                // Limitamos el largo del codigo. Enviamos "codigo de escape".
			printf(">> Enviamos codigo de escape.\n>> Limitamos el largo del prefijo (%d bits) a %d bits, con 8 bits de resto.\n\n", Q_golomb+1, Q_max+1);
		
			// Parte unaria
			for(int k=0; k<Q_max; k++) POKE_BIT(golomb, k, 0xFF, 1);      // Agrego q_max 1's...
			POKE_BIT(golomb, Q_max+1, 0x00, 1);                           // ...y un 0.
		
			// Parte binaria
			racha_byte=(unsigned char)((M_eps-1)%256);   // En este caso, se codifica (M(eps) - 1) por definicion.
			for(int k=Q_max+1; k<Q_max+9; k++) POKE_BIT(golomb, k, racha_byte, k-1-Q_max);
		
			printf(">> Codificacion del pixel de salida: ");
			for(int k=0; k<4; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(golomb[k]));
			printf("b representada con %d bits.\n", Q_max+9);
		
			l_pixel=Q_max+9;   // Largo total de la codificacion del pixel de salida.
		}
	
		// Agrego a la codificacion del largo la codificacion del pixel de salida.
		for(int m=0; m<4; m++){
			for(int k=l_racha%8; k<8; k++)   POKE_BIT(racha, l_racha+8*m+k-l_racha%8, golomb[m], k-l_racha%8);
			for(int k=8-l_racha%8; k<8; k++) POKE_BIT(racha, l_racha+8*m+k, golomb[m], k);	
		}
	}
	
	printf(">> Tira de bits: ");
	for(int k=0; k<10; k++) printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(racha[k]));
	printf("b - Largo: %d\n\n", l_racha+l_pixel);
	
	// Actualizacion y reset de estadisticos.
	A_racha+=abs(eps_val);
	if(eps_val<0) Nn_racha++;
	
	if(A_racha > 64){
		A_racha/=2;
		N_racha/=2;
		Nn_racha/=2;		
		printf(">> RESET de estadisticos: A=%d - N=%d - Nn=%d\n", A_racha, N_racha+1, Nn_racha);
	}
	
	N_racha++;  // N_racha se actualiza siempre despues de un posible reset.
}
