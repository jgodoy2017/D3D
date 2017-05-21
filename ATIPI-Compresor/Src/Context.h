 /**
  @file Context.h

  @author Felipe Tambasco, Mauro Barbosa
  @date Feb, 2017

*/

#ifndef CONTEXT_H_
#define CONTEXT_H_

namespace std {

class Context {
public:
	Context();
	Context(int, int, int);
	virtual ~Context();

	int A;
	int N;

	int q1,q2,q3;
};

} /* namespace std */

#endif /* CONTEXT_H_ */
