#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Camera.h"
#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Graphics
{
private:
	Camera* m_camera;
	Object* m_cube;
	GLint m_modelMatrix;
	GLint m_projectionMatrix;
	//Shader* m_shader;
	GLint m_viewMatrix;
public:
	bool Initialize(int width, int height)
	{
		GLuint VAO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		m_camera = new Camera();
		m_cube = new Object();
		// 
		//m_shader = new Shader();

		//if (!m_shader->AddShader(GL_VERTEX_SHADER)) {}
		//if (!m_shader->AddShader(GL_FRAGMENT_SHADER)) {}
		//if (!m_shader->Finalize()) {}

		//m_projectionMatrix = m_shader->getUniformLocation("projectionMatrix");
		//m_viewMatrix = m_shader->getUniformLocatino("viewMatrix");
		//m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		return true;
	}

	void Render()
	{
		glClearColor(0.5, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//m_shader->Enable();

		glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render();

		auto error = glGetError();
		if (error != GL_NO_ERROR)
		{
			//std::string val = ErrorString(error);
			//std::cout << "Error Initializing OpenGL!" << error << "," << val << std::endl;
		}
	}

	void Update(unsigned int dt)
	{
		m_cube->Update(dt);
	}
};

#endif