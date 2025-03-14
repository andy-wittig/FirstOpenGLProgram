#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <fstream>

#include "Camera.h"
#include "Object.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::ifstream cube_file("cube.txt");

class Graphics
{
private:
	const char* v_shader_source = "#version 460 core\n"
		"layout (location = 0) in vec3 v_position;\n"
		"layout (location = 1) in vec3 v_color;\n"
		"smooth out vec3 color;\n"
		"uniform mat4 projectionMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"uniform mat4 modelMatrix;\n"
		"void main() { vec4 v = vec4(v_position, 1.0);\n gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * v;\n color = v_color; }";
	
	const char* f_shader_source = "#version 460 core\n"
		"smooth in vec3 color;\n"
		"out vec4 frag_color;\n"
		"void main() { frag_color = vec4(color.rgb, 1.0); }";

	Camera* m_camera;
	Shader* m_shader;

	Object* m_floor;
	Object* m_cube;

	GLint m_modelMatrix;
	GLint m_projectionMatrix;
	GLint m_viewMatrix;
public:
	bool Initialize(int width, int height)
	{
		GLuint VAO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		m_camera = new Camera();
		if (!m_camera->Initialize(width, height))
		{
			std::cerr << "Error: Camera Could Not Initialize!\n" << std::endl;
			return false;
		}

		m_floor = new Object(cube_file);
		m_cube = new Object(cube_file);

		m_shader = new Shader();
		if (!m_shader->Initialize())
		{
			std::cerr << "Error: Shader Could Not Initialize!\n" << std::endl;
			return false;
		}

		if (!m_shader->AddShader(GL_VERTEX_SHADER, v_shader_source)) 
		{
			std::cerr << "Error: Vertex Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_shader->AddShader(GL_FRAGMENT_SHADER, f_shader_source)) 
		{
			std::cerr << "Error: Fragment Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_shader->Finalize()) 
		{
			std::cerr << "Error: Program Failed to Finialize!\n" << std::endl;
			return false;
		}

		m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
		m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
		m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		return true;
	}

	void Render()
	{
		glClearColor(0.0, 0.2, 0.5, 1.0); //background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_shader->Enable();

		//Give projection and view matrix to shader
		glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		
		//Render Objects
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_floor->GetModel()));
		m_floor->Render();

		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render();

		auto error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::string val = ErrorString(error);
			std::cout << "Error Initializing OpenGL!" << error << "," << val << std::endl;
		}
	}

	std::string ErrorString(GLenum error)
	{
		if (error == GL_INVALID_ENUM)
		{
			return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
		}

		else if (error == GL_INVALID_VALUE)
		{
			return "GL_INVALID_VALUE: A numeric argument is out of range.";
		}

		else if (error == GL_INVALID_OPERATION)
		{
			return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
		}

		else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
		{
			return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
		}

		else if (error == GL_OUT_OF_MEMORY)
		{
			return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
		}
		else
		{
			return "None";
		}
	}

	void UpdateMousePos(double x, double y)
	{
		m_camera->MouseLook(x, y);
	}

	void Update(unsigned int dt, glm::vec3 pos, float angle)
	{
		m_cube->Update(dt, pos, angle);
	}
};
#endif