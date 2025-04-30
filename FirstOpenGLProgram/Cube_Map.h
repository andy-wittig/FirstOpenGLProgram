#pragma once
#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include "Main_Header.h"
#include "Object.h"
#include "Texture.h"

class CubeMap : public Object
{
private:

	struct Vertex
	{
		glm::vec3 vertex;
	};

	std::vector<unsigned int> Indices;
	std::vector<Vertex> Vertices;

	GLuint skyboxVB;
	GLuint skyboxIB;
	GLuint VAO;

	Texture* m_cube_map_texture;

public:
	void Initialize(const char* object_file_path, std::vector<std::string> texture_faces_path)
	{
		loadModel(object_file_path);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &skyboxVB);
		glGenBuffers(1, &skyboxIB);

		glBindVertexArray(VAO);

		//Vertex VBO
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

		//Index VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		glBindVertexArray(0);

		//Load Textures
		m_cube_map_texture = new Texture();
		m_cube_map_texture->InitializeCubeMap(texture_faces_path);
	}

	void Render()
	{
		glBindVertexArray(VAO);
		m_cube_map_texture->bindCubeMapTextures();
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
					sstream >> m_vertex.vertex.x >> m_vertex.vertex.y >> m_vertex.vertex.z;
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
};

#endif