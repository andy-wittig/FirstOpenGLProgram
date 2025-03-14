#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

class Camera
{
private:
	int screen_width;
	int screen_height;

	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 eye_pos = glm::vec3(0.0, 5.0, -8.0);
	glm::vec3 orientation = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 direction;
	glm::vec3 up_dir = glm::vec3(0.0, 1.0, 0.0);

	float mouse_sensitivity = 85.0f;

public:
	bool Initialize(int w, int h)
	{
		screen_width = w;
		screen_height = h;

		direction = glm::normalize(orientation - eye_pos);
		view = glm::lookAt(eye_pos, eye_pos + direction, up_dir);
		
		projection = glm::perspective(  glm::radians(45.f), //90 degree FOV
										float(w) / float(h), //aspect ratio
										0.01f, //distance to near plane
										100.0f); //distance to far plane
		return true;
	}

	void MouseLook(double mouse_x, double mouse_y)
	{
		//std::cout << "X: " << mouse_x << ", Y: " << mouse_y << std::endl;

		float rot_x = mouse_sensitivity * (float)(mouse_y - (screen_height / 2)) / screen_height;
		float rot_y = mouse_sensitivity * (float)(mouse_x - (screen_width / 2)) / screen_width;

		glm::vec3 new_direction = glm::rotate(direction, glm::radians(-rot_x), glm::normalize(glm::cross(direction, up_dir)));

		if (abs(glm::angle(new_direction, up_dir) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			direction = new_direction;
		}

		direction = glm::rotate(direction, glm::radians(-rot_y), up_dir);
	}

	glm::mat4 GetProjection()
	{
		return projection;
	}

	glm::mat4 GetView()
	{
		
		view = glm::lookAt(eye_pos, eye_pos + direction, up_dir);
		return view;
	}

};

#endif
