#include "RainyScene.h"


RainyScene::RainyScene(Renderer * renderer) : Scene (renderer)
{
	heightMap = new HeightMap(TEXTUREDIR"flatterrain.data",257,257,1.25f);

	origin = Vector3((257 * HEIGHTMAP_X) / 2.0f, 0.0f, (257 * HEIGHTMAP_X) / 2.0f);

	emitter = new ParticleEmitter(SOIL_load_OGL_texture(TEXTUREDIR"rain.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT));
	emitter->SetPosition(Vector3(origin.x, 300.0f, origin.z));
	
	camera->SetPosition(Vector3(origin.x-500.0f, 350.0f, origin.z));
	camera->SetPitch(-30);
	camera->SetYaw(-90);
	light = new Spotlight(Vector3(origin.x, 300.0f, origin.z),Vector3(0, -1, 0),
		Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1), 5500.0f, 0.0f);

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap(), true);

	wall = Mesh::GeneratePlane(1,4);
	wall->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	wall->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellData->AddAnim(MESHDIR"walk7.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");


	SceneNode * heightNode = new SceneNode(heightMap);
	SceneNode * wallNode = new SceneNode(wall);

	wallNode->SetTransform(Matrix4::Translation(Vector3((257 * HEIGHTMAP_X) / 2.0f, 800.0f, (257 * HEIGHTMAP_X) / 2.0f)));
	wallNode->SetModelScale(Vector3(100, 1, 100));
		
	root->AddChild(heightNode);
	hellNodePos = Vector3((257 * HEIGHTMAP_X) / 2.0f, 800.0f, (257 * HEIGHTMAP_X) / 2.0f);
	hellNode->SetTransform(Matrix4::Translation(hellNodePos));
	root->AddChild(hellNode);
	root->AddChild(wallNode);
	
	particleShader = new Shader(SHADERDIR"particlevertex.glsl",
		SHADERDIR"particlefragment.glsl",
		SHADERDIR"particlegeometry.glsl");

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		SHADERDIR"shadowscenefrag.glsl");

	floorShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		SHADERDIR"shadowscenefloorfrag.glsl");

	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", 
		SHADERDIR"shadowFrag.glsl");

	animShader = new Shader(SHADERDIR"skeletonVertex.glsl",
		SHADERDIR"skeletonFragment.glsl");

	if (!particleShader->LinkProgram() || !sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !floorShader->LinkProgram()) {
		return;
	}

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	renderer->init = true;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

RainyScene::~RainyScene(void)
{
	delete emitter;
	delete light;
	delete hellData;
	delete hellNode;
	delete floor;
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

void RainyScene::RenderScene()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 90.0f);
	renderer->UpdateShaderMatrices();

	DrawShadowScene();

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);
	renderer->UpdateShaderMatrices();

	DrawCombinedScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawPostProcess();

	PresentScene();
}

void RainyScene::UpdateScene(float msec)
{
	emitter->Update(msec);
	hellNode->SetTransform(
		Matrix4::Translation(hellNodePos) *
		Matrix4::Rotation(hellNodeRot.y, Vector3(0, 1, 0)) *
		Matrix4::Rotation(hellNodeRot.z, Vector3(0, 0, 1)));
	hellNode->Update(msec);
	camera->UpdateCamera(msec);
	
	float fov = light->GetFov();
	if (fov < 20.0f) {
		fov += 1 / msec;
		light->SetFov(fov);
	}
	else if (hellNodePos.y > 10.0f) {
		hellNodePos.y -= 200 / msec;
	}
	else if (hellNodeRot.z > 0.0f) {
		hellNodeRot.z -= 10 / msec;
		if (hellNodePos.y > 0.0f) {
			hellNodePos.y -= 10 / msec;
		}
	}
	else if (hellNodeRot.y < 90.0f) {
		hellNodeRot.y += 10 / msec;
	}
	else if (!isWalking && hellNodePos.z < origin.z + 600.0f) {
		hellNode->PlayAnim(MESHDIR"walk7.md5anim");
		isWalking = true;
	}
	else if (hellNode->IsLastFrame() && isWalking && hellNodePos.z < origin.z + 600.0f) {
		hellNodePos.z += 60;
	}
	else if (isWalking && hellNodePos.z >= origin.z + 600.0f) {
		hellNode->PlayAnim(MESHDIR"idle2.md5anim");
		isWalking = false;
	}
	else if (!isWalking && hellNodePos.z >= origin.z + 600.0f) {
		ResetScene();
	}
}

void RainyScene::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderer->bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->bufferColourTex[0], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	renderer->SetCurrentShader(quadShader);
	renderer->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	renderer->viewMatrix.ToIdentity();
	renderer->UpdateShaderMatrices();

	quad->SetTexture(processColourTex);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	ClearNodeLists();
}

