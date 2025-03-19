#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

class Graphics
{
private:
	const char* v_shader_source = "#version 460 core\n"
		"layout (location = 0) in vec3 v_position;\n"
		"layout (location = 1) in vec2 v_tex_coords;\n"
		"out vec2 tex_coords;\n"
		"uniform mat4 projectionMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"uniform mat4 modelMatrix;\n"
		"void main() { gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(v_position, 1.0);\n tex_coords = v_tex_coords; }";

	const char* f_shader_source = "#version 460 core\n"
		"out vec4 frag_color;\n"
		"in vec2 tex_coords;\n"
		"uniform sampler2D Texture;"
		"uniform vec3 object_color;\n"
		"uniform vec3 light_color;\n"
		"void main() { frag_color = texture(Texture, tex_coords) * vec4(light_color * object_color, 1.0); }";

	const char* v_light_shader_source = "#version 460 core\n"
		"layout (location = 0) in vec3 v_position;\n"
		"uniform mat4 projectionMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"uniform mat4 modelMatrix;\n"
		"void main() { gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(v_position, 1.0); }";

	const char* f_light_shader_source = "#version 460 core\n"
		"out vec4 frag_color;\n"
		"void main() { frag_color = vec4(1.0); }";

	Camera* m_camera;
	Shader* m_shader;
	Shader* m_light_shader;

	Object* m_quad;
	Object* m_cube;

	Light* m_light;

	GLint m_modelMatrix;
	GLint m_projectionMatrix;
	GLint m_viewMatrix;

	GLint m_lightModelMatrix;
	GLint m_lightProjectionMatrix;
	GLint m_lightViewMatrix;
public:
	bool Initialize(int width, int height)
	{
		GLuint VAO;
		GLuint light_VAO;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenVertexArrays(1, &light_VAO);
		glBindVertexArray(light_VAO);

		m_camera = new Camera();
		if (!m_camera->Initialize(width, height))
		{
			std::cerr << "Error: Camera Could Not Initialize!\n" << std::endl;
			return false;
		}

		m_quad = new Object("quad.txt", "wood_floor.jpg");
		m_cube = new Object("cube.txt", "crate.jpg");

		m_light = new Light("light_cube.txt");

		m_shader = new Shader();
		m_light_shader = new Shader();

		if (!m_shader->Initialize())
		{
			std::cerr << "Error: Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->Initialize())
		{
			std::cerr << "Error: Light Shader Could Not Initialize!\n" << std::endl;
			return false;
		}

		if (!m_shader->AddShader(GL_VERTEX_SHADER, v_shader_source)) 
		{
			std::cerr << "Error: Vertex Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->AddShader(GL_VERTEX_SHADER, v_light_shader_source))
		{
			std::cerr << "Error: Vertex Light Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_shader->AddShader(GL_FRAGMENT_SHADER, f_shader_source)) 
		{
			std::cerr << "Error: Fragment Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->AddShader(GL_FRAGMENT_SHADER, f_light_shader_source))
		{
			std::cerr << "Error: Fragment Light Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		
		if (!m_shader->Finalize()) 
		{
			std::cerr << "Error: Shader Program Failed to Finialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->Finalize())
		{
			std::cerr << "Error: Light Shader Program Failed to Finialize!\n" << std::endl;
			return false;
		}

		m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
		m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
		m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");

		m_lightProjectionMatrix = m_light_shader->GetUniformLocation("projectionMatrix");
		m_lightViewMatrix = m_light_shader->GetUniformLocation("viewMatrix");
		m_lightModelMatrix = m_light_shader->GetUniformLocation("modelMatrix");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		return true;
	}

	void Render()
	{
		glClearColor(0.38, 0.64, 0.47, 1.0); //background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Objects
		m_shader->Enable();

		//Pass Uniforms to Shader
		glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		glUniform3fv(m_shader->GetUniformLocation("object_color"), 1, glm::value_ptr(glm::vec3(1.f, 0.5f, 0.31f)));
		glUniform3fv(m_shader->GetUniformLocation("light_color"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));
		
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_quad->GetModel()));
		m_quad->Render();

		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render();

		//Lights
		m_light_shader->Enable();

		////Pass Uniforms to Light Shader
		glUniformMatrix4fv(m_lightProjectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_lightViewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

		glUniformMatrix4fv(m_lightModelMatrix, 1, GL_FALSE, glm::value_ptr(m_light->GetModel()));
		m_light->Render();

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
	void MoveCameraForward() { m_camera->MoveForward(); }
	void MoveCameraBackward() { m_camera->MoveBackward(); }
	void MoveCameraLeft() { m_camera->MoveLeft(); }
	void MoveCameraRight() { m_camera->MoveRight(); }

	void Update(float dt, glm::vec3 pos, float angle)
	{
		m_camera->UpdateTime(dt);
		m_cube->Update(dt, pos, angle);
	}
};
#endif