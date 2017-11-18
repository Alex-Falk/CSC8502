#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/heightmap.h"
#include "Scene.h"
#include "MountainScene.h"
#include "VolcanoScene.h"
#include "RainyScene.h"
#include "TextMesh.h"

class Renderer : public OGLRenderer {
	friend class Scene;
	friend class MountainScene;
	friend class VolcanoScene;
	friend class RainyScene;
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	virtual bool HasInitialised() { return init; }
	virtual void UpdateShaderMatrices();

	void BuildNodeLists(SceneNode * from);

	void SortNodeLists();

	void DrawNodes();

	void DrawNode(SceneNode * n);

	void ClearNodeLists();

	void ResetScenes();

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	virtual void ToggleSobel() { sobelOn = !sobelOn; }
	virtual void Togglecontrast() { contrastOn = !contrastOn; }
	virtual void ToggleSplitScreen() { isSplitScreen = !isSplitScreen;}
	virtual void TogglePause() {
		paused = !paused;
		if (!paused) { sceneTimer = 0.0f; }
	}

	bool IsSplitScreen() { return isSplitScreen; }
	void SetControlledScene(int idx);
	void NextScene();
	void PreviousScene();

protected:
	Scene * scenes[3];
	Scene * currentScene;

	Shader * quadShader;
	Shader * textShader;

	Camera * camera;
	
	Mesh * quad;
	Mesh * textquad;

	SceneNode * root;

	bool init = false;
	float sceneTimer = 0.0f;
	float sceneLength = 40.0f;  // in seconds
	int currentSceneIdx = 0;
	int controlledScene = 0;
	float FPS;
	float updateTime = 0.0f;


	void switchActiveCamera(bool cama, bool camb, bool camc);
	void PresentScene();

	void PresentSplitScreen();

	GLuint bufferFBO;
	GLuint hudFBO;
	GLuint bufferDepthTex;
	GLuint bufferColourTex[4];

	Font*	basicFont;	//A font! a basic one...

	bool sobelOn = false;
	bool contrastOn = false;
	bool isSplitScreen = false;
	bool paused = false;

	Matrix4 shadowMatrix;
	Matrix4 tempMatrix;

	vector <SceneNode *> transparentNodeList;
	vector <SceneNode *> nodeList;
};
