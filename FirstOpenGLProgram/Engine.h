#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include "Main_Header.h"
#include "Window.h"
#include "Graphics.h"

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
const float MAX_FOV = 80.0f;
float fov = MAX_FOV;

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
	float delta_time_2 = 0.f;
	float last_frame = 0.0f;

	double last_mouse_x, last_mouse_y;
	bool first_click = true;

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

		glfwSetScrollCallback(m_window->getWindow(), scroll_callback);
		
		return true;
	}

	void Run()
	{
		m_running = true;

		while (!glfwWindowShouldClose(m_window->getWindow()))
		{
			float current_frame = glfwGetTime();
			delta_time = current_frame - last_frame;
			last_frame = current_frame;

			ProcessInput();
			Display(m_window->getWindow(), delta_time);
			glfwPollEvents();
		}

		m_running = false;
	}

	void Display(GLFWwindow* window, double time)
	{
		m_graphics->Update(time, fov);
		m_graphics->Render(time);
		m_window->Swap();
	}

	void ProcessInput()
	{
		//User Controlled Camera
		if (glfwGetMouseButton(m_window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			double x_pos, y_pos;

			glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			if (first_click)
			{
				glfwSetCursorPos(m_window->getWindow(), m_window->getWindowWidth() / 2, m_window->getWindowHeight() / 2);
				first_click = false;
			}

			glfwGetCursorPos(m_window->getWindow(), &x_pos, &y_pos);
			glfwSetCursorPos(m_window->getWindow(), m_window->getWindowWidth() / 2, m_window->getWindowHeight() / 2);
			
			m_graphics->UpdateMousePos(x_pos, y_pos, delta_time);
		}
		else if (glfwGetMouseButton(m_window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			first_click = true;
		}

		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) { m_graphics->MoveCameraForward(delta_time); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) { m_graphics->MoveCameraBackward(delta_time); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) { m_graphics->MoveCameraLeft(delta_time); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) { m_graphics->MoveCameraRight(delta_time); }

		//Exit Window
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_window->getWindow(), true);
		}
	}
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;

	//Clamp FOV
	if (fov < 1.0f)
	{
		fov = 1.0f;
	}
	if (fov > MAX_FOV)
	{
		fov = MAX_FOV;
	}
}

#endif