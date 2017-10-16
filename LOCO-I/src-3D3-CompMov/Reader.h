#include <fstream>
#include <iostream>
#include <string>

#ifndef READER_H
#define READER_H

namespace std{

class Reader{
public:
	Reader();
	virtual ~Reader();
	
	void open(string);
	int read(int);
	string readString(int);
	void close();
	
private:
	int readBuffer(int);
	int readFirstBits(char*, int);
	char readChar();
	
	ifstream file;
	int pReader=0;
	char vReader = 0x00;
};

}

#endif