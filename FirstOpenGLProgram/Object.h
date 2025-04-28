#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include "Main_Header.h"
#include "Texture.h"

class Object
{
public:
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
	GLuint VAO;

	Texture* m_texture;

	glm::mat4 model;
	glm::vec3 world_origin = glm::vec3(0.f, 0.f, 0.f);

	void Initialize(const char* object_file_path, const char* diffuse_map_path, const char* specular_map_path)
	{ 
		loadModel(object_file_path);
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VB);
		glGenBuffers(1, &IB);

		glBindVertexArray(VAO);

		//Vertex VBO
		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

		//Index VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coords));

		glBindVertexArray(0);

		//Compute Model Matrix
		model = glm::translate(glm::mat4(1.0f), world_origin);
		model *= glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		//Load Textures
		m_texture = new Texture();
		m_texture->Initialize(diffuse_map_path, specular_map_path);
	}

	void Render(Shader &shader)
	{
		glBindVertexArray(VAO);
		m_texture->bindTextures(shader);
		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
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

	void Update(glm::mat4 model_transform)
	{
		model = model_transform;
		//std::cout << glm::to_string(model) << std::endl;
	}

	glm::mat4 getModel()
	{
		return model;
	}

	void setPosition(glm::vec3 position)
	{
		model = glm::translate(glm::mat4(1.0f), position);
	}

	void setScale(glm::vec3 scale)
	{
		model *= glm::scale(scale);
	}

	glm::vec3 getPosition()
	{
		return model[3];
	}

	~Object()
	{
		Vertices.clear();
		Indices.clear();
	}
};
#endif