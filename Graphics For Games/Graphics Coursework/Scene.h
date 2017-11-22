#pragma once

#include "../../nclgl/SceneNode.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"

class Renderer;

class Scene
{
public:
	Scene(Renderer * renderer);
	~Scene() {
		delete root;
		delete quad;
	}

	virtual Mesh * getQuad() { return quad; }

	virtual void UpdateScene(float msec) = 0;
	virtual void RenderScene() = 0;

	virtual void EnableScene() = 0;
	virtual void ResetScene() = 0;

	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);
	void ClearNodeLists();

	void DrawPostProcess(GLuint * texture);

	void setCameraControl(bool b) { camera->setControls(b); }

protected:
	void SetShaderParticleSize(float f);	//And a new setter

	virtual void GenerateScreenTexture(GLuint & into, bool depth = false);

	Renderer * renderer;
	SceneNode * root;
	Mesh * quad;

	Shader * quadShader;
	Shader * sobelShader;
	Shader * contrastShader;
	Shader * combineProcessShader;

	Camera * camera;

	Frustum frameFrustum;

	vector <SceneNode *> transparentNodeList;
	vector <SceneNode *> nodeList;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint processColourTex;
	GLuint bufferColourTex[3];
	GLuint bufferDepthTex;
};

