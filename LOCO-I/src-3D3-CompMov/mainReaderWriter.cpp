#include <fstream>
#include <iostream>
#include <string>

#include "Writer.h"
#include "Reader.h"

using namespace std;

int main2(int nargs, char *args[]){
	string path=args[1];
	
	Writer* writer = new Writer();
	writer->open(path);
	
	// Ejemplo de escritura de String
	writer->writeString("P5\n640 480\n255\n", 15);
	
	writer->write(255,8);     // Ejemplo de escritura de entero
	writer->write(0xAA,8);    // Ejemplo de escritura de mascara de bits
	writer->write(0xAA,12);   // Ejemplo de escritura usando mas bits de lo minimo para representar un numero (agrega ceros a la izquierda)
	writer->write(0xFF,7);    // Ejemplo de escritura usando menos bits de lo que ocupa el entero (escribe los menos significativos)
	writer->write(0,1);       // Escribe "0"
	writer->write(0x01,1);    // Escribe "1"
	writer->write(1,1);       // Tambien escribe "1"
	
	writer->close();
	
	int val;
	Reader* reader = new Reader();
	reader->open(path);
	
	// Ejemplo de lectura de String
	string strVal = reader->readString(15);
	cout << endl << "Texto: " << endl << strVal << endl;
	
	val = reader->read(8);   // Ejemplo de lectura de 1 byte
	val = reader->read(11);  // Ejemplo de lectura de 11 bits
	val = reader->read(10);
	val = reader->read(9);
	val = reader->read(1);   // Ejemplo de lectura de 1 bit
	reader->close();
		
	return 0;
}
