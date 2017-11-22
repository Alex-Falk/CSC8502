#pragma once
#include "Scene.h"
#include "ParticleEmitter.h"
#include "Renderer.h"
#include "Floor.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"

#define SHADOWSIZE 2048

class Scene;
class HeightMap;
class Camera;

class RainyScene :
	public Scene
{
public:
	RainyScene(Renderer * renderer);
	virtual				~RainyScene(void);

	virtual void		RenderScene();
	virtual void		UpdateScene(float msec);

	virtual void		EnableScene();
	virtual void		ResetScene();
protected:
	void				PresentScene();
	void				DrawShadowScene();
	void				DrawCombinedScene();

	void				DrawMesh();
	void				DrawFloor();
	void				DrawWall();
	void				DrawRain();

	void				MoveLight(Vector3 by);

	ParticleEmitter*	emitter;	//A single particle emitter

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

