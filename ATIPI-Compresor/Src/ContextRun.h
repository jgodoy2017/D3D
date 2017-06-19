#pragma once

#include <cstdlib>

#define RESET 64

using namespace std;

class ContextRun {
public:
	ContextRun();
	virtual ~ContextRun();

	int A_racha;
	int N_racha;
	int Nn_racha;

	void updateA(int);
	void updateN(void);
	void updateNn(int);
	void reset(void);
};
