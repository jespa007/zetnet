#include "zetnet.h"

namespace zetnet{
	namespace time_utils{
		void delay(unsigned long _ms){
		#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
				Sleep( _ms );
		#else
				usleep( _ms * 1000 );
		#endif


		}
	}

}
