/*
 * Racha.h
 *
 *  Created on: Jun 24, 2017
 *      Author: felipe
 */

#ifndef RACHA_H_
#define RACHA_H_

namespace std {

class Racha {
public:
	Racha();
	Racha(int, int, int, int);
	void updateContexto();
	virtual ~Racha();

	int largo,interruption, contexto, pixel;

};

} /* namespace std */

#endif /* RACHA_H_ */
