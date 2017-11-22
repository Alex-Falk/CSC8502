#include "SmokeEmitter.h"
#define RAND() ((rand()%101)/100.0f)

void SmokeEmitter::Update(float msec) {
	nextParticleTime -= msec;

	if (enabled) {
		while (nextParticleTime <= 0) {
			nextParticleTime += particleRate;
			for (int i = 0; i < numLaunchParticles; ++i) {
				particles.push_back(GetFreeParticle());
			}
		}
	}

	for (std::vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */) {
		Particle *p = (*i);

		p->lifetime -= msec*0.001f;


		if (p->lifetime <= 0.0f || p->position.y < 0) {
			freeList.push_back(p);
			i = particles.erase(i);

		}
		else {	
			p->position += (p->velocity*msec);
			p->velocity -= Vector3(0, msec*0.0005f, 0);
			++i;
		}
	}

	if (particles.size() > largestSize) {
		ResizeArrays();
	}

}

void SmokeEmitter::Draw() {
	//Get 2 contiguous sections of memory full of our particle info
	for (unsigned int i = 0; i < particles.size(); ++i) {
		vertices[i] = particles.at(i)->position;
		colours[i] = particles.at(i)->colour;
	}

	glBindVertexArray(arrayObject);

	//Bind our vertex data, and update its data with that of the vertices array
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Vector3), (void*)vertices);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);	//Tell the VAO we have positions...
	glEnableVertexAttribArray(VERTEX_BUFFER);	//Binds this buffer to the VAO

												//And now do the same for colours...
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Vector4), (void*)colours);
	glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
	glEnableVertexAttribArray(COLOUR_BUFFER);

	//We're going to use a type of alpha blending known as additive blending here. Overlapping 
	//particles will have their colours added together on screen, giving us a cool effect when
	//particles are near each other. Fire a lot of particles at once with slow speed to get a
	//'plasma ball' type effect!
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//And now do our usual Drawing stuff...
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_POINTS, 0, particles.size());	// draw ordered list of vertices
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0); //Remember to turn off our VAO ;)
};

Particle* SmokeEmitter::GetFreeParticle() {
	Particle * p = NULL;

	//If we have a spare particle on the freelist, pop it off!
	if (freeList.size() > 0) {
		p = freeList.back();
		freeList.pop_back();
	}
	else {
		//no spare particles, so we need a new one
		p = new Particle();
	}

	//Now we have to reset its values - if it was popped off the
	//free list, it'll still have the values of its 'previous life'

	//p->colour		= Vector4(RAND(),RAND(),RAND(),1.0);
	p->direction = initialDirection;
	p->direction.x += (0);
	p->direction.y += (-1);
	p->direction.z += (0);

	p->direction.Normalise();	//Keep its direction normalised!
	p->position = emitterPosition + Vector3((2 * RAND() - 1)*(emitterRadius), 0, (2 * RAND() - 1)*(emitterRadius));
	p->lifetime = particleLifetime;
	p->colour = Vector4(1, 0.000, 0.000, 1);
	p->velocity = Vector3((2 * (RAND() - 0.5))*particleSpeed.x,2 *RAND()* particleSpeed.y, (2 * (RAND() - 0.5))*particleSpeed.x);

	return p;	//return the new particle :-)
}