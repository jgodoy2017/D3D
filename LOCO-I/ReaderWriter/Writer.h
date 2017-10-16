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
	void writeString(const char*, int);
	void close();
	
private:
	char* num2str(unsigned int);
	void writeToFile();
	void writeChar(char);
	void flushByte(int);
	
	ofstream file;
	int vWriter[32];
	int pWriter=0;
};

}

#endif