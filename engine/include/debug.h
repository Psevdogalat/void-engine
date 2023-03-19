#ifndef __DEBUG_H__
#define __DEBUG_H__

#define __DEBUG__

#ifdef __DEBUG__
	#include <stdio.h>	
	#define DEBUG(x...)\
		fprintf(stderr, x) 
#else
	#define DEBUG(x...)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	

#ifdef __cplusplus
}
#endif

#endif
