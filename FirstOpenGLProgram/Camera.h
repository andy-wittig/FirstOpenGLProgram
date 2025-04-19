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
	glm::vec3 velocity = glm::vec3(0.0);
	glm::vec3 orientation = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 up_dir = glm::vec3(0.0, 1.0, 0.0);

	const float CAMERA_SPEED = 0.0002f;
	const float FRICTION = 0.002;
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

	float lerpSpeed(float start, float end, float f)
	{
		return start * (1.0 - f) + (end * f);
	}

	void Update(float dt)
	{
		camera_speed = CAMERA_SPEED * dt; //Delta time keeps the camera's speed consistent across machines.

		if (glm::length(velocity) < 0.0001) { velocity = glm::vec3(0.0f); }
		else 
		{ 
			velocity.x = lerpSpeed(velocity.x, 0.f, FRICTION * dt);
			velocity.y = lerpSpeed(velocity.y, 0.f, FRICTION * dt);
			velocity.z = lerpSpeed(velocity.z, 0.f, FRICTION * dt);
		}

		camera_pos += velocity * camera_speed;
	}

	void MoveForward()
	{
		velocity += orientation;
	}
	void MoveBackward()
	{
		velocity -= orientation;
	}
	void MoveLeft()
	{
		velocity -= glm::normalize(glm::cross(orientation, up_dir));
	}
	void MoveRight()
	{
		velocity += glm::normalize(glm::cross(orientation, up_dir));
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
