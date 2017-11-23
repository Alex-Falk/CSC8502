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
	~Scene();

	virtual Mesh * getQuad() { return quad; }

	// This class is abstract and is only used to set up general things for the scenes
	virtual void UpdateScene(float msec) = 0;	// Update scene
	virtual void RenderScene() = 0;				// Render Scene to a quad

	virtual void EnableScene() = 0;			
	virtual void ResetScene() = 0;				// Reset Scene to starting values

	void BuildNodeLists(SceneNode * from);		// create node lists from a tree of nodes
	void SortNodeLists();						// Sort this list
	void DrawNodes();							// Draw the nodes from this list
	void DrawNode(SceneNode * n);				// Draw a signle node
	void ClearNodeLists();						// Clear the generated node lists

	void DrawPostProcess(GLuint * texture);		// Draw Post processing effects given a processed texture

	void setCameraControl(bool b) { camera->setControls(b); }	// Locks/Unlocks camera controls
	void toggleManual() { 
		manualControl = !manualControl;
		setCameraControl(manualControl);
	}

	bool isFinished = false;
protected:
	void SetShaderParticleSize(float f);		// Set the size of a particle (used in scenes with particle effects)

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

	bool manualControl = false;
};

