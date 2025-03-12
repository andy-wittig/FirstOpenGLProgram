#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

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

	GLuint VB;
	GLuint IB;
	
	glm::mat4 model;
	
	std::vector<unsigned int> Indices;
	std::vector<Vertex> Vertices;

public:
	Object()
	{
		Vertices = {
			{{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
			{{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
			{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
			{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
			{{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
			{{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},
		};

		Indices = {
			2, 3, 4,
			8, 7, 6,
			1, 5, 6,
			2, 6, 7,
			7, 8, 4,
			1, 4, 8,
			1, 2, 4,
			5, 8, 6,
			2, 1, 6,
			3, 2, 7,
			3, 7, 4,
			5, 1, 8
		};

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

	void Update(unsigned int delta_time) //Might not be correct
	{
		angle += delta_time * .25 * 3.1415f;;
		model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1.0f, 0));
	}
};
#endif