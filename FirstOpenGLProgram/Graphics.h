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
	Object* m_pyramid;
	Object* m_crystal;

	Light* m_light;

	GLint m_modelMatrix;
	GLint m_projectionMatrix;
	GLint m_viewMatrix;

	GLint m_lightModelMatrix;
	GLint m_lightProjectionMatrix;
	GLint m_lightViewMatrix;

	glm::mat4 tmat;
	glm::mat4 rmat;
	glm::mat4 smat;
	glm::mat4 t_offset;
	glm::mat4 r_offset;
	std::stack<glm::mat4> transformation_stack;

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
		m_pyramid = new Object();
		m_crystal = new Object();

		m_quad->Initialize("quad.txt", "wood_floor.png", "wood_floor_specular_map.png");
		m_cube->Initialize("cube.txt", "crate.png", "crate_specular_map.png");
		m_pyramid->Initialize("pyramid.txt", "pyramid.png", "pyramid_specular_map.png");
		m_crystal->Initialize("crystal.txt", "crystal.png", "crystal_specular_map.png");

		srand(time(0)); //Update seed of random number generator based on current time

		//Random Positioning
		float angle = glm::linearRand(0.0f, 360.0f);
		float tvec1 = glm::linearRand(-5.0f, 5.0f);
		float tvec2 = glm::linearRand(1.0f, 5.0f);
		float tvec3 = glm::linearRand(-5.0f, 5.0f);

		glm::vec3 axis;
		int rand_axis_choice = rand() % 3; //range from 0-2

		switch (rand_axis_choice)
		{
		case 0: //x axis of rotation
			axis = glm::vec3(1.0f, 0.0f, 0.0f);
			break;
		case 1: //y axis of rotation
			axis = glm::vec3(0.0f, 1.0f, 0.0f);
			break;
		case 2: //z axis of rotation
			axis = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
		}

		t_offset = glm::translate(glm::mat4(1.f), glm::vec3(tvec1, tvec2, tvec3));
		r_offset = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);

		m_quad->setPosition(glm::vec3(0.0f, -5.0f, 0.0f));

		m_light = new Light();

		m_light->Initialize("light_cube.txt");
		m_light->setPosition(glm::vec3(0.f, 6.f, 0.f));

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
		glUniform3fv(m_shader->GetUniformLocation("light.diffuse"), 1, glm::value_ptr(glm::vec3(0.8f, 0.6f, 0.2f)));
		glUniform3fv(m_shader->GetUniformLocation("light.specular"), 1, glm::value_ptr(glm::vec3(0.8f, 0.6f, 0.2f)));
		
		//Object Rendering
		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 20.0f);
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_quad->getModel()));
		m_quad->Render();

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 65.0f);
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->getModel()));
		m_cube->Render();

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 40.0f);
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_pyramid->getModel()));
		m_pyramid->Render();

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 80.0f);
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_crystal->getModel()));
		m_crystal->Render();

		//Light Rendering
		m_light_shader->Enable();

		glUniformMatrix4fv(m_lightProjectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_lightViewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

		glUniformMatrix4fv(m_lightModelMatrix, 1, GL_FALSE, glm::value_ptr(m_light->getModel()));
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

	void computeTransforms(double dt, std::vector<float> speed, std::vector<float> dist, std::vector<float> rotation_speed, std::vector<float> scale,
		glm::vec3 rotation_vector, glm::mat4 &tmat, glm::mat4 &rmat, glm::mat4 &smat)
	{
		double elapsed_time = glfwGetTime();
		tmat = glm::translate(glm::mat4(1.f), glm::vec3(cos(speed[0] * elapsed_time) * dist[0], sin(speed[1] * elapsed_time) * dist[1], sin(speed[2] * elapsed_time) * dist[2]));
		rmat = glm::rotate(glm::mat4(1.f), rotation_speed[0] * (float)elapsed_time, rotation_vector);
		smat = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	}

	void Update(double dt, glm::vec3 pos, float angle, float fov)
	{ //Objects transform should be updated here so different objects can move independently.
		m_camera->UpdateTime(dt);
		m_camera->setFOV(fov);

		//pyramid transform
		computeTransforms(dt, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.25f, 0.0f, 0.25f }, { 1.f, 1.f, 1.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		transformation_stack.push(tmat);
		transformation_stack.push(rmat);
		transformation_stack.push(smat);

		//cube transform
		computeTransforms(dt, { 0.35f, 0.0f, 0.35f }, { 6.f, 0.0f, 6.0f }, { 0.25f, 0.0f, 0.25f }, { 1.f, 1.f, 1.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		transformation_stack.push(tmat);
		transformation_stack.push(rmat);
		transformation_stack.push(smat);

		//crystal transform
		computeTransforms(dt, { 1.f, 1.f, 0.f }, { 3.f, 3.f, 0.f }, { 0.25f, 0.0f, 0.25f }, { .25f, .25f, .25f }, glm::vec3(0.f, 1.f, 0.f), tmat, rmat, smat);
		transformation_stack.push(tmat);
		transformation_stack.push(rmat);
		transformation_stack.push(smat);

		//Stack
		getTransformStack(tmat, rmat, smat);
		glm::mat4 crystal_matrix = tmat * rmat * smat;

		getTransformStack(tmat, rmat, smat);
		glm::mat4 cube_matrix = tmat * rmat * smat;

		getTransformStack(tmat, rmat, smat);
		glm::mat4 pyramid_matrix = t_offset * r_offset * tmat * rmat * smat;

		m_pyramid->Update(pyramid_matrix);
		m_cube->Update(pyramid_matrix * cube_matrix);
		m_crystal->Update(pyramid_matrix * cube_matrix * crystal_matrix);
	}

	void getTransformStack(glm::mat4 &tmat, glm::mat4 &rmat, glm::mat4 &smat)
	{
		smat = transformation_stack.top();
		transformation_stack.pop();
		rmat = transformation_stack.top();
		transformation_stack.pop();
		tmat = transformation_stack.top();
		transformation_stack.pop();
	}
};
#endif