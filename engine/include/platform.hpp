#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__
	
#include <windows.h>
#include <engine.hpp>

extern "C"{
	UINT getMainWindowWidth();
	UINT getMainWindowHeight();
	HWND getMainWindow();
}

void render(VoidEngine::Engine *, HDC );

#endif
