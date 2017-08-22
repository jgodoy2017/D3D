#include "ContextRun.h"

using namespace std;

	//esta clase modela los contextos especiales que aparecen en el modo de rachas. 

ContextRun::ContextRun(){
	this->A_racha=4;
	this->N_racha=1;
	this->Nn_racha=0;
}

void ContextRun::updateA(int eps){
	this->A_racha+=abs(eps);
}

void ContextRun::updateN(){
	this->N_racha++;
}

void ContextRun::updateNn(int eps){
	if(eps<0) this->Nn_racha++;
}

void ContextRun::reset(){
	this->A_racha/=2;
	this->N_racha/=2;
	this->Nn_racha/=2;		
}	

ContextRun::~ContextRun(){}
