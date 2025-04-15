#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "Main_Header.h"

class Camera
{
private:
	int screen_width;
	int screen_height;

	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 camera_pos = glm::vec3(0.0, 5.0, -8.0);
	glm::vec3 orientation = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 up_dir = glm::vec3(0.0, 1.0, 0.0);

	const float CAMERA_SPEED = 0.01f;
	float camera_speed;
	float mouse_sensitivity = 100.0f;

	float far_plane_dist = 500.f;
	float near_plane_dist = 0.01f;

public:
	bool Initialize(int w, int h)
	{
		screen_width = w;
		screen_height = h;

		view = glm::lookAt(camera_pos, camera_pos + orientation, up_dir);
		projection = glm::perspective(glm::radians(80.f), float(w) / float(h), near_plane_dist, far_plane_dist);
		return true;
	}

	void MouseLook(double mouse_x, double mouse_y)
	{
		//std::cout << "X: " << mouse_x << ", Y: " << mouse_y << std::endl;

		float rot_x = mouse_sensitivity * (float)(mouse_y - (screen_height / 2)) / screen_height;
		float rot_y = mouse_sensitivity * (float)(mouse_x - (screen_width / 2)) / screen_width;

		glm::vec3 new_orientation = glm::rotate(orientation, glm::radians(-rot_x), glm::normalize(glm::cross(orientation, up_dir)));

		if (abs(glm::angle(new_orientation, up_dir) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = new_orientation;
		}

		orientation = glm::rotate(orientation, glm::radians(-rot_y), up_dir);
	}

	void UpdateTime(float dt)
	{
		camera_speed = CAMERA_SPEED * dt; //Delta time keeps the camera's speed consistent across machines.
	}

	void MoveForward()
	{
		camera_pos += camera_speed * orientation;
	}
	void MoveBackward()
	{
		camera_pos -= camera_speed * orientation;
	}
	void MoveLeft()
	{
		camera_pos -= glm::normalize(glm::cross(orientation, up_dir)) * camera_speed;
	}
	void MoveRight()
	{
		camera_pos += glm::normalize(glm::cross(orientation, up_dir)) * camera_speed;
	}

	glm::vec3 getPosition()
	{
		return camera_pos;
	}

	glm::vec3 getRotation()
	{
		return orientation;
	}

	glm::mat4 GetProjection()
	{
		return projection;
	}

	glm::mat4 GetView()
	{
		view = glm::lookAt(camera_pos, camera_pos + orientation, up_dir);
		return view;
	}

	void setFOV(float fov_amount)
	{
		projection = glm::perspective(glm::radians(fov_amount), (float)screen_width / (float)screen_height, near_plane_dist, far_plane_dist);
	}

};

#endif
