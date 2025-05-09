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

	glm::vec3 camera_pos = glm::vec3(0.0, 30.0, -60.0);
	glm::vec3 velocity = glm::vec3(0.0);
	glm::vec3 orientation = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 up_dir = glm::vec3(0.0, 1.0, 0.0);

	const float CAMERA_SPEED = .2f; //0.2 default
	float extra_camera_speed;
	const float FRICTION = 2.5f;
	float camera_speed;
	float mouse_sensitivity = 100.0f;
	float rot_x;
	float rot_y;

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
		glm::vec3 new_orientation = glm::rotate(orientation, glm::radians(-rot_y), glm::normalize(glm::cross(orientation, up_dir)));

		if (abs(glm::angle(new_orientation, up_dir) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = new_orientation;
		}

		orientation = glm::rotate(orientation, glm::radians(-rot_x), up_dir);
	}

	void Update(float dt)
	{
		if (glm::length(velocity) < 0.0001) { velocity = glm::vec3(0.0f); }
		else 
		{ 
			velocity.x = glm::mix(velocity.x, 0.f, FRICTION * dt);
			velocity.y = glm::mix(velocity.y, 0.f, FRICTION * dt);
			velocity.z = glm::mix(velocity.z, 0.f, FRICTION * dt);
		}

		camera_pos += velocity * (CAMERA_SPEED + extra_camera_speed) * dt; //Delta time keeps the camera's speed consistent across machines.
	}

	void setExtraSpeed(float speed)
	{
		extra_camera_speed = speed;
	}

	void MoveForward()
	{
		velocity += glm::normalize(orientation);
	}
	void MoveBackward()
	{
		velocity -= glm::normalize(orientation);
	}
	void MoveLeft()
	{
		velocity -= glm::normalize(glm::cross(orientation, up_dir));
	}
	void MoveRight()
	{
		velocity += glm::normalize(glm::cross(orientation, up_dir));
	}

	void setPosition(glm::vec3 new_position)
	{
		camera_pos = new_position;
	}

	void setRotation(glm::vec3 new_rotation)
	{
		orientation = new_rotation;
	}

	glm::vec3 getPosition()
	{
		return camera_pos;
	}

	glm::vec3 getRotation()
	{
		return orientation;
	}

	std::pair<float, float> getMouseRot()
	{
		return std::make_pair(rot_x, rot_y);
	}

	void setMouseRot(double mouse_x, double mouse_y)
	{
		rot_x = mouse_sensitivity * (float)(mouse_x - (screen_width / 2)) / screen_width;
		rot_y = mouse_sensitivity * (float)(mouse_y - (screen_height / 2)) / screen_height;
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
