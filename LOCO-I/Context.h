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
	Context(int, int, int, int);
	void updateC();
	virtual ~Context();

	int A;
	int N;
	int N_;

	int B;
	int C;
	int D;

	int q1,q2,q3;
};

} /* namespace std */

#endif /* CONTEXT_H_ */
