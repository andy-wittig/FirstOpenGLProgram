#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "Main_Header.h"
#include "Object.h"

class Sphere : public Object
{
private:
	int precision = 20;

	void loadModel()
	{
		for (int i = 0; i <= precision; i++) //Compute Vertices
		{
			for (int j = 0; j <= precision; j++)
			{
				float y = (float)cos(glm::radians(180.f - i * 180.f / precision));
				float x = -(float)cos(glm::radians(j * 360.f / precision)) * (float)abs(cos(asin(y)));
				float z = (float)sin(glm::radians(j * 360.f / precision)) * (float)abs(cos(asin(y)));
				Vertices[i * (precision + 1) + j].vertex = glm::vec3(x, y, z);
				Vertices[i * (precision + 1) + j].normal = glm::vec3(x, y, z);
				Vertices[i * (precision + 1) + j].texture_coords = glm::vec2(((float)j / precision), ((float)i / precision));
			}
		}

		for (int i = 0; i < precision; i++) //Compute Indices
		{
			for (int j = 0; j < precision; j++)
			{
				Indices[6 * (i * precision + j) + 0] = i * (precision + 1) + j;
				Indices[6 * (i * precision + j) + 1] = i * (precision + 1) + j + 1;
				Indices[6 * (i * precision + j) + 2] = (i + 1) * (precision + 1) + j;
				Indices[6 * (i * precision + j) + 3] = i * (precision + 1) + j + 1;
				Indices[6 * (i * precision + j) + 4] = (i + 1) * (precision + 1) + j + 1;
				Indices[6 * (i * precision + j) + 5] = (i + 1) * (precision + 1) + j;
			}
		}
	}

public:
	void Initialize(const char* diffuse_map_path)
	{
		int num_vertices = (precision + 1) * (precision + 1);
		int num_indices = precision * precision * 6;

		for (int i = 0; i < num_vertices; i++)
		{
			Vertex new_vertex;
			Vertices.push_back(new_vertex);
		}
		for (int i = 0; i < num_indices; i++)
		{
			Indices.push_back(0);
		}

		loadModel();

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
		m_texture->Initialize(diffuse_map_path);
	}
};

#endif