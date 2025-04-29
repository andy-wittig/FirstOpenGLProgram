#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Main_Header.h"
#include "Camera.h"
#include "Object.h"
#include "Cube_Map.h"
#include "Shader.h"
#include "Model.h"
#include "Sphere.h"
#include "Emitter.h"

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
	Shader* m_particle_shader;
	Shader* m_outline_shader;

	//Light Models
	Model* m_point_light0;
	Model* m_point_light1;
	Model* m_point_light2;
	Model* m_point_light3;
	Model* m_dir_light;

	//Scene Models
	Model* m_spaceship;
	Model* m_player_ship;
	Model* m_sun;
	Model* m_earth;
	Model* m_moon;

	CubeMap* m_skybox;

	//Procedural Models
	Sphere* m_procedural_sphere;

	//Particle Emitters
	Emitter* m_engine_particle1;
	Emitter* m_engine_particle2;
	Emitter* m_sun_particle;
	Emitter* m_ship_particle;

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
	std::stack<std::pair<std::string, glm::mat4>> planet_stack;

	//Frame Buffers
	unsigned int hdrFBO;
	unsigned int pingpongFBO[2];
	unsigned int colorBuffers[2];
	unsigned int pingpongColorBuffers[2];
	unsigned int rboDepth;
	unsigned int rboStencil;

	//Asteroid Instance Variables
	static const int asteroid_amount = 600;
	std::vector<glm::mat4> asteroidMatrices;
	Model* m_asteroid;

	//Player Ship
	int screen_width;
	int screen_height;

	const float ROLL_MAX = 30.f;
	const float ROLL_DECAY = 1.f;
	float roll = 0.f;
	float roll_speed = 35.f;

	const float PITCH_MAX = 15.f;
	const float PITCH_DECAY = 1.f;
	float pitch = 0.f;
	float pitch_speed = 35.f;

	glm::mat4 player_mat;

	//Visting
	const float SELECT_PLANET_RANGE = 65.f;
	bool can_visit;
	bool visiting;
	std::pair<std::string, float> closest_planet;
	glm::vec3 temp_camera_pos;
	glm::vec3 temp_camera_rot;

	void setShaderLights(Shader *shader)
	{
		//glUniform3fv(shader->GetUniformLocation("dir_light.direction"), 1, glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f)));
		//glUniform3fv(shader->GetUniformLocation("dir_light.ambient"), 1, glm::value_ptr(glm::vec3(0.08f, 0.08f, 0.08f)));
		//glUniform3fv(shader->GetUniformLocation("dir_light.diffuse"), 1, glm::value_ptr(glm::vec3(.8f, .8f, .8f)));
		//glUniform3fv(shader->GetUniformLocation("dir_light.specular"), 1, glm::value_ptr(glm::vec3(0.4f, 0.4f, 0.4f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.direction"), 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.ambient"), 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.diffuse"), 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));
		glUniform3fv(shader->GetUniformLocation("dir_light.specular"), 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));

		glUniform3fv(shader->GetUniformLocation("point_lights[0].ambient"), 1, glm::value_ptr(glm::vec3(.1f, .1f, .1f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[0].diffuse"), 1, glm::value_ptr(glm::vec3(5.f, 5.f, 10.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[0].specular"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));
		glUniform1f(shader->GetUniformLocation("point_lights[0].constant"), 0.25f);
		glUniform1f(shader->GetUniformLocation("point_lights[0].linear"), 0.09f);
		glUniform1f(shader->GetUniformLocation("point_lights[0].quadratic"), 0.032f);

		//Player Engine Lights
		glUniform3fv(shader->GetUniformLocation("point_lights[1].ambient"), 1, glm::value_ptr(glm::vec3(0.25f, 0.25f, 0.25f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 2.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[1].specular"), 1, glm::value_ptr(glm::vec3(.4f, .4f, .4f)));
		glUniform1f(shader->GetUniformLocation("point_lights[1].constant"), 1.5f);
		glUniform1f(shader->GetUniformLocation("point_lights[1].linear"), 0.3f);
		glUniform1f(shader->GetUniformLocation("point_lights[1].quadratic"), 0.05f);

		glUniform3fv(shader->GetUniformLocation("point_lights[2].ambient"), 1, glm::value_ptr(glm::vec3(0.25f, 0.25f, 0.25f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[2].diffuse"), 1, glm::value_ptr(glm::vec3(1.f, 1.f, 2.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[2].specular"), 1, glm::value_ptr(glm::vec3(.4f, .4f, .4f)));
		glUniform1f(shader->GetUniformLocation("point_lights[2].constant"), 1.5f);
		glUniform1f(shader->GetUniformLocation("point_lights[2].linear"), 0.3f);
		glUniform1f(shader->GetUniformLocation("point_lights[2].quadratic"), 0.05f);

		//Sun Light
		glUniform3fv(shader->GetUniformLocation("point_lights[3].ambient"), 1, glm::value_ptr(glm::vec3(.22f, .35f, .7f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[3].diffuse"), 1, glm::value_ptr(glm::vec3(9.5f, 6.6f, 2.f)));
		glUniform3fv(shader->GetUniformLocation("point_lights[3].specular"), 1, glm::value_ptr(glm::vec3(.97f, .95f, .72f)));
		glUniform1f(shader->GetUniformLocation("point_lights[3].constant"), 0.1f);
		glUniform1f(shader->GetUniformLocation("point_lights[3].linear"), 0.02f);
		glUniform1f(shader->GetUniformLocation("point_lights[3].quadratic"), 0.00025f);
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
		m_particle_shader = new Shader();
		m_outline_shader = new Shader();

		std::map<Shader*, std::pair<std::string, std::string>> shader_map
		{
			{m_shader, {"v_shader_source.txt", "f_shader_source.txt"}},
			{m_instance_shader, {"v_instance_shader.txt", "f_shader_source.txt"}},
			{m_light_shader, {"v_light_shader_source.txt", "f_light_shader_source.txt"}},
			{m_skybox_shader, {"v_cube_map_shader_source.txt", "f_cube_map_shader_source.txt"}},
			{m_blur_shader, {"v_hdr_shader.txt", "f_blur_shader.txt"}},
			{m_hdr_shader, {"v_hdr_shader.txt", "f_hdr_shader.txt"}},
			{m_particle_shader, {"v_particle_shader.txt", "f_particle_shader.txt"}},
			{m_outline_shader, {"v_outline_shader.txt", "f_outline_shader.txt"}},
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

		//-------------------- Asteroids
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
		
		//-------------------- Solar System
		glm::vec3 axis;
		float angle = glm::linearRand(0.0f, 360.0f);
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

		//-------------------- Lights
		m_point_light0 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light1 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light2 = new Model("models/lightbulb/lightbulb.obj");
		m_point_light3 = new Model("models/lightbulb/lightbulb.obj");

		m_dir_light = new Model("models/lightbulb/lightbulb.obj");
		m_dir_light->setPosition(glm::vec3(0.f, 35.f, 0.f));
		m_dir_light->setScale(glm::vec3(0.6f, 0.6f, 0.6f));

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
		m_player_ship = new Model("models/starship/starship.obj");

		m_procedural_sphere = new Sphere();
		m_procedural_sphere->Initialize("textures/crystal.png", "textures/crystal_specular_map.png");

		//Initialize Particles
		m_engine_particle1 = new Emitter();
		m_engine_particle1->Initialize("textures/smoke.png", 20, 1, 20, .02f, 1.f);
		m_engine_particle2 = new Emitter();
		m_engine_particle2->Initialize("textures/smoke.png", 20, 1, 20, .02f, 1.f);
		m_sun_particle = new Emitter();
		m_sun_particle->Initialize("textures/flame.png", 50, 1, 10, 30.f, .8f);
		m_ship_particle = new Emitter();
		m_ship_particle->Initialize("textures/smoke.png", 40, 1, 10, .1f, 3.f);

		//OpenGL Global Settings
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		//-------------------- Frame Buffers
		glGenFramebuffers(1, &hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glGenTextures(2, colorBuffers);

		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}

		glGenRenderbuffers(1, &rboStencil);
		glBindRenderbuffer(GL_RENDERBUFFER, rboStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboStencil);

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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "Blur Framebuffer not complete!" << std::endl;
			}
		}

		//Shader Settings
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

		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//-------------------- Render Cube Map
		glDepthFunc(GL_LEQUAL);
		m_skybox_shader->Enable();
		glUniformMatrix4fv(m_skybox_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_skybox_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(m_camera->GetView()))));
		m_skybox->Render();
		glDepthFunc(GL_LESS);

		//-------------------- Render Models
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		m_shader->Enable();
		glUniform3fv(m_shader->GetUniformLocation("view_pos"), 1, glm::value_ptr(m_camera->getPosition()));
		glUniform1f(m_shader->GetUniformLocation("material.alpha"), 1.0);
		glUniform3fv(m_shader->GetUniformLocation("point_lights[0].position"), 1, glm::value_ptr(m_point_light0->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("point_lights[1].position"), 1, glm::value_ptr(m_point_light1->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("point_lights[2].position"), 1, glm::value_ptr(m_point_light2->getPosition()));
		glUniform3fv(m_shader->GetUniformLocation("point_lights[3].position"), 1, glm::value_ptr(m_point_light3->getPosition()));

		glUniformMatrix4fv(m_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

		//Ships
		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 50.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_spaceship->getModel()));
		m_spaceship->Render(*m_shader);

		if (!visiting)
		{
			glUniform1f(m_shader->GetUniformLocation("material.shininess"), 20.0f);
			glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_player_ship->getModel()));
			m_player_ship->Render(*m_shader);
		}

		//Planets
		glUniform1f(m_shader->GetUniformLocation("emissive"), true);
		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 30.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_sun->getModel()));
		m_sun->Render(*m_shader);

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 5.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_earth->getModel()));
		m_earth->Render(*m_shader);
		glUniform1f(m_shader->GetUniformLocation("emissive"), false);

		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 15.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_moon->getModel()));
		m_moon->Render(*m_shader);

		glStencilMask(0x00);

		//Procedurals
		/*
		glUniform1f(m_shader->GetUniformLocation("material.shininess"), 50.0f);
		glUniformMatrix4fv(m_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_procedural_sphere->getModel()));
		m_procedural_sphere->Render(*m_shader);
		*/

		//-------------------- Render Instances
		m_instance_shader->Enable();
		glUniform3fv(m_instance_shader->GetUniformLocation("view_pos"), 1, glm::value_ptr(m_camera->getPosition()));
		glUniform1f(m_instance_shader->GetUniformLocation("material.alpha"), 1.0);
		glUniform3fv(m_instance_shader->GetUniformLocation("point_lights[0].position"), 1, glm::value_ptr(m_point_light0->getPosition()));
		glUniform3fv(m_instance_shader->GetUniformLocation("point_lights[1].position"), 1, glm::value_ptr(m_point_light1->getPosition()));
		glUniform3fv(m_instance_shader->GetUniformLocation("point_lights[2].position"), 1, glm::value_ptr(m_point_light2->getPosition()));
		glUniform3fv(m_instance_shader->GetUniformLocation("point_lights[3].position"), 1, glm::value_ptr(m_point_light3->getPosition()));

		glUniform1f(m_instance_shader->GetUniformLocation("material.shininess"), 45.f);
		glUniformMatrix4fv(m_instance_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_instance_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		m_asteroid->Render(*m_instance_shader);

		//-------------------- Render Lights
		m_light_shader->Enable();
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		/*
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_dir_light->getModel()));
		m_dir_light->Render(*m_light_shader);
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_point_light0->getModel()));
		m_point_light0->Render(*m_light_shader);
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_point_light1->getModel()));
		m_point_light1->Render(*m_light_shader);
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_point_light2->getModel()));
		m_point_light2->Render(*m_light_shader);
		*/
		glUniformMatrix4fv(m_light_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_point_light3->getModel()));
		m_point_light3->Render(*m_light_shader);

		//-------------------- Render Outlines
		can_visit = closest_planet.second <= SELECT_PLANET_RANGE;
		if (can_visit && !visiting)
		{
			if (closest_planet.first == "sun") { drawOutline(m_sun); }
			else if (closest_planet.first == "earth") { drawOutline(m_earth); }
			else if (closest_planet.first == "moon") { drawOutline(m_moon); }
		}

		//-------------------- Render Particles
		m_particle_shader->Enable();
		glUniformMatrix4fv(m_particle_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		glUniformMatrix4fv(m_particle_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		
		if (!visiting)
		{
			glUniform1f(m_particle_shader->GetUniformLocation("scale"), .08f);
			m_engine_particle1->Render(*m_particle_shader);
			m_engine_particle2->Render(*m_particle_shader);
		}

		glUniform1f(m_particle_shader->GetUniformLocation("scale"), 1.f);
		m_sun_particle->Render(*m_particle_shader);

		glUniform1f(m_particle_shader->GetUniformLocation("scale"), .5f);
		m_ship_particle->Render(*m_particle_shader);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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

	void drawOutline(Model* model)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glDisable(GL_DEPTH_TEST);

		m_outline_shader->Enable();
		glUniform1f(m_outline_shader->GetUniformLocation("outline"), 1.01f);
		glUniformMatrix4fv(m_outline_shader->GetUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
		glUniformMatrix4fv(m_outline_shader->GetUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
		glUniformMatrix4fv(m_outline_shader->GetUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(model->getModel()));
		model->RenderOutline();

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
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
			roll -= roll_speed * dt;
			roll = std::min(ROLL_MAX, std::max(-ROLL_MAX, roll));
		}
		else if (x > screen_width / 2)
		{
			roll += roll_speed * dt;
			roll = std::min(ROLL_MAX, std::max(-ROLL_MAX, roll));
		}

		if (y < screen_height / 2)
		{
			pitch += pitch_speed * dt;
			pitch = std::min(PITCH_MAX, std::max(-PITCH_MAX, pitch));
		}
		else if (y > screen_height / 2)
		{
			pitch -= pitch_speed * dt;
			pitch = std::min(PITCH_MAX, std::max(-PITCH_MAX, pitch));
		}

		m_camera->MouseLook(x, y);
	}

	void visitPlanet()
	{
		if (can_visit && !visiting)
		{
			visiting = true;
			temp_camera_pos = m_camera->getPosition();
			temp_camera_rot = m_camera->getRotation();
			//std::cout << "Entered the planet: " << closest_planet.first << std::endl;
		}
		else if (visiting)
		{
			visiting = false;
			m_camera->setPosition(temp_camera_pos);
			m_camera->setRotation(temp_camera_rot);
			//std::cout << "Leaving the planet: " << closest_planet.first << std::endl;
		}
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

		//-------------------- Player
		if (abs(roll) < 0.001) { roll = 0; }
		else { roll = lerp(roll, 0.f, ROLL_DECAY * dt); }

		if (abs(pitch) < 0.001) { pitch = 0; }
		else { pitch = lerp(pitch, 0.f, PITCH_DECAY * dt); }

		if (!visiting) //Player has control
		{
			glm::mat4 player_translation = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.8f, 1.8f));
			glm::mat4 player_rotation = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 player_roll = glm::rotate(glm::mat4(1.f), glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 player_pitch = glm::rotate(glm::mat4(1.f), glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 player_scale = glm::scale(glm::vec3(.03f, .03f, .03f));
			player_mat = glm::inverse((player_rotation * player_roll * player_pitch * player_translation * m_camera->GetView()));
			m_player_ship->Update(player_mat * player_scale);
		}
		else //In visiting mode
		{
			if (closest_planet.first == "sun")
			{
				m_camera->setPosition(m_sun->getPosition() + glm::vec3(65.f, 0.f, 0.f));
				glm::mat4 cam_rmat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
				glm::vec3 cam_direction = glm::vec3(cam_rmat * glm::vec4(0.f, 0.f, 1.f, 0.f));
				m_camera->setRotation(glm::normalize(cam_direction));
			}
			if (closest_planet.first == "earth")
			{
				m_camera->setPosition(m_earth->getPosition() + glm::vec3(25.f, 0.f, 0.f));
				glm::mat4 cam_rmat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
				glm::vec3 cam_direction = glm::vec3(cam_rmat * glm::vec4(0.f, 0.f, 1.f, 0.f));
				m_camera->setRotation(glm::normalize(cam_direction));
			}
			if (closest_planet.first == "moon")
			{
				m_camera->setPosition(m_moon->getPosition() + glm::vec3(5.f, 0.f, 0.f));
				glm::mat4 cam_rmat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
				glm::vec3 cam_direction = glm::vec3(cam_rmat * glm::vec4(0.f, 0.f, 1.f, 0.f));
				m_camera->setRotation(glm::normalize(cam_direction));
			}
		}

		//Player Particles
		glm::mat4 particle_engine_origin1 = glm::translate(player_mat, glm::vec3(.43f, -.05f, -.7f));
		glm::mat4 particle_engine_origin2 = glm::translate(player_mat, glm::vec3(-.43f, -.05f, -.7f));
		glm::vec3 particle_engine_velocity = glm::normalize(glm::vec3(player_mat * glm::vec4(0.f, 0.f, -1.f, 0.f)));

		m_engine_particle1->emitParticles(dt, particle_engine_origin1[3], particle_engine_velocity);
		m_engine_particle2->emitParticles(dt, particle_engine_origin2[3], particle_engine_velocity);

		//Player Lights
		particle_engine_origin1 = glm::translate(particle_engine_origin1, glm::vec3(.0f, 0.f, -.6f));
		particle_engine_origin2 = glm::translate(particle_engine_origin2, glm::vec3(.0f, 0.f, -.6f));

		m_point_light1->Update(particle_engine_origin1 * glm::scale(glm::vec3(.03f, .03f, .03f)));
		m_point_light2->Update(particle_engine_origin2 * glm::scale(glm::vec3(.03f, .03f, .03f)));

		//--------------------Spaceship transform
		double elapsed_time = glfwGetTime();
		float speed = 0.05f;
		float dist = 125.f;

		glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 direction = glm::vec3(cos(speed * elapsed_time) * dist, 5.f, sin(speed * elapsed_time) * dist);
		glm::vec3 tangent = glm::normalize(glm::cross(direction, up));
		float angle = glm::atan2(tangent.x, tangent.z);

		spaceship_tmat = glm::translate(glm::mat4(1.f), direction);
		spaceship_rmat = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f));
		spaceship_smat = glm::scale(glm::vec3(.25f, .25f, .25f));
		m_spaceship->Update(spaceship_tmat * spaceship_rmat * spaceship_smat);

		//Ship Particles
		glm::mat4 particle_ship_origin = glm::translate(spaceship_tmat * spaceship_rmat, glm::vec3(.0f, .0f, -1.f));
		glm::vec3 particle_ship_velocity = glm::normalize(glm::vec3(spaceship_tmat * spaceship_rmat * glm::vec4(0.f, 0.f, -1.f, 0.f)));
		m_ship_particle->emitParticles(dt, particle_ship_origin[3], particle_ship_velocity);

		//Ship Lights
		glm::vec3 light_direction = glm::vec3(cos(speed * elapsed_time - .04f) * dist, 5.f, sin(speed * elapsed_time - .04f) * dist); //Light trails behind spaceship
		glm::mat4 light0_tmat = glm::translate(glm::mat4(1.f), light_direction);
		m_point_light0->Update(light0_tmat);

		//--------------------Solar System transform
		//sun transform
		computeTransforms(dt, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.05f, 0.0f, 0.05f }, { 2.f, 2.f, 2.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		planet_stack.push(std::make_pair("sun", t_offset * r_offset * tmat * rmat * smat));

		m_sun_particle->emitParticles(dt, glm::vec3(0.f), glm::vec3(0.f));

		//earth transform
		computeTransforms(dt, { 0.10f, 0.0f, 0.10f }, { 40.f, 0.0f, 40.0f }, { 0.15f, 0.0f, 0.15f }, { 2.f, 2.f, 2.f }, glm::vec3(0.0f, 1.0f, 0.0f), tmat, rmat, smat);
		tmat *= glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f)); //adjust cube's offset
		planet_stack.push(std::make_pair("earth", planet_stack.top().second * tmat * rmat * smat));

		//moon transform
		computeTransforms(dt, { 0.1f, 0.1f, 0.f }, { 5.f, 5.f, 0.f }, { 0.15f, 0.0f, 0.15f }, { .5f, .5f, .5f }, glm::vec3(0.f, 1.f, 0.f), tmat, rmat, smat);
		planet_stack.push(std::make_pair("moon", planet_stack.top().second * tmat * rmat * smat));

		if (!visiting) //Find closest planet
		{
			std::stack<std::pair<std::string, glm::mat4>> planet_stack_copy = planet_stack;
			std::vector<std::pair<std::string, float>> planet_distances;

			while (!planet_stack_copy.empty())
			{
				planet_distances.push_back(std::make_pair(planet_stack_copy.top().first, glm::length(planet_stack_copy.top().second[3] - player_mat[3])));
				planet_stack_copy.pop();
			}

			closest_planet = *std::min_element(planet_distances.begin(), planet_distances.end(),
				[](const auto& a, const auto& b) { return a.second < b.second; });

			planet_distances.clear();
			//std::cout << "Closest planet is: " << closest_planet.first << ", at a distance of: " << closest_planet.second << std::endl;
		}

		//Stack Updates
		m_moon->Update(planet_stack.top().second);
		planet_stack.pop();
		m_earth->Update(planet_stack.top().second);
		planet_stack.pop();
		m_sun->Update(planet_stack.top().second);
		planet_stack.pop();
	}
};
#endif