#include "MountainScene.h"

MountainScene::MountainScene(Renderer * renderer) : Scene(renderer) {

	camera->Setmaxheight(1000);

	heightMap = new HeightMap(TEXTUREDIR"testTerrain3.data");
	quad2 = Mesh::GenerateQuad();

	emitter = new ParticleEmitter(SOIL_load_OGL_texture(TEXTUREDIR"snow.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT),2000);
	emitter->Toggle();

	camera->SetPosition(Vector3(heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		heightMap->RAW_HEIGHT*heightMap->HEIGHTMAP_Y, heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f));

	emitter->SetPosition(camera->GetPosition());

	particleShader = new Shader(SHADERDIR"particlevertex.glsl",
		SHADERDIR"snowfrag.glsl",
		SHADERDIR"particlegeometry.glsl");

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",
		SHADERDIR"skyboxFragment.glsl");
	quadShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");
	//shadowShader = new Shader(SHADERDIR"shadowVert.glsl",
	//	SHADERDIR"shadowFrag.glsl");

	if (!particleShader->LinkProgram()) {
		return;
	}


	if (!reflectShader->LinkProgram()) { 
		return; 
	}

	lightShader = new Shader(SHADERDIR"BumpVertex.glsl",
		SHADERDIR"BumpFragment.glsl");

	if (!lightShader->LinkProgram()) { 
		return; 
	}
	if (!skyboxShader->LinkProgram()) { 
		return; 
	}
	if (!quadShader->LinkProgram()) { 
		return; 
	}
	if (!sobelShader->LinkProgram()) {
		return; 
	}
	//if (!shadowShader->LinkProgram()) {
	//	return;
	//}

	quad2->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture2(SOIL_load_OGL_texture(
		TEXTUREDIR"grass.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	heightMap->SetBumpMap2(SOIL_load_OGL_texture(
		TEXTUREDIR"grassbumps.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"skybox/Right.bmp", TEXTUREDIR"skybox/Left.bmp",
		TEXTUREDIR"skybox/Top.bmp", TEXTUREDIR"skybox/Bottom.bmp",
		TEXTUREDIR"skybox/Front.bmp", TEXTUREDIR"skybox/Back.bmp",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	if (!cubeMap || !heightMap->GetTexture() ||
		!heightMap->GetTexture2() || !quad2->GetTexture()) {
		return;
	}

	renderer->SetTextureRepeating(quad2->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetTexture2(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap2(), true);

	for (int i = 0; i < 4; ++i) {
		lights[i] = new Light(Vector3(1000.0f + i*2000.0f,
			500.0f, 1000.0f + i*2000.0f),
			Vector4(0, 0, 0, 1), Vector4(0, 0, 0, 1), 100.0f);
	}
	lights[0]->SetPosition(Vector3(0.0f, 500.0f, 0.0f));
	lights[0]->SetRadius(1000.0f);
	r = 2 * heightMap->RAW_WIDTH * HEIGHTMAP_X;
	lights[1]->SetColour(Vector4(1, 1, 1, 1));
	lights[1]->SetPosition(Vector3(heightMap->RAW_WIDTH * HEIGHTMAP_X, 0.0f, heightMap->RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
	lights[1]->SetRadius(heightMap->RAW_WIDTH * HEIGHTMAP_X * 8.0f);
	

	lights[2]->SetColour(Vector4(0.2, 0.2, 0.25, 1));
	lights[2]->SetPosition(Vector3(-heightMap->RAW_WIDTH * HEIGHTMAP_X, 0.0f, heightMap->RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
	lights[2]->SetRadius(heightMap->RAW_WIDTH * HEIGHTMAP_X * 8.0f);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f,
		(float)renderer->width / (float)renderer->height, 45.0f);

	// Generate our scene depth texture ...
	renderer->init = true;
}

MountainScene ::~MountainScene(void) {
	delete heightMap;
	delete quad2;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete *lights;
	renderer->currentShader = 0;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void MountainScene::UpdateScene(float msec) {
	emitter->SetPosition(camera->GetPosition() + Vector3(0,200,0));
	emitter->SetDestructionHeight(camera->GetPosition().y - 300);
	emitter->Update(msec);
	camera->UpdateCamera(msec);
	camera->BuildViewMatrix();
	frameFrustum.FromMatrix(renderer->projMatrix*renderer->viewMatrix);

	waterRotate += msec / 1000.0f;

	if (oscillation > PI / 2.0f && !emitter->GetEnabled()) {
		emitter->Toggle();
	}
	if (oscillation > 3 * PI / 2.0f && emitter->GetEnabled()) {
		emitter->Toggle();
	}

	if (oscillation > 2 * PI) { 
		oscillation = 0.0f; 
	}
	lights[1]->SetPosition(Vector3(r*cos(oscillation) + heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f, r*sin(oscillation), camera->GetPosition().z));
	lights[2]->SetPosition(Vector3(r*cos(PI+oscillation) + heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f, r*sin(PI+oscillation), camera->GetPosition().z));

	if (lights[1]->GetPosition().y <= 0.0f) {
		lights[0]->SetPosition(lights[0]->GetPosition() + Vector3(rand() % 100, 0, rand() % 100));
		randLightOsc += (msec * 0.001f);
		if (randLightOsc > 1.0f) {
			randLight = rand() % 4;
			randLightOsc = 0.0f;
		}

		switch (randLight) {
		case 0: lights[0]->SetColour(Vector4(1, 1, 1, 1)); break;
		case 1: lights[0]->SetColour(Vector4(1, 0, 0, 1)); break;
		case 2: lights[0]->SetColour(Vector4(0, 1, 0, 1)); break;
		case 3: lights[0]->SetColour(Vector4(0, 0, 1, 1)); break;
		}
	}


	oscillation += (msec * 0.00025f);


}

void MountainScene::RenderScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);
	renderer->UpdateShaderMatrices();

	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawSnow();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawPostProcess();	

	PresentScene();
}

void MountainScene::DrawSkybox() {
	renderer->viewMatrix = camera->BuildViewMatrix();

	glDepthMask(GL_FALSE);
	renderer->SetCurrentShader(skyboxShader);

	renderer->UpdateShaderMatrices();
	quad2->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void MountainScene::DrawHeightmap() {
	renderer->SetCurrentShader(lightShader);

	offset = ((sin(oscillation) * 400)) + 400;
	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(), "offset"), offset);

	//Vector4 tint(161.0f / 256.0f, 37.0f / 256.0f, 27.0f / 256.0f, 1);
	//Vector4 tint(0.545, 0.271, 0.075, 1);
	renderer->SetShaderLight(lights);

	Vector4 tint(1, 1, 1, 0);
	glUniform4fv(glGetUniformLocation(renderer->currentShader->GetProgram(), "terrainTint"), 1, (float*)&tint);

	Vector4 snowcolour(0.878, 1, 1, 1);
	glUniform4fv(glGetUniformLocation(renderer->currentShader->GetProgram(), "snowColour"), 1, (float*)&snowcolour);

	Vector4 shadecolour(0, 0, 0, 1);
	glUniform4fv(glGetUniformLocation(renderer->currentShader->GetProgram(), "shadeColour"), 1, (float*)&shadecolour);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "diffuseTex2"), 1);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "bumpTex"), 2);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "bumpTex2"), 3);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->modelMatrix.ToIdentity();
	renderer->textureMatrix.ToIdentity();
	renderer->UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);
}

