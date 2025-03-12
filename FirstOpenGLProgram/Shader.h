#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

#include <vector>

class Shader
{
private:
	std::vector<GLuint> m_shaderObjList;
	GLuint m_shaderProg;

public:



};

#endif
