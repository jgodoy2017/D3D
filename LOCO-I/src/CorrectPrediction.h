/*
 * CorrectPrediction.h
 *
 *  Created on: May 19, 2017
 *      Author: felipe
 */

#ifndef CORRECTPREDICTION_H_
#define CORRECTPREDICTION_H_

namespace std {

class CorrectPrediction {
public:
	CorrectPrediction();
	int correctPredictedValue(int, int);
	virtual ~CorrectPrediction();
};

} /* namespace std */

#endif /* CORRECTPREDICTION_H_ */
