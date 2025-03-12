#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	glm::mat4 projection;
	glm::mat4 view;
public:
	bool Initialize(int w, int h)
	{
		view = glm::lookAt( glm::vec3(0.0, 5.0, -8.0), //eye position
							glm::vec3(0.0, 0.0, 0.0), //focus point
							glm::vec3(0.0, 1.0, 0.0)); //positive y is up

		projection = glm::perspective(  glm::radians(45.f), //90 degree FOV
										float(w) / float(h), //aspect ratio
										0.01f, //distance to near plane
										100.0f); //distance to far plane
		return true;
	}

	glm::mat4 GetProjection()
	{
		return projection;
	}

	glm::mat4 GetView()
	{
		return view;
	}

};

#endif
