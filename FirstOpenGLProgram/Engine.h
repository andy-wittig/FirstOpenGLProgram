#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include "Main_Header.h"
#include "Window.h"
#include "Graphics.h"

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
const float MAX_FOV = 80.0f;
float fov = MAX_FOV;
float EXTRA_FOV = 10.f;
float temp_fov = fov;
float zoom_amount;
const float ZOOM_SPEED = 20;
const float EXTRA_CAMERA_SPEED = .2f;
bool accelerate_mode = false;

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
	bool first_press;

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
		glfwSetKeyCallback(m_window->getWindow(), key_callback);
		
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
		m_graphics->Render();
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

		//Control Movement
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) { m_graphics->MoveCameraForward(delta_time); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) { m_graphics->MoveCameraBackward(delta_time); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) { m_graphics->MoveCameraLeft(delta_time); }
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) { m_graphics->MoveCameraRight(delta_time); }

		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{	
			m_graphics->setExtraCameraSpeed(EXTRA_CAMERA_SPEED);
			if (!accelerate_mode)
			{
				temp_fov = fov;
				accelerate_mode = true;
			}
			fov = glm::mix(fov, temp_fov + EXTRA_FOV, 10 * delta_time);
		}
		else 
		{
			m_graphics->setExtraCameraSpeed(0.f);
			if (accelerate_mode)
			{
				fov = glm::mix(fov, temp_fov, 10 * delta_time);
				if (abs(fov - temp_fov) <= 0.1f) { accelerate_mode = false; }
			}
		}

		int state = glfwGetKey(m_window->getWindow(), GLFW_KEY_V);
		if (state == GLFW_PRESS && !first_press)
		{
			m_graphics->visitPlanet();
			first_press = true;
		}
		else if (state == GLFW_RELEASE)
		{
			if (first_press) { zoom_amount = 0; } //reset zoom
			first_press = false;
		}

		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		{
			zoom_amount += ZOOM_SPEED * delta_time;
			zoom_amount = glm::clamp(zoom_amount, 0.f, 100.f);
			m_graphics->setZoomDistance(zoom_amount);
		}
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			zoom_amount -= ZOOM_SPEED * delta_time;
			zoom_amount = glm::clamp(zoom_amount, 0.f, 100.f);
			m_graphics->setZoomDistance(zoom_amount);
		}

		//Exit Window
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_window->getWindow(), true);
		}
	}
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//get keys!
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!accelerate_mode)
	{
		fov -= (float)yoffset;
		fov = glm::clamp(fov, 40.f, MAX_FOV);
	}
	else
	{
		fov -= (float)yoffset;
		fov = glm::clamp(fov, 40.f, MAX_FOV + EXTRA_FOV);
	}
}

#endif