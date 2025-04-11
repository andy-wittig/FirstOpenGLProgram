#pragma once
#ifndef MESH_H
#define MESH_H

#include "Main_Header.h"
#include "Shader.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
	glm::vec3 tangent;
	glm::vec3 bitangents;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Model_Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Model_Texture> textures;
	std::vector<glm::mat4> instanceMatrices;

	unsigned int instanceVB, VB, IB, VAO;

	void Initialize()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VB);
		glGenBuffers(1, &IB);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

		if (instanceMatrices.size() > 1)
		{
			glGenBuffers(1, &instanceVB);
			glBindBuffer(GL_ARRAY_BUFFER, instanceVB);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceMatrices.size(), &instanceMatrices[0], GL_STATIC_DRAW);

			for (int i = 0; i < 4; i++)
			{
				glEnableVertexAttribArray(3 + i);
				glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * i));
				glVertexAttribDivisor(3 + i, 1);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		glBindVertexArray(0);
	}

public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Model_Texture> textures, std::vector<glm::mat4> instances)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->instanceMatrices = instances;

		Initialize();
	}
	
	void Render(Shader &shader)
	{
		//Bind appropriate textures
		unsigned int diffuse_n = 1, specular_n = 1, normal_n = 1, height_n = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse") { number = std::to_string(diffuse_n++); }
			else if (name == "texture_specular") { number = std::to_string(specular_n++); }
			else if (name == "texture_normal") { number = std::to_string(normal_n++); }
			else if (name == "texture_height") { number = std::to_string(height_n++); }

			glUniform1i(shader.GetUniformLocation((name + number).c_str()), i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glBindVertexArray(VAO);

		if (instanceMatrices.size() > 1)
		{
			glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceMatrices.size());
		}
		else
		{
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
};
#endif