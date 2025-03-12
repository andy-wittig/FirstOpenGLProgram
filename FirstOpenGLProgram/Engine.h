#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <string>

#include "Window.h"
#include "Graphics.h"

class Engine 
{
private:
	Graphics* m_graphics;
	Window* m_window;

	bool m_running;
	bool m_FULLSCREEN;
	
	const char* m_WINDOW_NAME;
	int m_WINDOW_WIDTH;
	int m_WINDOW_HEIGHT;

public:
	Engine(const char* window_name, int window_width, int window_height) : m_WINDOW_NAME(window_name), m_WINDOW_WIDTH(window_width), m_WINDOW_HEIGHT(window_height)
	{
		//constructor
	}

	bool Initialize()
	{
		//Start Window
		m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
		if (!m_window->Initialize())
		{
			printf("The window failed to Initalize!\n");
			return false;
		}

		//Start Graphics
		m_graphics = new Graphics();
		if (!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
		{
			printf("The graphics failed to Initalize!\n");
			return false;
		}

		return true;
	}

	void Run()
	{
		m_running = true;

		while (!glfwWindowShouldClose(m_window->getWindow()))
		{
			ProcessInput();
			Display(m_window->getWindow(), glfwGetTime());
			glfwPollEvents();
		}
		m_running = false;
	}

	void ProcessInput()
	{
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_window->getWindow(), true);
		}
	}

	void Display(GLFWwindow* window, double time)
	{
		m_graphics->Render();
		m_window->Swap();
		m_graphics->Update(time);
	}

};

#endif