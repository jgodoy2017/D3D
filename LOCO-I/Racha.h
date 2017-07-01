/*
 * Racha.h
 *
 *  Created on: Jun 24, 2017
 *      Author: felipe
 */

#pragma once
#define K_MAX 9

namespace std {

class Racha {
public:
	Racha();
	Racha(int, int, int, int);
	virtual ~Racha();

	int largo, interruption, contexto, pixel;
};

} /* namespace std */
