#pragma once
#include "Light.h"
class Spotlight :
	public Light
{
public:
	Spotlight() : Light() {
		direction	= Vector3(0, 0, 0);
		fov			= 90.0f;
	}
	Spotlight(Vector3 position, Vector3 direction, Vector4 colour, Vector4 specColour, float radius, float fov) : Light(position, colour, specColour, radius) {
		this->direction = direction;
		this->fov = fov;
	}
	~Spotlight() {};

	void SetDirection(Vector3 dir) { direction = dir; }
	Vector3 GetDirection() { return direction; }

	void SetFov(float fov) { this->fov = fov; }
	float GetFov() { return fov; }

protected:
	Vector3 direction;
	float	fov;
};

