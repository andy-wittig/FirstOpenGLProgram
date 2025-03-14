#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object
{
private:

	struct Vertex
	{
		glm::vec3 color;
		glm::vec3 vertex;
	};

	float angle;
	float rotation_damp = 0.05;
	float offset_damp = 0.05;

	GLuint VB;
	GLuint IB;
	
	glm::mat4 model;
	
	std::vector<unsigned int> Indices;
	std::vector<Vertex> Vertices;

public:
	Object(std::string object_file_path)
	{ 
		std::ifstream object_file(object_file_path);

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
					sstream >> m_vertex.color.x >> m_vertex.color.y >> m_vertex.color.z >> m_vertex.vertex.x >> m_vertex.vertex.y >> m_vertex.vertex.z;
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

		angle = 0.0f;

		//Vertex VBO
		glGenBuffers(1, &VB);
		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

		//Index VBO
		glGenBuffers(1, &IB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

		//Compute Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.f, 2.f, -3.f));
		model *= glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1.0f, .0f));
	}

	~Object()
	{
		Vertices.clear();
		Indices.clear();
	}

	glm::mat4 GetModel()
	{
		return model;
	}

	void Render()
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glBindBuffer(GL_ARRAY_BUFFER, IB);

		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	void Update(float delta_time, glm::vec3 offset, float angle_offset)
	{
		model = glm::translate(glm::mat4(1.0f), offset * offset_damp);
		model *= glm::rotate(glm::mat4(1.0f), angle_offset * rotation_damp, glm::vec3(0, 1.0f, 0));

		//Spin Model 
		//model *= glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * rotation_damp, glm::vec3(0, 1.0f, 0));
	}
};
#endif