#include "NetDef.h"



// get last error defined for Linux
#if defined(_WIN32) || defined(_WIN64)
#else // GNU
	#include <errno.h>
	int getle()
	{
		return errno;
	}
#endif // #if defined(_WIN32) || defined(_WIN64)




