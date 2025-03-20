#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Main_Header.h"
#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "Shader.h"

std::string processShaderFile(const std::string& shader_file_name)
{
	std::ifstream shader_file(shader_file_name);

	if (shader_file.is_open())
	{
		std::stringstream buffer;
		buffer << shader_file.rdbuf();
		shader_file.close();
		return buffer.str();
	}
	else
	{
		return ""; //file could not be processed
	}
}

class Graphics
{
private:
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

		if (!m_shader->AddShader(GL_VERTEX_SHADER, processShaderFile("v_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Vertex Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->AddShader(GL_VERTEX_SHADER, processShaderFile("v_light_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Vertex Light Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_shader->AddShader(GL_FRAGMENT_SHADER, processShaderFile("f_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Fragment Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->AddShader(GL_FRAGMENT_SHADER, processShaderFile("f_light_shader_source.txt").c_str()))
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

		m_quad = new Object();
		m_cube = new Object();

		m_quad->Initialize("quad.txt", "wood_floor.png", "wood_floor_specular_map.png");
		m_cube->Initialize("cube.txt", "crate.png", "crate_specular_map.png");
		m_cube->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

		m_light = new Light();

		m_light->Initialize("light_cube.txt");
		m_light->setPosition(glm::vec3(5.f, 3.f, 5.f));

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
		glClearColor(0.17, 0.12, 0.19, 1.0); //background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_shader->Enable();

		glUniform1i(m_shader->GetUniformLocation("material.diffuse"), 0);
		glUniform1i(m_shader->GetUniformLocation("material.specular"), 1);

		//Pass Uniforms to Shaders
		glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

		//Light Properties
		glUniform3fv(m_shader->GetUniformLocation("light.position"), 1, glm::value_ptr(m_light->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("view_pos"), 1, glm::value_ptr(m_camera->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("light.ambient"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
		glUniform3fv(m_shader->GetUniformLocation("light.diffuse"), 1, glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.9f)));
		glUniform3fv(m_shader->GetUniformLocation("light.specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		
		//Object Render
		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 65.0f);
		
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_quad->GetModel()));
		m_quad->Render();

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 50.0f);

		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render();

		//Light Render
		m_light_shader->Enable();

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

	void Update(float dt, glm::vec3 pos, float angle, float fov)
	{
		m_camera->UpdateTime(dt);
		m_camera->setFOV(fov);
		m_cube->Update(dt, pos, angle);
	}
};
#endif