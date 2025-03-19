#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include "Main_Header.h"
#include "Window.h"
#include "Graphics.h"

class Engine 
{
private:
	Graphics* m_graphics;
	Window* m_window;

	const char* m_WINDOW_NAME;
	int m_WINDOW_WIDTH;
	int m_WINDOW_HEIGHT;

	bool m_running;
	bool m_FULLSCREEN;

	float delta_time = 0.0f;
	float last_frame = 0.0f;

	glm::vec3 position;
	float rotation;

public:
	Engine(const char* name, int width, int height)
	{
		m_WINDOW_NAME = name;
		m_WINDOW_WIDTH = width;
		m_WINDOW_HEIGHT = height;
	}

	~Engine()
	{
		delete m_window;
		delete m_graphics;
		m_window = NULL;
		m_graphics = NULL;
	}

	bool Initialize()
	{
		//Start Window
		m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
		if (!m_window->Initialize())
		{
			std::cerr << "The window failed to Initalize!" << std::endl;
			return false;
		}

		//Start Graphics
		m_graphics = new Graphics();
		if (!m_graphics->Initialize(m_window->getWindowWidth(), m_window->getWindowHeight()))
		{
			std::cerr << "The graphics failed to Initalize!" << std::endl;
			return false;
		}
		
		return true;
	}

	void Run()
	{
		m_running = true;

		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		while (!glfwWindowShouldClose(m_window->getWindow()))
		{
			ProcessInput();
			Display(m_window->getWindow(), delta_time);
			glfwPollEvents();
		}

		m_running = false;
	}

	void ProcessInput()
	{
		//User Controlled Camera
		if (glfwGetMouseButton(m_window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			double x_pos, y_pos;

			glfwGetCursorPos(m_window->getWindow(), &x_pos, &y_pos);
			glfwSetCursorPos(m_window->getWindow(), m_window->getWindowWidth() / 2, m_window->getWindowHeight() / 2);
			glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			m_graphics->UpdateMousePos(x_pos, y_pos);
		}
		else if (glfwGetMouseButton(m_window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) { m_graphics->MoveCameraForward(); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) { m_graphics->MoveCameraBackward(); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) { m_graphics->MoveCameraLeft(); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) { m_graphics->MoveCameraRight(); }

		//Exit Window
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_window->getWindow(), true);
		}

		//Translate and Rotate Object
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		{
			position.y++;
		}
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			position.y--;
		}
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			position.x++;
		}
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			position.x--;
		}
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_P) == GLFW_PRESS)
		{
			rotation--;
		}
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_O) == GLFW_PRESS)
		{
			rotation++;
		}
	}


	void Display(GLFWwindow* window, double time)
	{
		m_graphics->Render();
		m_window->Swap();
		m_graphics->Update(time, position, rotation);
	}
};

#endif