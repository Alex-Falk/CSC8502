# pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light() {
		position = Vector3(0, 0, 0);
		colour = Vector4(1, 1, 1, 1);
		radius = 1.0f;
		specColour = colour;
	}
	Light(Vector3 position, Vector4 colour, Vector4 specColour, float radius) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
		this->specColour = specColour;
	}
	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

	Vector4 GetSpecColour() const { return specColour; }
	void SetSpecColour(Vector4 val) { specColour = val; }

protected:
	Vector3 position;
	Vector4 colour;
	float radius;

	Vector4 specColour;
};
