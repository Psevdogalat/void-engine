#include <engine.hpp>
#include <cstdio>

class myEngine: VoidEngine::Engine
{
	int onInit();
	int onFree();
};

int myEngine::onInit()
{
	printf("onInit\n");
	return 0;
}

int myEngine::onFree()
{
	printf("onFree\n");
	return 0;
}

int main(int argc, char ** argv)
{
	myEngine engine;
	return engine.run(argc, argv);	
}
