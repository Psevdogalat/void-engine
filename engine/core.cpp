#include <engine.h>

using namespace VoidEngine;

Engine::Engine()
{
	if(instance != nullptr)
		throw std::runtime_error("Engine: multyinstance deprecated");

	instance = this;
	init = true;
};

Engine::~Engine()
{

};

int Engine::run(int argc, const char ** argv)
{
	init();
	loop();
	free();	
	return 0;
}; 

int Engine::init()
{
	initPlatform();
	initRenderer();
	onInit();
	return 0;
};

int Engine::free()
{
	onFree();
	freeRenderer();
	freePlatform();
	return 0;
};

int Engine::loop()
{
	 return loopPlatform();
};

Engine * Engine::getInstance()
{
	return instance;
};


