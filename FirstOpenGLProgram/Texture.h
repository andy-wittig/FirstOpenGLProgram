#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Main_Header.h"
#include "stb_image.h"

class Texture
{
private:
	unsigned int diffuse_map = 0;
	unsigned int specular_map = 0;

public:
	void Initialize(const char* diffuse_map_path, const char* specular_map_path)
	{
		diffuse_map = loadTexture(diffuse_map_path);
		specular_map = loadTexture(specular_map_path);
	}

	unsigned int loadTexture(const char* texture_path)
	{
		int width, height, nr_color_channels;

		unsigned int texture;
		glGenTextures(1, &texture);

		stbi_set_flip_vertically_on_load(true);

		//Load Texture
		unsigned char* data = stbi_load(texture_path, &width, &height, &nr_color_channels, 3);
		if (data)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//Wrapping and Filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cerr << "Failed to Load Texture!" << stbi_failure_reason() << std::endl;
			stbi_image_free(data);
		}
		
		return texture;
	}

	void bindTextures()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_map);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular_map);
	}
};

#endif