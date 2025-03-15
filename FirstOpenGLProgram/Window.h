#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

#include <iostream>

class Window
{
private:
	GLFWwindow* gWindow;

public:
	Window()
	{
		gWindow = NULL;
	}

	Window(const char* name, int* width, int* height)
	{
		gWindow = NULL;

		if (!glfwInit()) //GLFW Init Failed
		{
			const char* err;
			glfwGetError(&err);
			printf("GLFW Initialization: Failed: %s\n", err);

			gWindow = NULL;
		}
		printf("GLFW Initialization: Succeeded!\n");

		//Create Window
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		gWindow = glfwCreateWindow(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, name, glfwGetPrimaryMonitor(), NULL);

		if (!gWindow) //Window Failed
		{
			printf("Window or Context Creation: Failed!\n");
		}
		printf("Window or Context Creation: Succeeded!\n");

		glfwMakeContextCurrent(gWindow);

		this->Initialize();
	}

	~Window()
	{
		glfwDestroyWindow(gWindow);
		gWindow = NULL;
		glfwTerminate();
	}

	bool Initialize()
	{
		if (glewInit() != GLEW_OK) //Initialize GLEW Libraries
		{
			printf("GLEW Initialization: Failed!\n");
			exit(EXIT_FAILURE);
		}
		printf("GLEW Initialization: Succeeded!\n");
		glfwSwapInterval(1);

		return true;
	}

	int getWindowWidth()
	{
		return glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	}
	int getWindowHeight()
	{
		return glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	}

	GLFWwindow* getWindow()
	{
		return gWindow;
	}

	void Swap()
	{
		glfwSwapBuffers(gWindow);
	}
};

#endif
