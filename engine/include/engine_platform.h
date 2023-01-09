#ifndef __ENGINE_PLATFORM_H__
#define __ENGINE_PLATFORM_H__
	
	#include <events.h>
	
	namespace ENGINE{
		
		/* platform calling engine core functions */
		void eng_init	(const char* );
		void eng_free	();
		void eng_compute(double );
		void eng_render	();
		void eng_event	(const EVENT_MSG& );
	}
	
#endif