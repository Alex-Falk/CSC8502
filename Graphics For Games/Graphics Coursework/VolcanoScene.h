#pragma once
#include "Scene.h"
#include "Renderer.h"
#include "ParticleEmitter.h"

class Scene;
class HeightMap;
class Camera;

//#define SHADOWSIZE 2048
class VolcanoScene : public Scene {
public:
	VolcanoScene(Renderer * renderer);
	virtual ~VolcanoScene(void);

	virtual void		RenderScene();
	virtual void		UpdateScene(float msec);

	virtual void		EnableScene();
	virtual void		ResetScene();

protected:
	void				DrawHeightmap();
	void				DrawWater();
	void				DrawSkybox();
	void				PresentScene();

	Shader *			tessShader;
	Shader *			skyboxShader;
	Shader *			testShader;

	ParticleEmitter *	emitter;

	Mesh *				heightMap;
	Mesh *				quad2;

	Light *				lights[5];

	GLuint				cubeMap;
	GLuint				terrainTex;
	GLuint				terrainNormalTex;

	float				growthTime = 0.0f;
	float				maxTime = 20.0f;


};
