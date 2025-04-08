#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "Main_Header.h"

class Shader
{
private:
	std::vector<GLuint> m_shaderObjList;
	GLuint m_shaderProg;

public:
	Shader()
	{
		m_shaderProg = 0; //Shader Program Default, not initialized yet.
	}

	~Shader()
	{
		for (GLuint shader : m_shaderObjList)
		{
			glDeleteShader(shader);
		}
		

		if (m_shaderProg != 0)
		{
			glDeleteProgram(m_shaderProg);
			m_shaderProg = 0;
		}
	}

	bool Initialize()
	{
		m_shaderProg = glCreateProgram();

		if (m_shaderProg == 0)
		{
			GLenum err = glGetError();
			std::cerr << "Error: Failed to Create Shader Program!\n" << err << std::endl;
			return false;
		}

		return true;
	}

	bool AddShader(GLenum shader_type, const GLchar* shader_source)
	{
		GLint compile_success;
		GLchar infoLog[1024];

		GLint shader_object = glCreateShader(shader_type);
		if (shader_object == 0)
		{
			std::cout << "Error: Shader Type Creation Failed!\n" << shader_type << std::endl;
			return false;
		}

		m_shaderObjList.push_back(shader_object);

		glShaderSource(shader_object, 1, &shader_source, NULL);
		glCompileShader(shader_object);

		//Check Vertex Shader Compiles
		glGetShaderiv(shader_object, GL_COMPILE_STATUS, &compile_success);
		if (!compile_success)
		{
			glGetShaderInfoLog(shader_object, sizeof(infoLog), NULL, infoLog);
			std::cerr << "Error: Shader Object Compilation Failed!\n" << infoLog << std::endl;
			return false;
		}

		glAttachShader(m_shaderProg, shader_object);
		return true;
	}

	bool Finalize()
	{
		GLint compile_success;
		GLchar infoLog[1024];

		glLinkProgram(m_shaderProg);

		//Check if Shader Program Links
		glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &compile_success);

		if (!compile_success)
		{
			glGetProgramInfoLog(m_shaderProg, sizeof(infoLog), NULL, infoLog);
			std::cerr << "Error: Linking Shader Program Failed!\n" << infoLog << std::endl;
			return false;
		}

		//Check if Shader Program Validates
		glValidateProgram(m_shaderProg);
		glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &compile_success);
		if (!compile_success)
		{
			glGetProgramInfoLog(m_shaderProg, sizeof(infoLog), NULL, infoLog);
			std::cerr << "Error: Invalid Shader Program!\n" << infoLog << std::endl;
			return false;
		}

		for (GLuint shader : m_shaderObjList)
		{
			glDeleteShader(shader);
		}
		m_shaderObjList.clear();

		return true;
	}

	void Enable()
	{
		glUseProgram(m_shaderProg);
	}

	GLuint getAttribuLocation(const char* attribute_name)
	{
		GLuint location = glGetAttribLocation(m_shaderProg, attribute_name);
		if (location == -1)
		{
			std::cerr << "Error: Attribute Location Not Available!\n" << attribute_name << std::endl;
		}

		return location;
	}

	GLint GetUniformLocation(const char* uniform_name)
	{
		GLuint location = glGetUniformLocation(m_shaderProg, uniform_name);
		if (location == -1)
		{
			std::cerr << "Error: Uniform Location Not Available!\n" << uniform_name << std::endl;
		}

		return location;
	}

};
#endif
