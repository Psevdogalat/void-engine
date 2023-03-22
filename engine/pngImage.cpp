#include <pngImage.hpp>
#include <cstring>


PNG_RGBA8_IMAGE::PNG_RGBA8_IMAGE(){
	init_image();
}

PNG_RGBA8_IMAGE::~PNG_RGBA8_IMAGE(){
	free_image();
}

void PNG_RGBA8_IMAGE::init_image(){
	memset(&image_ctrl, 0, sizeof(png_image));
	image_ctrl.version 	= PNG_IMAGE_VERSION;
	image_buffer 		= nullptr;
	image_buffer_size	= 0;
}

void PNG_RGBA8_IMAGE::free_image(){
	if(image_ctrl.opaque != nullptr)
		png_image_free(&image_ctrl);
	
	if(image_buffer != nullptr)
		free(image_buffer);
	
}

size_t PNG_RGBA8_IMAGE::get_width()		 
	{ return (size_t)image_ctrl.width; }
size_t PNG_RGBA8_IMAGE::get_height()	 
	{ return (size_t)image_ctrl.height;}
size_t PNG_RGBA8_IMAGE::get_buffer_size()
	{ return image_buffer_size;}
unsigned char* PNG_RGBA8_IMAGE::get_buffer()
	{ return (unsigned char*) image_buffer; }

bool PNG_RGBA8_IMAGE::load(const char* File_name){
	if(image_buffer != nullptr){
		free_image();
		init_image();
	}
	
	if(png_image_begin_read_from_file(&image_ctrl, File_name) != 0){
		
		image_ctrl.format = PNG_FORMAT_RGBA;
		image_buffer_size = PNG_IMAGE_SIZE(image_ctrl);
		image_buffer = (png_bytep) malloc(image_buffer_size);
		
		if(png_image_finish_read(&image_ctrl, nullptr, image_buffer, 0, nullptr) != 0)
			return true;	
		
	}
	
	free_image();
	init_image();
	
	return false;
}


bool PNG_RGBA8_IMAGE::save(const char* File_name){
	return false;
}
