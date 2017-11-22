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

	virtual void		FillBuffers();

	virtual void		EnableScene();
	virtual void		ResetScene();

protected:
	void				DrawHeightmap();
	void				DrawWater();
	void				DrawSkybox();
	void				PresentScene();
	void				DrawSmoke();
	void				DrawPointLights();
	void				CombineBuffers();

	Shader *			tessShader;
	Shader *			skyboxShader;
	Shader *			testShader;
	Shader *			reflectShader;
	Shader *			particleShader;
	Shader *			combineShader;
	Shader *			pointLightShader;

	SmokeEmitter *		emitter;

	vector<Light *>		pointLights;

	Mesh *				heightMap;
	Mesh *				combineQuad;
	Mesh *				quad2;
	Mesh *				skybox;
	OBJMesh *			sphere;

	GLuint				cubeMap;
	GLuint				terrainTex;
	GLuint				terrainNormalTex;

	GLuint				pointLightFBO;
	GLuint				lightEmissiveTex;
	GLuint				lightSpecularTex;

	GLuint				buffer2FBO;
	GLuint				buffer2ColourTex; // Albedo goes here
	GLuint				buffer2NormalTex; // Normals go here
	GLuint				buffer2DepthTex; // Depth goes here

	GLuint				combinedFBO;
	GLuint				combinedColorTex;

	float				time = 0.0f;
	float				maxTime = 30.0f;
	float				scalefactor = 0.0f;
	float				coolingRatio = 0.0f;
	float				waterRotate = 0.0f;

	virtual void		GenerateScreenTexture(GLuint & into, bool depth = false);

};