void MountainScene::DrawWater() {
	renderer->SetCurrentShader(reflectShader);
	renderer->SetShaderLight(lights);

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(), "offset"), offset);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * heightMap->HEIGHTMAP_Y / 30.0f +(offset / 10.0f);

	float heightZ = (heightMap->RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	renderer->modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	renderer->textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	renderer->UpdateShaderMatrices();

	quad2->Draw();

	glUseProgram(0);
}

void MountainScene::PresentScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, renderer->bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	renderer->SetCurrentShader(quadShader);
	renderer->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	renderer->modelMatrix.ToIdentity();
	renderer->textureMatrix.ToIdentity();
	renderer->viewMatrix.ToIdentity();
	renderer->UpdateShaderMatrices();

	quad->SetTexture(processColourTex);
	
	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	ClearNodeLists();
}

void MountainScene::DrawSnow()
{
	glClearColor(0, 0, 0, 1);
	renderer->SetCurrentShader(particleShader);

	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->modelMatrix.ToIdentity();
	renderer->textureMatrix.ToIdentity();
	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleRate(1.0f);
	emitter->SetParticleSize(10.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(2.0f);
	emitter->SetParticleLifetime(2000.0f);
	emitter->SetParticleSpeed(0.1f);

	renderer->UpdateShaderMatrices();

	emitter->Draw();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(0);
}

void MountainScene::EnableScene() {
	camera->SetPosition(Vector3(heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		heightMap->RAW_HEIGHT*heightMap->HEIGHTMAP_Y, heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f));
}

void MountainScene::ResetScene() {
	camera->SetPosition(Vector3(heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		heightMap->RAW_HEIGHT*heightMap->HEIGHTMAP_Y, heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f));
	float oscillation = 0.0f;
	float randLightOsc = 0.0f;

	lights[0]->SetPosition(Vector3(0.0f, 500.0f, 0.0f));
	lights[0]->SetRadius(1000.0f);
	r = 2 * heightMap->RAW_WIDTH * HEIGHTMAP_X;
	lights[1]->SetColour(Vector4(1, 1, 1, 1));
	lights[1]->SetPosition(Vector3(heightMap->RAW_WIDTH * HEIGHTMAP_X, 0.0f, heightMap->RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
	lights[1]->SetRadius(heightMap->RAW_WIDTH * HEIGHTMAP_X * 8.0f);


	lights[2]->SetColour(Vector4(0.2, 0.2, 0.25, 1));
	lights[2]->SetPosition(Vector3(-heightMap->RAW_WIDTH * HEIGHTMAP_X, 0.0f, heightMap->RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
	lights[2]->SetRadius(heightMap->RAW_WIDTH * HEIGHTMAP_X * 8.0f);

}