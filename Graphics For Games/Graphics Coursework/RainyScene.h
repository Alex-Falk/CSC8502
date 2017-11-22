#pragma once
#include "Scene.h"
#include "ParticleEmitter.h"
#include "Renderer.h"
#include "Floor.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"

#define SHADOWSIZE 2048		// Size of Texture used to create shadowmap

class Scene;
class HeightMap;
class Camera;

class RainyScene :
	public Scene
{
public:
	RainyScene(Renderer * renderer);
	virtual				~RainyScene(void);

	virtual void		UpdateScene(float msec);	// Update scene
	virtual void		RenderScene();				// Render Scene to a quad

	virtual void		EnableScene();
	virtual void		ResetScene();				// Reset Scene to starting values
protected:
	void				PresentScene();				// Presents this scene to the renderers buffercolourtex[0]
	
	void				DrawShadowScene();			// Draw the shadowmap of the scene
	void				DrawCombinedScene();		// Draw the Scene with shadows

	void				DrawMesh();					// Draw hellknight
	void				DrawFloor();				// Draw ground
	void				DrawWall();					// draw wall - TODO: make it work
	void				DrawRain();					// Draw rain particle effect

	ParticleEmitter*	emitter;					// Rain particle emitter

	Shader *			sceneShader;
	Shader *			shadowShader;
	Shader *			animShader;
	Shader *			particleShader;
	Shader *			floorShader;

	GLuint				shadowTex;
	GLuint				shadowFBO;

	MD5FileData *		hellData;
	MD5Node *			hellNode;
	Mesh *				floor;
	Mesh *				wall;
	Spotlight *			light;
	HeightMap *			heightMap;

	Vector3				origin = Vector3(0, 0, 0);

	Vector3				hellNodePos;
	Vector3				hellNodeRot = Vector3(0, 0, 90);
	bool				isWalking = false;
};

