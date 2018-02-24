#ifndef WAITFORKEY
#define WAITFORKEY

#include <iostream>
#include <stdlib.h>

namespace std {
	
	class WaitForKey{
		
	public:
		WaitForKey();
		~WaitForKey();
		
		static int now();
	};

}

#endif
