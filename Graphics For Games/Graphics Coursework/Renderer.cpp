#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent)
{
	root = new SceneNode;

	quad = Mesh::GenerateQuad();					// Quad to draw scene to
	textquad = Mesh::GenerateQuad();				// Quad to draw the text to

	camera = new Camera(0.0f, 0.0f, 0.0f, 0.0f, Vector3(0, 0, 0));	// Basic camera, this doesn't really get used

	// ------------------------------------------------------------------------------------------------------------------
	// SHADERS
	// ------------------------------------------------------------------------------------------------------------------
	quadShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");

	if (!quadShader->LinkProgram() || !textShader->LinkProgram()) {
		return;
	}

	// ------------------------------------------------------------------------------------------------------------------
	// TEXTURES AND SCENES
	// ------------------------------------------------------------------------------------------------------------------
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	scenes[0] = new RainyScene(this);
	scenes[1] = new MountainScene(this);
	scenes[2] = new VolcanoScene(this);

	currentSceneIdx = 1;
	currentScene = scenes[currentSceneIdx];
	currentScene->EnableScene();

	SceneNode *		scene1 = new SceneNode(Mesh::GenerateQuad());
	SceneNode *		scene2 = new SceneNode(Mesh::GenerateQuad());
	SceneNode *		scene3 = new SceneNode(Mesh::GenerateQuad());

	root->AddChild(scene1);
	root->AddChild(scene2);
	root->AddChild(scene3);

	// ------------------------------------------------------------------------------------------------------------------
	// FRAME BUFFERS AND TEXTURES
	// ------------------------------------------------------------------------------------------------------------------

	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 4; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
	glGenFramebuffers(1, &hudFBO); // We ’ll render the scene into this

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	// We can check FBO attachment success using this command !
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}

	init = true;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	ResetScenes();
	switchActiveCamera(false, false, false);
}

Renderer::~Renderer(void) {	
	delete scenes[0];
	delete scenes[1];
	delete scenes[2];
	delete camera;
	delete quadShader;
	delete textShader;
	delete quad;
	delete textquad;
	delete root;
	
	currentShader = 0;
	glDeleteTextures(4, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &hudFBO);

	
}


void Renderer::RenderScene() {
	if (!isSplitScreen) {
		// Render current scene if splitscreen is disabled
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		currentScene->RenderScene();
		PresentScene();
		SwapBuffers();
	}
	else {
		// RESET SETTINGS FOR EACH SCENE TO ENSURE THEY DON'T MESS WITH EACH OTHER AND RENDER EACH SCENE
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		scenes[0]->RenderScene();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		scenes[1]->RenderScene();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		scenes[2]->RenderScene();

		PresentSplitScreen();
		SwapBuffers();
	}


}


void Renderer::UpdateScene(float msec) {
	// Update timer, calculate FPS every second			
	updateTime += 1.0f / msec;
	if (updateTime > 1.0f) {
		FPS = (1000.0f / msec);				 // msec / 1000 = Seconds per frame
		updateTime = 0.0f;
	}

	// If splitscreen disabled count towards the scene timer to have automatic switching
	if (!isSplitScreen) {
		if (!paused) {
			sceneTimer += msec * 0.001f;
			if (sceneTimer > sceneLength) {
				NextScene();
			}
		}

		if (!scenepaused) {
			currentScene->UpdateScene(msec);
		}	
	}
	// Else update all 3 scenes
	else {
		root->Update(msec);
		scenes[0]->UpdateScene(msec);
		scenes[1]->UpdateScene(msec);
		scenes[2]->UpdateScene(msec);
	}
}

