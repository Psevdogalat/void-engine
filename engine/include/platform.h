#ifndef __PLATFORM_H__
#define __PLATFORM_H__
	
	#include <string>
	
	namespace ENGINE{
		
		void terminate 			();
		bool get_absolute_path	(std::string&, const std::string& );
	}
#endif