#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera
{
private:
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 eye_pos = glm::vec3(0.0, 5.0, -8.0);
	glm::vec3 focus_pos = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 up_dir = glm::vec3(0.0, 1.0, 0.0);

	glm::vec2 prev_mouse_pos;

public:
	bool Initialize(int w, int h)
	{
		view = glm::lookAt(eye_pos, focus_pos, up_dir);

		projection = glm::perspective(  glm::radians(45.f), //90 degree FOV
										float(w) / float(h), //aspect ratio
										0.01f, //distance to near plane
										100.0f); //distance to far plane
		return true;
	}

	void MouseLook(int mouse_x, int mouse_y)
	{
		std::cout << "X: " << mouse_x << ", Y: " << mouse_y << std::endl;
		focus_pos = glm::rotate(focus_pos, glm::radians(mouse_x), up_dir);
		view = glm::lookAt(eye_pos, focus_pos, up_dir);
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
