#pragma once
#include "ParticleEmitter.h"
class SmokeEmitter :
	public ParticleEmitter
{
public:
	SmokeEmitter(GLuint texture, float rad = 10.0f) : ParticleEmitter(texture, rad) {};
	~SmokeEmitter() {};

	virtual void Update(float msec);

	virtual void Draw();
	Particle * GetFreeParticle();
};