void Renderer::NextScene() {
	sceneTimer = 0.0f;
	switch (currentSceneIdx) {
	case 0:
	case 1:
		currentSceneIdx += 1;
		break;
	case 2:
		currentSceneIdx = 0;
		break;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	currentScene = scenes[currentSceneIdx];
	switchActiveCamera(false, false, false);
	//SetControlledScene(currentSceneIdx);
	currentScene->ResetScene();
}

void Renderer::PreviousScene() {
	sceneTimer = 0.0f;
	switch (currentSceneIdx) {
	case 1:
	case 2:
		currentSceneIdx -= 1;
		break;
	case 0:
		currentSceneIdx = 2;
		break;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	currentScene = scenes[currentSceneIdx];
	switchActiveCamera(false, false, false);
	//SetControlledScene(currentSceneIdx);
	currentScene->ResetScene();
}

void Renderer::switchActiveCamera(bool cama,bool camb,bool camc) {

	scenes[0]->setCameraControl(cama);
	scenes[1]->setCameraControl(camb);
	scenes[2]->setCameraControl(camc);
}

void Renderer::PresentScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



	SetCurrentShader(textShader);
	// ------------------------------------------------------------------------------------------------------------------
	// DRAW BASIC TEXT
	// ------------------------------------------------------------------------------------------------------------------
	float ts = 16.0f;
	int i = 0;

	if (scenepaused) {
		DrawText("------PAUSED------", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("------------------", Vector3(0, i * ts, 0), ts); ++i;
	}

	DrawText("Framerate: " + to_string((int)FPS), Vector3(0, i*ts, 0), ts); ++i;
	string sobel_on = "OFF";
	if (sobelOn) { sobel_on = "ON"; }
	DrawText("Sobel \t" + sobel_on, Vector3(0, i * ts, 0), ts); ++i;
	string contrast_on = "OFF";
	if (contrastOn) { contrast_on = "ON"; }
	DrawText("contrast \t" + contrast_on, Vector3(0, i * ts, 0), ts); ++i;

	DrawText("Active Scene \t" + to_string(currentSceneIdx + 1), Vector3(0, i * ts, 0), ts); ++i;
	DrawText("Time left \t" + to_string((int)sceneLength - (int)sceneTimer) + "s", Vector3(0, i * ts, 0), ts); ++i;

	// ------------------------------------------------------------------------------------------------------------------
	// DRAW FEATURE TEXT
	// ------------------------------------------------------------------------------------------------------------------
	if (showFeatures) {
		switch (currentSceneIdx) {
		case 0:
			DrawText("------------------", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("SCENE FEATURES", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Rain Effect", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Skeletal Animation", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Spotlight", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Spotlight cone", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Realtime Shadows", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Scene Management", Vector3(0, i * ts, 0), ts); ++i;
			break;
		case 1:
			DrawText("------------------", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("SCENE FEATURES", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Landscape", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Basic Lighting", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Environment mapping", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Reflections", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Texture Blending", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Periodic Change textures", Vector3(0, i * ts, 0), ts); ++i;
			break;
		case 2:
			DrawText("------------------", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("SCENE FEATURES", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Tesselation on water", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Water waves", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Tesselation on Terrain", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Changing terrain", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Deffered Rendering", Vector3(0, i * ts, 0), ts); ++i;
			DrawText("Particle Effects", Vector3(0, i * ts, 0), ts); ++i;
			break;
		}
		if (sobelOn) { DrawText("Sobel Postprocessing", Vector3(0, i * ts, 0), ts); ++i; }
		if (contrastOn) { DrawText("Constrast Postprocessing", Vector3(0, i * ts, 0), ts); ++i; }
	}

	// ------------------------------------------------------------------------------------------------------------------
	// DRAW CONTROLS TEXT
	// ------------------------------------------------------------------------------------------------------------------
	if (showControls) {
		DrawText("------------------", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("CONTROLS", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("WASD \tCamera control", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("L    \tLock camera", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("Z/X  \tCamera speed", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("1-3  \tScene selection", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("7    \tSobel", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("8    \tContrast", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("TAB  \tToggle Splitscreen", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("C    \tShow/Hide Controls", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("F    \tShow/Hide Features", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("PAUSE\tPause/Unpause Scene Rotation", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("P    \tPause/Unpause Current Scene", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("LEFT \tPrevious Scene", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("RIGHT\tNext Scene", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("R    \tReset Scene", Vector3(0, i * ts, 0), ts); ++i;
	}

	// ------------------------------------------------------------------------------------------------------------------
	// DRAW CHOSEN SCENE TO A QUAD - each scene is stored in a different bufferColourTex
	// ------------------------------------------------------------------------------------------------------------------
	SetCurrentShader(quadShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	quad->SetTexture(bufferColourTex[currentSceneIdx]);
	quad->Draw();


	glUseProgram(0);
}

void Renderer::PresentSplitScreen() {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// ------------------------------------------------------------------------------------------------------------------
	// DRAW BASIC TEXT
	// ------------------------------------------------------------------------------------------------------------------

	float ts = 16.0f;
	int i = 0;

	int maxi = (int)(height / ts);
	i = (maxi / 2) + 1;

	SetCurrentShader(textShader);
	DrawText("Framerate: " + to_string((int)FPS), Vector3(0, i * ts, 0), ts); ++i;
	string sobel_on = "OFF";
	if (sobelOn) { sobel_on = "ON"; }
	DrawText("Sobel \t" + sobel_on, Vector3(0, i * ts, 0), ts); ++i;
	string contrast_on = "OFF";
	if (contrastOn) { contrast_on = "ON"; }
	DrawText("contrast \t" + contrast_on, Vector3(0, i * ts, 0), ts); ++i;

	DrawText("Controlled Scene \t" + to_string(controlledScene+1), Vector3(0, i * ts, 0), ts); ++i;

	// ------------------------------------------------------------------------------------------------------------------
	// DRAW CONTROLS TEXT
	// ------------------------------------------------------------------------------------------------------------------
	if (showControls) {
		DrawText("------------------", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("CONTROLS", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("WASD \tCamera control", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("Z/X  \tCamera speed", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("1-3  \tScene control selection", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("7    \tSobel", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("8    \tContrast", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("TAB  \tToggle Splitscreen", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("C    \tShow/Hide Controls", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("PAUSE\tPause/Unpause Scene Rotation", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("LEFT \tPrevious Scene", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("RIGHT\tControl next Scene", Vector3(0, i * ts, 0), ts); ++i;
		DrawText("R    \tcontrol previous Scene", Vector3(0, i * ts, 0), ts); ++i;
	}

	// ------------------------------------------------------------------------------------------------------------------
	// SET UP SPLITSCREEN
	// ------------------------------------------------------------------------------------------------------------------
	root->GetChild(0)->SetModelScale(Vector3(0.5, 0.5, 0.5));
	root->GetChild(0)->SetTransform(Matrix4::Translation(Vector3(-.5f, -.5f, 0)));
	root->GetChild(0)->SetTexture(bufferColourTex[0]);

	root->GetChild(1)->SetModelScale(Vector3(0.5, 0.5, 0.5));
	root->GetChild(1)->SetTransform(Matrix4::Translation(Vector3(.5f, -.5f, 0)));
	root->GetChild(1)->SetTexture(bufferColourTex[1]);

	root->GetChild(2)->SetModelScale(Vector3(0.5, 0.5, 0.5));
	root->GetChild(2)->SetTransform(Matrix4::Translation(Vector3(.5f, .5f, 0)));
	root->GetChild(2)->SetTexture(bufferColourTex[2]);

	// ------------------------------------------------------------------------------------------------------------------
	// DRAW SCENES - each scene is stored in a different bufferColourTex
	// ------------------------------------------------------------------------------------------------------------------

	SetCurrentShader(quadShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	BuildNodeLists(root);
	SortNodeLists();

	DrawNodes();

	ClearNodeLists();

}



void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::SetControlledScene(int idx)
{
	bool cama = false, camb = false, camc = false;
	controlledScene = idx;
	switch (idx) {
	case 0:
		cama = true;
		break;
	case 1:
		camb = true;
		break;
	case 2:
		camc = true;
		break;
	}
	switchActiveCamera(cama,camb,camc);

	
}

void Renderer::SetActiveScene(int idx)
{
	currentSceneIdx = idx;
	currentScene = scenes[currentSceneIdx];
	SetControlledScene(currentSceneIdx);
}

void Renderer::UpdateShaderMatrices() {
	if (currentShader) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&projMatrix);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, (float*)&textureMatrix);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, (float*)&shadowMatrix);
	}
}

void Renderer::BuildNodeLists(SceneNode * from) {
	if (from->GetColour().w < 1.0f) {
		transparentNodeList.push_back(from);
	}
	else {
		nodeList.push_back(from);
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
}

void Renderer::DrawNode(SceneNode * n) {
	if (n->GetMesh()) {
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		tempMatrix = shadowMatrix * modelMatrix;

		UpdateShaderMatrices();


		n->Draw();
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::ResetScenes() {
	scenes[0]->ResetScene();
	scenes[1]->ResetScene();
	scenes[2]->ResetScene();
}