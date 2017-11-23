#pragma once
#include "Scene.h"
#include "Renderer.h"
#include "ParticleEmitter.h"

class Scene;
class HeightMap;
class Camera;

class MountainScene : public Scene {
public:
	MountainScene(Renderer * renderer);
	virtual ~MountainScene(void);

	virtual void		UpdateScene(float msec);	// Update scene
	virtual void		RenderScene();				// Render Scene to a quad

	virtual void		EnableScene();
	virtual void		ResetScene();				// Reset Scene to starting values

protected:
	void				DrawHeightmap();			
	void				DrawWater();
	void				DrawSkybox();
	void				PresentScene();				// Presents this scene to the renderers buffercolourtex[1]

	Shader *			lightShader;
	Shader *			reflectShader;
	Shader *			skyboxShader;
	

	HeightMap *			heightMap;
	Mesh *				EnvironmentQuad;

	Light *				lights[5];

	GLuint				cubeMap;
	GLuint				waterTex;

	float				waterRotate;

	float				offset;
	float				oscillation = 0.0f;
	float				randLightOsc = 0.0f;
	int					randLight;
	float				r;

	bool				night = false;
};
