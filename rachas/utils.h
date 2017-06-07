#ifndef UTILS_H_
#define UTILS_H_

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

using namespace std;

class Utils{
public:
	Utils();
	unsigned char* bool2uchar(int*);
	void uchar2bool(unsigned char*, int);
	virtual ~Utils();
	
	bool bitsToFile[800];
	int bitsToFilePointer;
};

#endif
