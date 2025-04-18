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

float lerp(float start, float end, float f)
{
	return start * (1.0 - f) + (end * f);
}

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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

class Graphics
{
private:
	Camera* m_camera;

	//Shaders
	Shader* m_shader;
	Shader* m_instance_shader;
	Shader* m_light_shader;
	Shader* m_skybox_shader;
	Shader* m_blur_shader;
	Shader* m_hdr_shader;

	//Light Models
	Model* m_point_light0;
	Model* m_point_light1;
	Model* m_point_light2;
	Model* m_dir_light;

	//Scene Models
	Model* m_spaceship;
	Model* m_player_ship;
	Model* m_sun;
	Model* m_earth;
	Model* m_moon;

	CubeMap* m_skybox;

	//Transformations
	glm::mat4 player_tmat;
	glm::mat4 player_rmat;
	glm::mat4 player_smat;

	glm::mat4 spaceship_tmat;
	glm::mat4 spaceship_rmat;
	glm::mat4 spaceship_smat;

	glm::mat4 tmat;
	glm::mat4 rmat;
	glm::mat4 smat;
	glm::mat4 t_offset;
	glm::mat4 r_offset;
	std::stack<glm::mat4> transformation_stack;

	//Frame Buffers
	unsigned int hdrFBO;
	unsigned int pingpongFBO[2];
	unsigned int colorBuffers[2];
	unsigned int pingpongColorBuffers[2];
	unsigned int rboDepth;

	//Asteroid Instance Variables
	static const int asteroid_amount = 300;
	std::vector<glm::mat4> asteroidMatrices;
	Model* m_asteroid;

	//Player Ship
	int screen_width;
	int screen_height;

	const float ROLL_MAX = 30.f;
	const float ROLL_DECAY = .001;
	float roll = 0.f;
	float roll_speed = 0.1f;

	const float PITCH_MAX = 15.f;
	const float PITCH_DECAY = .0025;
	float pitch = 0.f;
	float pitch_speed = 0.05f;

