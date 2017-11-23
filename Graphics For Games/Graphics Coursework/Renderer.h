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
	// Set up friends, this is so that the scenes can use the renderer
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

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	// ------------------------------------------------------------------------------------------------------------------
	// TOGGLE COMMANDS
	// ------------------------------------------------------------------------------------------------------------------
	virtual void ToggleSobel() { sobelOn = !sobelOn; }						// Toggle Sobel Filters
	virtual void Togglecontrast() { contrastOn = !contrastOn; }				// Toggle Contrast Post processing effect
	virtual void ToggleSplitScreen() { isSplitScreen = !isSplitScreen;}		// Toggle Splitscreen
	virtual void TogglePause() {											// Toggle Scene Rotation
		paused = !paused;
		if (!paused) { sceneTimer = 0.0f; }
	}
	virtual void ToggleScenePause() { if(!isSplitScreen) { scenepaused = !scenepaused; }; }			// Toggles wheter the scene updates or not
	virtual void ToggleControls() { showControls = !showControls; }			// Toggle Controls display
	virtual void ToggleFeatures() { showFeatures = !showFeatures; }			// Toggle Feature display
	virtual void ToggleManual() {
		if (!isSplitScreen) { currentScene->toggleManual(); }
		else { scenes[controlledScene]->toggleManual(); }
	}
	// ------------------------------------------------------------------------------------------------------------------
	// GENERAL METHODS
	// ------------------------------------------------------------------------------------------------------------------
	bool IsSplitScreen() { return isSplitScreen; }			
	void SetControlledScene(int idx);						// Sets the currently controlled Scene to idx (for splitscreen)
	void SetActiveScene(int idx);							// Sets the currently active Scene to idx (for individual scenes)
	void NextScene();										// Cycle to next Scene
	void PreviousScene();									// Cycle to previous Scene
	void ResetCurrent() { currentScene->ResetScene(); }		// Reset scene to initial state
	void ResetScenes();										// Reset all scenes to start
protected:
	void BuildNodeLists(SceneNode * from);		// create node lists from a tree of nodes
	void SortNodeLists();						// Sort this list
	void DrawNodes();							// Draw the nodes from this list
	void DrawNode(SceneNode * n);				// Draw a signle node
	void ClearNodeLists();						// Clear the generated node lists



	void switchActiveCamera(bool cama, bool camb, bool camc); // Set which cameras are currently active
	
	void PresentScene();						// Presents current Scene
	void PresentSplitScreen();					// Presents all scenes in splitscreen

	Scene * scenes[3];
	Scene * currentScene;

	Shader * quadShader;
	Shader * textShader;

	Camera * camera;
	
	Mesh * quad;
	Mesh * textquad;

	SceneNode * root;

	bool init = false;
	bool showControls = false;
	bool showFeatures = false;
	float sceneTimer = 0.0f;
	float sceneLength = 30.0f;  // in seconds
	int currentSceneIdx = 0;
	int controlledScene = 0;
	float FPS;
	float updateTime = 0.0f;

	GLuint bufferFBO;
	GLuint hudFBO;
	GLuint bufferDepthTex;
	GLuint bufferColourTex[4];

	Font*	basicFont;	//A font! a basic one...

	bool sobelOn = false;
	bool contrastOn = false;
	bool isSplitScreen = false;
	bool paused = false;
	bool scenepaused = true;

	Matrix4 shadowMatrix;
	Matrix4 tempMatrix;

	vector <SceneNode *> transparentNodeList;
	vector <SceneNode *> nodeList;
};
