#pragma once
#include "Scene.h"
#include "Renderer.h"
#include "ParticleEmitter.h"
#include "SmokeEmitter.h"

#define NPATCHES 16.0f		// How many patches to use to create the Terrain & Water planes
#define YSCALE 350.0f		// By how much to scale the height by in the tesselation shader that takes in a heightmap

class Scene;
class HeightMap;
class Camera;

//#define SHADOWSIZE 2048
class VolcanoScene : public Scene {
public:
	VolcanoScene(Renderer * renderer);
	virtual ~VolcanoScene(void);

	virtual void		UpdateScene(float msec);	// Update scene
	virtual void		RenderScene();				// Render Scene to a quad

	virtual void		EnableScene();
	virtual void		ResetScene();				// Reset Scene to starting values

protected:
	void				PresentScene();				// Presents this scene to the renderers buffercolourtex[2]
	void				FillBuffers();				// Deffered rendering - draw only the scene
	void				DrawHeightmap();
	void				DrawWater();
	void				DrawSkybox();
	void				DrawLava();
	void				DrawPointLights();			// Deffered rendering - draw lights
	void				CombineBuffers();			// Deffered rendering - draw combined scenes with lights

	Shader *			tessShader;
	Shader *			skyboxShader;
	Shader *			testShader;
	Shader *			waterTessShader;
	Shader *			particleShader;
	Shader *			combineShader;
	Shader *			pointLightShader;

	SmokeEmitter *		emitter;

	vector<Light>		pointLights;

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