	void setShaderLights(Shader *shader)
	{
		glUniform3fv(shader->GetUniformLocation("dir_light.direction"), 1, glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.ambient"), 1, glm::value_ptr(glm::vec3(0.08f, 0.08f, 0.08f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.diffuse"), 1, glm::value_ptr(glm::vec3(.8f, .8f, .8f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.specular"), 1, glm::value_ptr(glm::vec3(0.4f, 0.4f, 0.4f)));

		glUniform3fv(shader->GetUniformLocation("point_lights[0].ambient"), 1, glm::value_ptr(glm::vec3(.1f, .1f, .1f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[0].diffuse"), 1, glm::value_ptr(glm::vec3(5.f, 5.f, 10.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[0].specular"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));
		glUniform1f(shader->GetUniformLocation("point_lights[0].constant"), 0.25f);
		glUniform1f(shader->GetUniformLocation("point_lights[0].linear"), 0.09f);
		glUniform1f(shader->GetUniformLocation("point_lights[0].quadratic"), 0.032f);

		glUniform3fv(shader->GetUniformLocation("point_lights[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[1].diffuse"), 1, glm::value_ptr(glm::vec3(5.f, 5.f, 5.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[1].specular"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));
		glUniform1f(shader->GetUniformLocation("point_lights[1].constant"), 1.f);
		glUniform1f(shader->GetUniformLocation("point_lights[1].linear"), 0.09f);
		glUniform1f(shader->GetUniformLocation("point_lights[1].quadratic"), 0.032f);

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
		screen_width = width;
		screen_height = height;

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
		m_blur_shader = new Shader;
		m_hdr_shader = new Shader();

		std::map<Shader*, std::pair<std::string, std::string>> shader_map
		{
			{m_shader, {"v_shader_source.txt", "f_shader_source.txt"}},
			{m_instance_shader, {"v_instance_shader.txt", "f_shader_source.txt"}},
			{m_light_shader, {"v_light_shader_source.txt", "f_light_shader_source.txt"}},
			{m_skybox_shader, {"v_cube_map_shader_source.txt", "f_cube_map_shader_source.txt"}},
			{m_blur_shader, {"v_hdr_shader.txt", "f_blur_shader.txt"}},
			{m_hdr_shader, {"v_hdr_shader.txt", "f_hdr_shader.txt"}}
		};

		for (const auto& shader_entry : shader_map)
		{
			Shader* target_shader = shader_entry.first;
			const auto& shader_source = shader_entry.second;

			if (!target_shader->Initialize())
			{
				std::cerr << "Error: Shader Could Not Initialize!\n" << std::endl;
				return false;
			}
			if (!target_shader->AddShader(GL_VERTEX_SHADER, processShaderFile(shader_source.first).c_str()))
			{
				std::cerr << "Error: Vertex Shader Could Not Initialize!\n" << std::endl;
				return false;
			}
			if (!target_shader->AddShader(GL_FRAGMENT_SHADER, processShaderFile(shader_source.second).c_str()))
			{
				std::cerr << "Error: Fragment Shader Could Not Initialize!\n" << std::endl;
				return false;
			}
			if (!target_shader->Finalize())
			{
				std::cerr << "Error: Shader Program Failed to Finialize!\n" << std::endl;
				return false;
			}
		}

		srand(time(0)); //Update seed of random number generator based on current time.

		//-------------------- Asteroid
		float radius = 125.0;
		float offset = 15.f;
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
		//float tvec1 = glm::linearRand(-5.0f, 5.0f);
		//float tvec2 = glm::linearRand(1.0f, 5.0f);
		//float tvec3 = glm::linearRand(-5.0f, 5.0f);

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

		t_offset = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
		r_offset = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);

		m_sun = new Model("models/Sun/Sun.obj");
		m_earth = new Model("models/Earth/Earth.obj");
		m_moon = new Model("models/Moon/Moon.obj");
		//--------------------

		//-------------------- Lights
		m_point_light0 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light0->setScale(glm::vec3(0.6f, 0.6f, 0.6f));

		m_point_light1 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light1->setPosition(glm::vec3(-50.f, 0.f, 0.f));
		m_point_light1->setScale(glm::vec3(0.6f, 0.6f, 0.6f));

		m_point_light2 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light2->setPosition(glm::vec3(50.f, 0.f, 0.f));
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

		m_player_ship = new Model("models/starship/starship.obj");

		//OpenGL Global Settings
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//-------------------- Frame Buffers
		glGenFramebuffers(1, &hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glGenTextures(2, colorBuffers);

		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}

		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

		unsigned int attachements[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachements);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Framebuffer not complete!" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Blurring Frame Buffer
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongColorBuffers);

		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "Blur Framebuffer not complete!" << std::endl;
			}
		}
		//--------------------

		//Shader Light Settings
		m_shader->Enable();
		setShaderLights(m_shader);

		m_instance_shader->Enable();
		setShaderLights(m_instance_shader);

		m_blur_shader->Enable();
		glUniform1i(m_blur_shader->GetUniformLocation("image"), 0);

		m_hdr_shader->Enable();
		glUniform1i(m_hdr_shader->GetUniformLocation("scene"), 0);
		glUniform1i(m_hdr_shader->GetUniformLocation("bloomBlur"), 1);

		return true;
	}

	void Render()
	{
		glClearColor(0.17, 0.12, 0.19, 1.0); //background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-------------------- Render Models
		m_shader->Enable();
		
		glUniform3fv(m_shader->GetUniformLocation("view_pos"), 1, glm::value_ptr(m_camera->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("point_lights[0].position"), 1, glm::value_ptr(m_point_light0->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("point_lights[1].position"), 1, glm::value_ptr(m_point_light1->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("point_lights[2].position"), 1, glm::value_ptr(m_point_light2->getPosition()));
		glUniform1f(m_shader->GetUniformLocation("material.alpha"), 1.0);

		glUniformMatrix4fv(m_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

		//Space Ship
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_spaceship->getModel()));
		m_spaceship->Render(*m_shader);

		//Planets
		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 30.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_sun->getModel()));
		m_sun->Render(*m_shader);

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 5.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_earth->getModel()));
		m_earth->Render(*m_shader);

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 15.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_moon->getModel()));
		m_moon->Render(*m_shader);

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 10.0f);

		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_player_ship->getModel()));
		m_player_ship->Render(*m_shader);
		//--------------------

		//-------------------- Render Instances
		m_instance_shader->Enable();

		glUniform3fv(m_instance_shader->GetUniformLocation("view_pos"), 1, glm::value_ptr(m_camera->getPosition()));
		glUniform3fv(m_instance_shader->GetUniformLocation("point_lights[0].position"), 1, glm::value_ptr(m_point_light0->getPosition()));
		glUniform3fv(m_instance_shader->GetUniformLocation("point_lights[1].position"), 1, glm::value_ptr(m_point_light1->getPosition()));
		glUniform3fv(m_instance_shader->GetUniformLocation("point_lights[2].position"), 1, glm::value_ptr(m_point_light2->getPosition()));
		glUniform1f(m_instance_shader->GetUniformLocation("material.alpha"), 1.0);

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
		//glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_point_light0->getModel()));
		//m_point_light0->Render(*m_light_shader);
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Two-pass Gaussian blur
		bool horizontal = true;
		bool first_iteration = true;
		unsigned int amount = 10;

		m_blur_shader->Enable();
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			glUniform1i(m_blur_shader->GetUniformLocation("horizontal"), horizontal);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
			renderQuad();
			horizontal = !horizontal;
			if (first_iteration) { first_iteration = false; }
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//HDR Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_hdr_shader->Enable();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
		renderQuad();

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

	void UpdateMousePos(double x, double y, double dt)
	{
		if (x < screen_width / 2)
		{
			roll -= roll_speed * 0.5 * dt;
			roll = std::min(ROLL_MAX, std::max(-ROLL_MAX, roll));
		}
		else if (x > screen_width / 2)
		{
			roll += roll_speed * 0.5 * dt;
			roll = std::min(ROLL_MAX, std::max(-ROLL_MAX, roll));
		}

		if (y < screen_height / 2)
		{
			pitch += pitch_speed * 0.6 * dt;
			pitch = std::min(PITCH_MAX, std::max(-PITCH_MAX, pitch));
		}
		else if (y > screen_height / 2)
		{
			pitch -= pitch_speed * 0.6 * dt;
			pitch = std::min(PITCH_MAX, std::max(-PITCH_MAX, pitch));
		}

		m_camera->MouseLook(x, y);
	}

	void MoveCameraForward(double dt)
	{ 
		m_camera->MoveForward();
		pitch += pitch_speed * dt;
		pitch = std::min(PITCH_MAX, std::max(-PITCH_MAX, pitch)); //clamp
	}
	void MoveCameraBackward(double dt)
	{
		m_camera->MoveBackward();
		pitch -= pitch_speed * dt;
		pitch = std::min(PITCH_MAX, std::max(-PITCH_MAX, pitch));
	}
	void MoveCameraLeft(double dt) 
	{ 
		m_camera->MoveLeft();
		roll -= roll_speed * dt;
		roll = std::min(ROLL_MAX, std::max(-ROLL_MAX, roll));
	}
	void MoveCameraRight(double dt)
	{
		m_camera->MoveRight();
		roll += roll_speed * dt;
		roll = std::min(ROLL_MAX, std::max(-ROLL_MAX, roll));
	}

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
		m_camera->Update(dt);
		m_camera->setFOV(fov);

		//Player Ship
		if (abs(roll) < 0.001) { roll = 0; }
		else { roll = lerp(roll, 0.f, ROLL_DECAY * dt); }

		if (abs(pitch) < 0.001) { pitch = 0; }
		else { pitch = lerp(pitch, 0.f, PITCH_DECAY * dt); }

		glm::mat4 player_translation = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 4.f));
		glm::mat4 player_rotation = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 player_roll = glm::rotate(glm::mat4(1.f), glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 player_pitch = glm::rotate(glm::mat4(1.f), glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
		glm::mat4 player_scale = glm::scale(glm::vec3(.06f, .06f, .06f));
		m_player_ship->Update(glm::inverse(player_rotation * player_roll * player_pitch * player_translation * m_camera->GetView()) * player_scale);

		//Spaceship transform
		double elapsed_time = glfwGetTime();
		float speed = 0.05f;
		float dist = 75.f;

		glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 direction = glm::vec3(cos(speed * elapsed_time) * dist, 0.f, sin(speed * elapsed_time) * dist);
		glm::vec3 tangent = glm::normalize(glm::cross(direction, up));
		float angle = glm::atan2(tangent.x, tangent.z);

		spaceship_tmat = glm::translate(glm::mat4(1.f), direction);
		spaceship_rmat = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f));
		spaceship_smat = glm::scale(glm::vec3(.25f, .25f, .25f));
		m_spaceship->Update(spaceship_tmat * spaceship_rmat * spaceship_smat);

		glm::vec3 light_direction = glm::vec3(cos(speed * elapsed_time - .06f) * dist, 0.f, sin(speed * elapsed_time - .06f) * dist); //Light trails behind spaceship
		glm::mat4 light0_tmat = glm::translate(glm::mat4(1.f), light_direction);
		m_point_light0->Update(light0_tmat);

		//sun transform
		computeTransforms(dt, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.05f, 0.0f, 0.05f }, { 2.f, 2.f, 2.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		transformation_stack.push(t_offset * r_offset * tmat * rmat * smat);

		//earth transform
		computeTransforms(dt, { 0.10f, 0.0f, 0.10f }, { 40.f, 0.0f, 40.0f }, { 0.15f, 0.0f, 0.15f }, { 3.f, 3.f, 3.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		tmat *= glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f)); //adjust cube's offset
		transformation_stack.push(transformation_stack.top() * tmat * rmat * smat);

		//moon transform
		computeTransforms(dt, { 0.1f, 0.1f, 0.f }, { 5.f, 5.f, 0.f }, { 0.15f, 0.0f, 0.15f }, { 1.f, 1.f, 1.f }, glm::vec3(0.f, 1.f, 0.f), tmat, rmat, smat);
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