#include <iostream>

#include "Engine.h"

int main(int argc, char** argv)
{
	Engine* engine = new Engine("OpenGL Cube Renderer", 800, 600);

	if (!engine->Initialize())
	{
		printf("The Engine Failed to Start! \n");
		delete engine;
		engine = NULL;
		return 1;
	}

	engine->Run();
	delete engine;
	engine = NULL;

	return 0;
}