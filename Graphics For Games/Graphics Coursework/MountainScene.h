#pragma once
#include "Scene.h"
#include "Renderer.h"
#include "ParticleEmitter.h"

class Scene;
class HeightMap;
class Camera;

//#define SHADOWSIZE 2048
class MountainScene : public Scene {
public:
	MountainScene(Renderer * renderer);
	virtual ~MountainScene(void);

	virtual void		RenderScene();
	virtual void		UpdateScene(float msec);

	virtual void		EnableScene();
	virtual void		ResetScene();

protected:
	void				DrawHeightmap();
	void				DrawWater();
	void				DrawSkybox();
	void				PresentScene();

	Shader *			lightShader;
	Shader *			reflectShader;
	Shader *			skyboxShader;
	Shader *			particleShader;
	GLuint				waterTex;

	ParticleEmitter *	emitter;

	HeightMap *			heightMap;
	Mesh *				quad2;

	Light *				lights[5];

	GLuint				cubeMap;

	float				waterRotate;

	float				offset;
	float				oscillation = 0.0f;
	float				randLightOsc = 0.0f;
	int					randLight;
	float				r;

};
