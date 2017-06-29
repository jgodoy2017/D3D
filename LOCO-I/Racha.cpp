/*
 * Racha.cpp
 *
 *  Created on: Jun 24, 2017
 *      Author: felipe
 */

#include <iostream>
#include <stdlib.h>
#include "Racha.h"

namespace std {

Racha::Racha() {
	// TODO Auto-generated constructor stub

}
Racha::Racha(int largo, int interruption, int pixel, int contexto) {

	this->largo=largo;
	this->interruption=interruption;
	this->contexto=contexto;
	this->pixel=pixel;

}

Racha::~Racha() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
