#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Main_Header.h"
#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "Cube_Map.h"
#include "Shader.h"
#include "Model.h"

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
	Shader* m_instance_shader;
	Shader* m_light_shader;
	Shader* m_skybox_shader;

	Model* m_point_light1;
	Model* m_point_light2;
	Model* m_dir_light;

	Model* m_spaceship;
	Model* m_sun;
	Model* m_earth;
	Model* m_moon;

	CubeMap* m_skybox;

	glm::mat4 spaceship_tmat;
	glm::mat4 spaceship_rmat;
	glm::mat4 spaceship_smat;

	glm::mat4 tmat;
	glm::mat4 rmat;
	glm::mat4 smat;
	glm::mat4 t_offset;
	glm::mat4 r_offset;
	std::stack<glm::mat4> transformation_stack;

	//Asteroid Instance Variables
	static const int asteroid_amount = 250;
	std::vector<glm::mat4> asteroidMatrices;
	Model* m_asteroid;

	void setShaderLights(Shader *shader)
	{
		glUniform3fv(shader->GetUniformLocation("view_pos"), 1, glm::value_ptr(m_camera->getPosition()));
		glUniform1f(shader->GetUniformLocation("material.alpha"), 1.0);

		//Lights
		glUniform3fv(shader->GetUniformLocation("dir_light.direction"), 1, glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.ambient"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.diffuse"), 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, 0.6f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.specular"), 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));

		glUniform3fv(shader->GetUniformLocation("point_lights[0].position"), 1, glm::value_ptr(m_spaceship->getPosition()));
		glUniform3fv(shader->GetUniformLocation("point_lights[0].ambient"), 1, glm::value_ptr(glm::vec3(.1f, .1f, .1f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[0].diffuse"), 1, glm::value_ptr(glm::vec3(5.f, 5.f, 10.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[0].specular"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));
		glUniform1f(shader->GetUniformLocation("point_lights[0].constant"), 0.25f);
		glUniform1f(shader->GetUniformLocation("point_lights[0].linear"), 0.09f);
		glUniform1f(shader->GetUniformLocation("point_lights[0].quadratic"), 0.032f);

		glUniform3fv(shader->GetUniformLocation("point_lights[1].position"), 1, glm::value_ptr(m_point_light1->getPosition()));
		glUniform3fv(shader->GetUniformLocation("point_lights[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[1].diffuse"), 1, glm::value_ptr(glm::vec3(5.f, 5.f, 5.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[1].specular"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));
		glUniform1f(shader->GetUniformLocation("point_lights[1].constant"), 1.0f);
		glUniform1f(shader->GetUniformLocation("point_lights[1].linear"), 0.09f);
		glUniform1f(shader->GetUniformLocation("point_lights[1].quadratic"), 0.032f);

		glUniform3fv(shader->GetUniformLocation("point_lights[2].position"), 1, glm::value_ptr(m_point_light2->getPosition()));
		glUniform3fv(shader->GetUniformLocation("point_lights[2].ambient"), 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[2].diffuse"), 1, glm::value_ptr(glm::vec3(5.f, 5.f, 5.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[2].specular"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));
		glUniform1f(shader->GetUniformLocation("point_lights[2].constant"), 1.0f);
		glUniform1f(shader->GetUniformLocation("point_lights[2].linear"), 0.09f);
		glUniform1f(shader->GetUniformLocation("point_lights[2].quadratic"), 0.032f);
	}

public:
	bool Initialize(int width, int height)
	{
		GLuint VAO;
		GLuint light_VAO;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenVertexArrays(1, &light_VAO);
		glBindVertexArray(light_VAO);

		//Initialize Camera
		m_camera = new Camera();
		if (!m_camera->Initialize(width, height))
		{
			std::cerr << "Error: Camera Could Not Initialize!\n" << std::endl;
			return false;
		}

		//Initialize Shaders
		m_shader = new Shader();
		m_instance_shader = new Shader();
		m_light_shader = new Shader();
		m_skybox_shader = new Shader();

		if (!m_shader->Initialize())
		{
			std::cerr << "Error: Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_instance_shader->Initialize())
		{
			std::cerr << "Error: Instance Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->Initialize())
		{
			std::cerr << "Error: Light Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_skybox_shader->Initialize())
		{
			std::cerr << "Error: Cube Map Shader Could Not Initialize!\n" << std::endl;
			return false;
		}

		if (!m_shader->AddShader(GL_VERTEX_SHADER, processShaderFile("v_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Vertex Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_instance_shader->AddShader(GL_VERTEX_SHADER, processShaderFile("v_instance_shader.txt").c_str()))
		{
			std::cerr << "Error: Vertex Instance Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->AddShader(GL_VERTEX_SHADER, processShaderFile("v_light_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Vertex Light Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_skybox_shader->AddShader(GL_VERTEX_SHADER, processShaderFile("v_cube_map_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Vertex Cube Map Shader Could Not Initialize!\n" << std::endl;
			return false;
		}

		if (!m_shader->AddShader(GL_FRAGMENT_SHADER, processShaderFile("f_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Fragment Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_instance_shader->AddShader(GL_FRAGMENT_SHADER, processShaderFile("f_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Fragment Instance Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->AddShader(GL_FRAGMENT_SHADER, processShaderFile("f_light_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Fragment Light Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		if (!m_skybox_shader->AddShader(GL_FRAGMENT_SHADER, processShaderFile("f_cube_map_shader_source.txt").c_str()))
		{
			std::cerr << "Error: Fragment Cube Map Shader Could Not Initialize!\n" << std::endl;
			return false;
		}
		
		if (!m_shader->Finalize())
		{
			std::cerr << "Error: Shader Program Failed to Finialize!\n" << std::endl;
			return false;
		}
		if (!m_instance_shader->Finalize())
		{
			std::cerr << "Error: Instance Shader Program Failed to Finialize!\n" << std::endl;
			return false;
		}
		if (!m_light_shader->Finalize())
		{
			std::cerr << "Error: Light Shader Program Failed to Finialize!\n" << std::endl;
			return false;
		}
		if (!m_skybox_shader->Finalize())
		{
			std::cerr << "Error: Cube Map Shader Program Failed to Finialize!\n" << std::endl;
			return false;
		}

		srand(time(0)); //Update seed of random number generator based on current time.

		//-------------------- Asteroid
		float radius = 75.0;
		float offset = 14.f;
		for (int i = 0; i < asteroid_amount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);

			//Random Translation
			float angle = (float)(i) / (float)(asteroid_amount) * 360.f;

			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = cos(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.1f;
			
			model = glm::translate(model, glm::vec3(x, y, z));

			//Random Scale
			float scale = (rand() % 6) / 10.f + 0.5f;
			model = glm::scale(model, glm::vec3(scale));

			//Random Rotation
			float rotation_angle = (rand() % 361);
			model = glm::rotate(model, rotation_angle, glm::vec3(0.f, 1.f, 0.f));

			asteroidMatrices.push_back(model);
		}
		m_asteroid = new Model("models/asteroid/asteroid.obj", asteroidMatrices);
		//--------------------
		
		//-------------------- Solar System
		float angle = glm::linearRand(0.0f, 360.0f);
		float tvec1 = glm::linearRand(-5.0f, 5.0f);
		float tvec2 = glm::linearRand(1.0f, 5.0f);
		float tvec3 = glm::linearRand(-5.0f, 5.0f);

		glm::vec3 axis;
		int rand_axis_choice = rand() % 3; //Ranges from (0-2).

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

		m_sun = new Model("models/Sun/Sun.obj");
		m_earth = new Model("models/Earth/Earth.obj");
		m_moon = new Model("models/Moon/Moon.obj");
		//--------------------

		//-------------------- Lights
		m_point_light1 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light1->setPosition(glm::vec3(-25.f, 0.f, 0.f));
		m_point_light1->setScale(glm::vec3(0.6f, 0.6f, 0.6f));

		m_point_light2 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light2->setPosition(glm::vec3(25.f, 0.f, 0.f));
		m_point_light2->setScale(glm::vec3(0.6f, 0.6f, 0.6f));

		m_dir_light = new Model("models/lightbulb/lightbulb.obj");
		m_dir_light->setPosition(glm::vec3(0.f, 35.f, 0.f));
		m_dir_light->setScale(glm::vec3(0.6f, 0.6f, 0.6f));
		//--------------------

		//-------------------- Cube Maps
		m_skybox = new CubeMap();
		std::vector<std::string> sky_box_faces = 
		{ 
			"skybox/right.png",
			"skybox/left.png",
			"skybox/top.png",
			"skybox/bottom.png",
			"skybox/front.png",
			"skybox/back.png"
		};
		m_skybox->Initialize("skybox.txt", sky_box_faces);
		//--------------------

		//Initialize Models
		m_spaceship = new Model("models/carrier/carrier.obj");
		m_spaceship->setPosition(glm::vec3(0.f, 2.f, -10.f));

		//GL Settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);

		return true;
	}

	void Render()
	{
		glClearColor(0.17, 0.12, 0.19, 1.0); //background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-------------------- Render Models
		m_shader->Enable();
		setShaderLights(m_shader);
		glUniformMatrix4fv(m_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

		//Space Ship
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_spaceship->getModel()));
		m_spaceship->Render(*m_shader);

		//Planets
		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 50.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_sun->getModel()));
		m_sun->Render(*m_shader);

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 25.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_earth->getModel()));
		m_earth->Render(*m_shader);

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 15.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_moon->getModel()));
		m_moon->Render(*m_shader);
		//--------------------

		//-------------------- Render Instances
		m_instance_shader->Enable();
		setShaderLights(m_instance_shader);
		glUniform1f(m_instance_shader->GetUniformLocation("material.shininess"), 45.f);
		glUniformMatrix4fv(m_instance_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_instance_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		m_asteroid->Render(*m_instance_shader);
		//--------------------

		//-------------------- Render Lights
		m_light_shader->Enable();
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_dir_light->getModel()));
		m_dir_light->Render(*m_light_shader);
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_point_light1->getModel()));
		m_point_light1->Render(*m_light_shader);
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_point_light2->getModel()));
		m_point_light2->Render(*m_light_shader);
		//--------------------
		
		//Render Cube Map
		glDepthFunc(GL_LEQUAL);
		m_skybox_shader->Enable();
		glUniformMatrix4fv(m_skybox_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_skybox_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(m_camera->GetView()))));
		m_skybox->Render();
		glDepthFunc(GL_LESS);

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

	void Update(double dt, float fov)
	{ //Objects transform should be updated here so different objects can move independently.
		m_camera->UpdateTime(dt);
		m_camera->setFOV(fov);

		//Spaceship transform
		double elapsed_time = glfwGetTime();
		float speed = 0.05f;
		float dist = 75.f;

		glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 direction = glm::vec3(cos(speed * elapsed_time) * dist, 8.f, sin(speed * elapsed_time) * dist);
		glm::vec3 tangent = glm::normalize(glm::cross(direction, up));
		float angle = glm::atan2(tangent.x, tangent.z);

		spaceship_tmat = glm::translate(glm::mat4(1.f), direction);
		spaceship_rmat = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f));
		spaceship_smat = glm::scale(glm::vec3(.25f, .25f, .25f));
		m_spaceship->Update(spaceship_tmat * spaceship_rmat * spaceship_smat);

		//sun transform
		computeTransforms(dt, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.05f, 0.0f, 0.05f }, { 1.f, 1.f, 1.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		transformation_stack.push(t_offset * r_offset * tmat * rmat * smat);

		//earth transform
		computeTransforms(dt, { 0.10f, 0.0f, 0.10f }, { 25.f, 0.0f, 25.0f }, { 0.15f, 0.0f, 0.15f }, { 1.f, 1.f, 1.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		tmat *= glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f)); //adjust cube's offset
		transformation_stack.push(transformation_stack.top() * tmat * rmat * smat);

		//moon transform
		computeTransforms(dt, { 0.5f, 0.5f, 0.f }, { 3.f, 3.f, 0.f }, { 0.15f, 0.0f, 0.15f }, { .3f, .3f, .3f }, glm::vec3(0.f, 1.f, 0.f), tmat, rmat, smat);
		transformation_stack.push(transformation_stack.top() * tmat * rmat * smat);

		//Stack
		m_moon->Update(transformation_stack.top());
		transformation_stack.pop();
		m_earth->Update(transformation_stack.top());
		transformation_stack.pop();
		m_sun->Update(transformation_stack.top());
		transformation_stack.pop();
	}
};
#endif