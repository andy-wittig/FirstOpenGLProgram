#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Main_Header.h"
#include "stb_image.h"

class Texture
{
private:
	GLuint texture = 0;
	int width, height, nr_color_channels;
public:
	bool loadTexture(const char* texture_path)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		//Wrapping and Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true);

		//Load Texture
		unsigned char* data = stbi_load(texture_path, &width, &height, &nr_color_channels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			return false;
		}
		stbi_image_free(data);
		return true;
	}

	void bindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
};

#endif