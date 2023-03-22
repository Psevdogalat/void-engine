#ifndef __PNG_IMAGES__
#define __PNG_IMAGES__
	#include <cstdlib>
	#include <png.h>
	
	class PNG_RGBA8_IMAGE{
		protected:
			png_image 	image_ctrl;
			png_bytep 	image_buffer;
			size_t	 	image_buffer_size;
			
			void free_image();
			void init_image();
			
		public:
			PNG_RGBA8_IMAGE();
			~PNG_RGBA8_IMAGE();
			size_t 			get_width();
			size_t 			get_height();
			size_t 			get_buffer_size();
			unsigned char* 	get_buffer();   
			
			bool load(const char* );
			bool save(const char* );
			
	};

#endif