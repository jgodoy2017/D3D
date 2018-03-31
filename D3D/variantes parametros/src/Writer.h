#include <fstream>
#include <iostream>
#include <string>

#ifndef WRITER_H
#define WRITER_H

namespace std{

class Writer{
public:
	Writer();
	virtual ~Writer();

	void open(string);
	void write(int, int);
	void writeChar(char);
	void writeString(const char*, int);
	void close();
	
	int cantidad_bytes_escritos=0;

private:
	char* num2str(unsigned int);
	void writeToFile();

	void flushByte(int);
	
	ofstream file;
	int vWriter[32];
	int pWriter=0;


};

}

#endif
