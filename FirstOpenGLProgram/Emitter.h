#pragma once
#ifndef EMITTER_H
#define EMITTER_H

#include "Shader.h"
#include "Main_Header.h"

class Emitter 
{
private:
	struct Particle
	{
		glm::vec3 position, velocity;
		glm::vec4 color;
		float life;

		Particle() : position(0.f), velocity(0.f), color(1.f), life(0.f) { }
	};
	std::vector<Particle> particles;

	//Particle settings
	const char* texture_path;
	unsigned int particle_total;
	unsigned int spawn_amount;
	float particle_range;
	float particle_life;

	unsigned int particleVBO, particleVAO;
	unsigned int last_used_particle = 0;
	unsigned int particle_texture;
	unsigned int spawn_rate;
	float spawn_accumulator;
	glm::vec3 particle_velocity;
	glm::vec3 prev_origin;

	bool local_space = true;

public:
	void useWorldSpace() { local_space = false; }
	void useLocalSpace() { local_space = true; }

	void Initialize(const char* texture_path, unsigned int total_spawned, unsigned int spawn_amount, unsigned int rate, float range, float life)
	{
		this->texture_path = texture_path;
		particle_total = total_spawned;
		this->spawn_amount = spawn_amount;
		spawn_rate = rate;
		particle_range = range;
		particle_life = life;

		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &particleVAO);
		glGenBuffers(1, &particleVBO);
		glBindVertexArray(particleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindVertexArray(0);

		particle_texture = TextureFromFile(texture_path);

		for (unsigned int i = 0; i < particle_total; i++)
		{
			particles.push_back(Particle());
		}
	}

	unsigned int firstUnusedParticle()
	{
		for (unsigned int i = last_used_particle; i < particle_total; i++) //First attempt to find, usually works
		{
			if (particles[i].life <= 0.f)
			{
				last_used_particle = i;
				return i;
			}
		}

		//Linear Search
		for (unsigned int i = 0; i < particle_total; i++)
		{
			if (particles[i].life <= 0.f)
			{
				last_used_particle = i;
				return i;
			}
		}

		last_used_particle = 0;
		return 0;
	}

	void respawnParticle(Particle &particle, glm::vec3 particle_origin, glm::vec3 particle_velocity)
	{
		glm::vec3 offset = glm::sphericalRand(particle_range);
		particle.position = particle_origin + offset;
		particle.life = particle_life;
		particle.velocity = particle_velocity;
		particle.color = glm::vec4(1.0f);
	}

	void emitParticles(double dt, glm::vec3 origin, glm::vec3 velocity)
	{
		glm::vec3 object_movement = origin - prev_origin;
		prev_origin = origin;

		spawn_accumulator += spawn_rate * dt;

		if (spawn_accumulator > 1)
		{
			spawn_accumulator = 0;
			for (unsigned int i = 0; i < spawn_amount; i++) //Emit new particles
			{
				int unused_particle = firstUnusedParticle();
				respawnParticle(particles[unused_particle], origin, velocity);
			}
		}

		for (unsigned int i = 0; i < particle_total; i++) //Update particles
		{
			Particle& part = particles[i];
			part.life -= dt;

			if (part.life > 0.f)
			{
				//part.position -= part.velocity * dt;
				if (local_space) { part.position += object_movement; }
				part.position += part.velocity * dt;
				part.color.a = part.life / particle_life;
			}
		}
	}

	void Render(Shader& shader)
	{
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, particle_texture);

		glBindVertexArray(particleVAO);
		
		for (unsigned int i = 0; i < particles.size(); i++)
		{
			if (particles[i].life > 0.f)
			{
				glUniform3fv(shader.GetUniformLocation("offset"), 1, glm::value_ptr(particles[i].position));
				glUniform4fv(shader.GetUniformLocation("color"), 1, glm::value_ptr(particles[i].color));

				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	unsigned int TextureFromFile(const char* texture_path)
	{
		unsigned int texture_id;
		glGenTextures(1, &texture_id);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(texture_path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1) { format = GL_RED; }
			else if (nrComponents == 3) { format = GL_RGB; }
			else if (nrComponents == 4) { format = GL_RGBA; }

			glBindTexture(GL_TEXTURE_2D, texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << texture_path << std::endl;
			stbi_image_free(data);
		}
		return texture_id;
	}
};

#endif
