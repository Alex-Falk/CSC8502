#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent)
{
	root = new SceneNode;

	quad = Mesh::GenerateQuad();
	textquad = Mesh::GenerateQuad();

	camera = new Camera(0.0f, 0.0f, 0.0f, 0.0f, Vector3(0, 0, 0));//Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));

	//camera->SetPosition(Vector3(10));

	quadShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");

	if (!quadShader->LinkProgram() || !textShader->LinkProgram()) {
		return;
	}

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	scenes[0] = new RainyScene(this);
	scenes[1] = new MountainScene(this);
	scenes[2] = new RainyScene(this);

	currentSceneIdx = 0;
	currentScene = scenes[currentSceneIdx];
	currentScene->EnableScene();

	SceneNode *		scene1 = new SceneNode(Mesh::GenerateQuad());
	SceneNode *		scene2 = new SceneNode(Mesh::GenerateQuad());
	SceneNode *		scene3 = new SceneNode(Mesh::GenerateQuad());

	root->AddChild(scene1);
	root->AddChild(scene2);
	root->AddChild(scene3);

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
}

Renderer::~Renderer(void) {
	//delete scenes;
	delete currentScene;
	delete camera;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);

}


void Renderer::RenderScene() {
	if (!isSplitScreen) {
		currentScene->RenderScene();
		PresentScene();
		SwapBuffers();
	}
	else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		scenes[0]->RenderScene();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		scenes[1]->RenderScene();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		scenes[2]->RenderScene();

		PresentSplitScreen();
		SwapBuffers();
	}


}


void Renderer::UpdateScene(float msec) {
	updateTime += 1.0f / msec;
	if (updateTime > 1.0f) {
		FPS = (1000.0f / msec);
		updateTime = 0.0f;
	}

	if (!isSplitScreen) {
		if (!paused) {
			sceneTimer += msec * 0.001f;
			if (sceneTimer > sceneLength) {
				NextScene();
			}
		}

		currentScene->UpdateScene(msec);
	}
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
	SetControlledScene(currentSceneIdx);
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
	SetControlledScene(currentSceneIdx);
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
	DrawText("Framerate: " + to_string((int)FPS), Vector3(0, 0, 0), 16.0f);
	string sobel_on = "OFF";
	if (sobelOn) { sobel_on = "ON"; }
	DrawText("Sobel \t" + sobel_on, Vector3(0, 16.0f, 0), 16.0f);
	string contrast_on = "OFF";
	if (contrastOn) { contrast_on = "ON"; }
	DrawText("contrast \t" + contrast_on, Vector3(0, 32.0f, 0), 16.0f);

	DrawText("Active Scene \t" + to_string(currentSceneIdx+1), Vector3(0, 48.0f, 0), 16.0f);
	DrawText("Time left \t" + to_string((int)sceneLength-(int)sceneTimer) + "s", Vector3(0, 64.0f, 0), 16.0f);

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

	SetCurrentShader(textShader);
	DrawText("Framerate: " + to_string((int)FPS), Vector3(0, 0, 0), 16.0f);
	string sobel_on = "OFF";
	if (sobelOn) { sobel_on = "ON"; }
	DrawText("Sobel \t" + sobel_on, Vector3(0, 16.0f, 0), 16.0f);
	string contrast_on = "OFF";
	if (contrastOn) { contrast_on = "ON"; }
	DrawText("contrast \t" + contrast_on, Vector3(0, 32.0f, 0), 16.0f);

	DrawText("Controlled Scene \t" + to_string(controlledScene+1), Vector3(0, 48.0f, 0), 16.0f);

	root->GetChild(0)->SetModelScale(Vector3(0.5, 0.5, 0.5));
	root->GetChild(0)->SetTransform(Matrix4::Translation(Vector3(-.5f, -.5f, 0)));
	root->GetChild(0)->SetTexture(bufferColourTex[0]);

	root->GetChild(1)->SetModelScale(Vector3(0.5, 0.5, 0.5));
	root->GetChild(1)->SetTransform(Matrix4::Translation(Vector3(.5f, .5f, 0)));
	root->GetChild(1)->SetTexture(bufferColourTex[1]);

	root->GetChild(2)->SetModelScale(Vector3(0.1, 0.1, 0.1));
	root->GetChild(2)->SetTransform(Matrix4::Translation(Vector3(.5f, -.5f, 0)));
	root->GetChild(2)->SetTexture(bufferColourTex[2]);

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
	/*if (frameFrustum.InsideFrustum(*from)) {
	Vector3 dir = from->GetWorldTransform().GetPositionVector() - renderer->camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));
	*/
	if (from->GetColour().w < 1.0f) {
		transparentNodeList.push_back(from);
	}
	else {
		nodeList.push_back(from);
	}
	//}

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