#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Main_Header.h"

class Texture
{
private:
	unsigned int diffuse_map = 0;
	unsigned int specular_map = 0;
	unsigned int cube_map = 0;

public:
	void Initialize(const char* diffuse_map_path, const char* specular_map_path)
	{
		diffuse_map = loadTexture(diffuse_map_path);
		specular_map = loadTexture(specular_map_path);
	}

	void Initialize(std::vector<std::string> texture_faces_path)
	{
		cube_map = loadCubeMapTexture(texture_faces_path);
	}

	unsigned int loadTexture(const char* texture_path)
	{
		int width, height, nr_color_channels;

		unsigned int texture;
		glGenTextures(1, &texture);

		stbi_set_flip_vertically_on_load(true);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

	unsigned int loadCubeMapTexture(std::vector<std::string> texture_faces)
	{
		int width, height, nr_color_channels;

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		stbi_set_flip_vertically_on_load(false);

		for (unsigned int i = 0; i < texture_faces.size(); i++)
		{
			unsigned char* data = stbi_load(texture_faces[i].c_str(), &width, &height, &nr_color_channels, 3);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cerr << "Failed to Load Cube Map Texture!" << stbi_failure_reason() << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return texture;
	}

	void bindTextures()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_map);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular_map);
	}

	void bindCubeMapTextures()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
	}
};

#endif