#pragma once
#include "Scene.h"
#include "Renderer.h"
#include "ParticleEmitter.h"
#include "SmokeEmitter.h"

#define NPATCHES 16.0f
#define YSCALE 350.0f

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
	void				DrawSmoke();

	Shader *			tessShader;
	Shader *			skyboxShader;
	Shader *			testShader;
	Shader *			reflectShader;
	Shader *			particleShader;

	SmokeEmitter *		emitter;

	Light *				light;

	Mesh *				heightMap;
	Mesh *				quad2;
	Mesh *				skybox;

	Light *				lights[5];

	GLuint				cubeMap;
	GLuint				terrainTex;
	GLuint				terrainNormalTex;

	float				time = 0.0f;
	float				maxTime = 30.0f;
	float				scalefactor = 0.0f;
	float				coolingRatio = 0.0f;
	float				waterRotate = 0.0f;


};
