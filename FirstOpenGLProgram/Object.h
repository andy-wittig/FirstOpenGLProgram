#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include "Main_Header.h"
#include "Texture.h"

class Object
{
private:

	struct Vertex
	{
		glm::vec3 vertex;
		glm::vec3 normal;
		glm::vec2 texture_coords;
	};

	std::vector<unsigned int> Indices;
	std::vector<Vertex> Vertices;

	GLuint VB;
	GLuint IB;

	Texture* m_texture;
	
	glm::mat4 model;
	glm::vec3 object_pos = glm::vec3(0.f, 0.f, 0.f);

	float angle;
	float rotation_speed = 0.5f;
	float move_speed = 0.8f;

public:
	void Initialize(const char* object_file_path, const char* diffuse_map_path, const char* specular_map_path)
	{ 

		loadModel(object_file_path);

		//Vertex VBO
		glGenBuffers(1, &VB);
		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

		//Index VBO
		glGenBuffers(1, &IB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

		//Compute Model Matrix
		angle = 0.0f;
		model = glm::translate(glm::mat4(1.0f), object_pos);
		model *= glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1.0f, .0f));

		//Load Textures
		m_texture = new Texture();
		m_texture->Initialize(diffuse_map_path, specular_map_path);
	}

	void Render()
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

		m_texture->bindTextures();

		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void loadModel(const char* file_path)
	{
		std::ifstream object_file(file_path);

		if (object_file.is_open())
		{
			std::string line;

			while (getline(object_file, line))
			{
				std::stringstream sstream(line);
				std::string type;
				sstream >> type;

				if (type == "V")
				{
					Vertex m_vertex;
					sstream >> m_vertex.vertex.x >> m_vertex.vertex.y >> m_vertex.vertex.z
						>> m_vertex.normal.x >> m_vertex.normal.y >> m_vertex.normal.z
						>> m_vertex.texture_coords.x >> m_vertex.texture_coords.y;
					Vertices.push_back(m_vertex);
				}
				else if (type == "I")
				{
					std::string index;
					while (sstream >> index)
					{
						Indices.push_back((unsigned int)std::stoi(index));
					}
				}
			}
			object_file.close();
		}
		else
		{
			std::cerr << "Error: Could not Open File!\n" << std::endl;
		}

		for (unsigned int i = 0; i < Indices.size(); i++)
		{
			Indices[i] = Indices[i] - 1;
		}
	}

	void Update(float delta_time, glm::vec3 offset, float angle_offset)
	{
		model = glm::translate(glm::mat4(1.0f), object_pos + offset * delta_time * move_speed);
		model *= glm::rotate(glm::mat4(1.0f), angle_offset * delta_time * rotation_speed, glm::vec3(0, 1.0f, 0));

		//Spin Model 
		//model *= glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * rotation_damp, glm::vec3(0, 1.0f, 0));
	}

	glm::mat4 getModel()
	{
		return model;
	}

	glm::vec3 getPosition()
	{
		return object_pos;
	}

	void setPosition(glm::vec3 position)
	{
		object_pos = position;
		model = glm::translate(glm::mat4(1.0f), object_pos);
	}

	~Object()
	{
		Vertices.clear();
		Indices.clear();
	}
};
#endif