void RainyScene::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	renderer->SetCurrentShader(shadowShader);

	renderer->viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(),Vector3(hellNode->GetTransform().GetPositionVector().x+1, 0, hellNode->GetTransform().GetPositionVector().z+1));
	renderer->shadowMatrix = biasMatrix *(renderer->projMatrix * renderer->viewMatrix);

	renderer->UpdateShaderMatrices();

	DrawMesh();
	DrawFloor();
	DrawWall();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, renderer->width, renderer->height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RainyScene::DrawCombinedScene()
{
	renderer->modelMatrix.ToIdentity();
	renderer->SetCurrentShader(sceneShader);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"bumpTex"), 2);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"shadowTex"), 4);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"lightDirection"), 1, (float *)& light->GetDirection());

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"lightfov"), light->GetFov());

	renderer->SetSingleShaderLight(*light);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	renderer->textureMatrix.ToIdentity();
	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->UpdateShaderMatrices();

	DrawFloor();
	DrawWall();

	renderer->SetCurrentShader(floorShader);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"bumpTex"), 2);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"shadowTex"), 4);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"lightDirection"), 1, (float *)& light->GetDirection());

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"lightfov"), light->GetFov());

	renderer->SetSingleShaderLight(*light);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	renderer->textureMatrix.ToIdentity();
	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->UpdateShaderMatrices();

	DrawMesh();

	glUseProgram(0);

	DrawRain();
}

void RainyScene::DrawMesh()
{
	BuildNodeLists(root->GetChild(0));
	SortNodeLists();
	renderer->modelMatrix.ToIdentity();
	Matrix4 tempMatrix = renderer->shadowMatrix * renderer->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(renderer->currentShader->GetProgram()
		, "shadowMatrix"), 1, false, *& renderer->tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(renderer->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *& renderer->modelMatrix.values);

	DrawNodes();
	ClearNodeLists();
}

void RainyScene::DrawFloor()
{
	BuildNodeLists(root->GetChild(1));
	SortNodeLists();
	renderer->modelMatrix.ToIdentity();
	renderer->tempMatrix = renderer->shadowMatrix * renderer->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(renderer->currentShader->GetProgram()
		, "shadowMatrix"), 1, false, *& renderer->tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(renderer->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *&	renderer->modelMatrix.values);

	DrawNodes();
	ClearNodeLists();
}

void RainyScene::DrawWall()
{
	BuildNodeLists(root->GetChild(2));
	SortNodeLists();
	renderer->modelMatrix.ToIdentity();
	renderer->tempMatrix = renderer->shadowMatrix * renderer->modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(renderer->currentShader->GetProgram()
		, "shadowMatrix"), 1, false, *& renderer->tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(renderer->currentShader->GetProgram()
		, "modelMatrix"), 1, false, *&	renderer->modelMatrix.values);

	DrawNodes();
	ClearNodeLists();
}

void RainyScene::DrawRain()
{	
	glClearColor(0, 0, 0, 1);
	renderer->SetCurrentShader(particleShader);

	renderer->viewMatrix = camera->BuildViewMatrix();

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"lightPos"), 1, (float *)& light->GetPosition());

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"lightDirection"), 1, (float *)& light->GetDirection());

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"lightfov"), light->GetFov());

	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleRate(5.0f);
	emitter->SetParticleSize(10.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(16.0f);
	emitter->SetParticleLifetime(2000.0f);
	emitter->SetParticleSpeed(Vector3(0,1.0f,0));

	renderer->UpdateShaderMatrices();

	emitter->Draw();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(0);
}

void RainyScene::MoveLight(Vector3 by) {
	Vector3 newpos = light->GetPosition();
	newpos += by;
	light->SetPosition(newpos);
}

void RainyScene::EnableScene() {
	camera->SetPosition(Vector3(origin.x - 500.0f, 350.0f, origin.z));
	camera->SetPitch(-30);
	camera->SetYaw(-90);
}

void RainyScene::ResetScene() {
	camera->SetPosition(Vector3(origin.x - 500.0f, 350.0f, origin.z));
	camera->SetPitch(-30);
	camera->SetYaw(-90);
	light = new Spotlight(Vector3(origin.x, 300.0f, origin.z), Vector3(0, -1, 0),
		Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1), 5500.0f, 0.0f);
	hellNodeRot = Vector3(0, 0, 90);
	hellNodePos = Vector3((257 * HEIGHTMAP_X) / 2.0f, 800.0f, (257 * HEIGHTMAP_X) / 2.0f);
	hellNode->SetTransform(Matrix4::Translation(hellNodePos));
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	isWalking = false;
}