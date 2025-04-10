#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Main_Header.h"
#include "Mesh.h"
#include "Shader.h"

unsigned int TextureFromFile(const char* texture_path, const std::string &directory, bool gamma = false);

class Model
{
private:
	//Variables
	std::vector<Model_Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;
	std::vector<glm::mat4> instanceMatrices;

	glm::mat4 model = glm::mat4(1.f);
	glm::vec3 origin = glm::vec3(0.f, 0.f, 0.f);

	//Functions
	void loadModel(std::string const &model_path)
	{
		Assimp::Importer importer; //Read model file using ASSIMP
		const aiScene* scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Error: ASSIMP:" << importer.GetErrorString() << std::endl;
			return;
		}

		directory = model_path.substr(0, model_path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Model_Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;

			//positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			//normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
			}

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;

				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.tex_coords = vec;

				//tangents
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.tangent = vector;

				//bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.bitangents = vector;
			}
			else
			{
				vertex.tex_coords = glm::vec2(0.f, 0.f);
			}
			vertices.push_back(vertex);
		}

		//Get corresponding vertex indices from mesh's face.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//Diffuse maps
		std::vector<Model_Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		//Specular maps
		std::vector<Model_Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		
		//Normal maps
		std::vector<Model_Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		//Height maps
		std::vector<Model_Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		//Return a mesh object created from the extracted mesh data.
		return Mesh(vertices, indices, textures, instanceMatrices);
	}

	std::vector<Model_Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Model_Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false; //check if texrture was loaded before
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				Model_Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();

				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}
public:
	Model(std::string const& path, std::vector<glm::mat4> instances = {}, bool gamma = false) : instanceMatrices(instances), gammaCorrection(gamma)
	{
		if (instances.size() == 0)
		{
			model = glm::translate(glm::mat4(1.0f), origin);
			model *= glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			instanceMatrices.push_back(model);
		}

		loadModel(path);
	}

	void Render(Shader &shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Render(shader);
		}
	}

	void Update(glm::mat4 model_transform)
	{
		model = model_transform;
		//std::cout << glm::to_string(model) << std::endl;
	}

	glm::mat4 getModel()
	{
		return model;
	}

	glm::vec3 getPosition()
	{
		return model[3];
	}

	void setPosition(glm::vec3 position)
	{
		model = glm::translate(glm::mat4(1.0f), position);
	}

	void setScale(glm::vec3 scale)
	{
		model *= glm::scale(scale);
	}
};

unsigned int TextureFromFile(const char* texture_path, const std::string &directory, bool gamma)
{
	std::string file_name = std::string(texture_path);
	file_name = directory + '/' + file_name;

	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	//stbi_set_flip_vertically_on_load(true);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(file_name.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1) { format = GL_RED; }
		else if (nrComponents == 3) { format = GL_RGB; }
		else if (nrComponents == 4) { format = GL_RGBA; }

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << texture_path << std::endl;
		stbi_image_free(data);
	}
	return texture_id;
}
#endif