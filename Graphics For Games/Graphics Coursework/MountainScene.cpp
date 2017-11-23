#include "MountainScene.h"

MountainScene::MountainScene(Renderer * renderer) : Scene(renderer) {

	// Initial camera Position
	camera->SetPosition(Vector3(2469.6, 1619.19, 16700));
	camera->SetYaw(332);
	camera->SetPitch(-22);

	heightMap = new HeightMap(TEXTUREDIR"Alps.data");
	EnvironmentQuad = Mesh::GenerateQuad();

	// ------------------------------------------------------------------------------------------------------------------
	// SHADERS
	// ------------------------------------------------------------------------------------------------------------------

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"reflectFragment.glsl");

	if (!reflectShader->LinkProgram()) {
		return;
	}

	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",
		SHADERDIR"skyboxFragment.glsl");

	if (!skyboxShader->LinkProgram()) {
		return;
	}

	quadShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");

	if (!quadShader->LinkProgram()) {
		return;
	}

	lightShader = new Shader(SHADERDIR"BumpVertex.glsl",
		SHADERDIR"BumpFragment.glsl");

	if (!lightShader->LinkProgram()) { 
		return; 
	}

	// ------------------------------------------------------------------------------------------------------------------
	// Textures
	// ------------------------------------------------------------------------------------------------------------------

	EnvironmentQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.JPG",
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
		TEXTUREDIR"hw_glacier/Front.tga", TEXTUREDIR"hw_glacier/Back.tga",
		TEXTUREDIR"hw_glacier/Up.tga", TEXTUREDIR"hw_glacier/Down.tga",
		TEXTUREDIR"hw_glacier/Right.tga", TEXTUREDIR"hw_glacier/Left.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	if (!cubeMap || !heightMap->GetTexture() ||
		!heightMap->GetTexture2() || !EnvironmentQuad->GetTexture()) {
		return;
	}

	renderer->SetTextureRepeating(EnvironmentQuad->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetTexture2(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap2(), true);

	// ------------------------------------------------------------------------------------------------------------------
	// Lights
	// ------------------------------------------------------------------------------------------------------------------


	for (int i = 0; i < 4; ++i) {
		lights[i] = new Light(Vector3(1000.0f + i*2000.0f,
			500.0f, 1000.0f + i*2000.0f),
			Vector4(0, 0, 0, 1), Vector4(0, 0, 0, 1), 100.0f);
	}
	lights[0]->SetPosition(Vector3(0.0f, 500.0f, 0.0f));
	lights[0]->SetRadius(2000.0f);
	r = 2 * heightMap->RAW_WIDTH * HEIGHTMAP_X;
	lights[1]->SetColour(Vector4(1, 1, 1, 1));
	lights[1]->SetPosition(Vector3(heightMap->RAW_WIDTH * HEIGHTMAP_X, 0.0f, heightMap->RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
	lights[1]->SetRadius(heightMap->RAW_WIDTH * HEIGHTMAP_X * 8.0f);
	

	lights[2]->SetColour(Vector4(0.2, 0.2, 0.25, 1));
	lights[2]->SetPosition(Vector3(-heightMap->RAW_WIDTH * HEIGHTMAP_X, 0.0f, heightMap->RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
	lights[2]->SetRadius(heightMap->RAW_WIDTH * HEIGHTMAP_X * 8.0f);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)renderer->width / (float)renderer->height, 45.0f);

	renderer->init = true;
}

MountainScene ::~MountainScene(void) {
	delete heightMap;
	delete EnvironmentQuad;

	delete reflectShader;
	delete skyboxShader;
	delete lightShader;

	delete *lights;
	renderer->currentShader = 0;
}

void MountainScene::UpdateScene(float msec) {
	// update camera
	camera->UpdateCamera(msec);
	camera->BuildViewMatrix();
	frameFrustum.FromMatrix(renderer->projMatrix*renderer->viewMatrix);

	// TODO: Change this
	waterRotate += msec / 1000.0f;

	// Update lights
	lights[1]->SetPosition(Vector3(r*cos(oscillation) + heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		r*sin(oscillation), camera->GetPosition().z));
	lights[2]->SetPosition(Vector3(r*cos(PI+oscillation) + heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		r*sin(PI+oscillation), camera->GetPosition().z));

	if (lights[1]->GetPosition().y <= 0.0f) {
		
		if (!night) {
			Vector3 campos = camera->GetPosition();
			lights[0]->SetPosition(Vector3(campos.x + 600 + rand() % 2000, 1000, campos.z - 600 - rand() % 2000));
			night = true;
		}



		randLightOsc += (msec * 0.001f);
		if (randLightOsc > 0.5f) {
			randLight = rand() % 4;
			randLightOsc = 0.0f;
			lights[0]->SetPosition(lights[0]->GetPosition() + Vector3((rand() % 501) - 250, 0, (rand() % 501) - 250));
		}

		switch (randLight) {
		case 0: lights[0]->SetColour(Vector4(1, 1, 1, 1)); break;
		case 1: lights[0]->SetColour(Vector4(1, 0, 0, 1)); break;
		case 2: lights[0]->SetColour(Vector4(0, 1, 0, 1)); break;
		case 3: lights[0]->SetColour(Vector4(0, 0, 1, 1)); break;
		}
	}
	else {
		night = false;
	}

	// Set up oscillation used for lights and snow/water levels Reset to zero if greater than 2PI
	oscillation += (msec * 0.00025f);
	if (oscillation > 2 * PI) {
		oscillation = 0.0f;
	}
}

void MountainScene::RenderScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)renderer->width / (float)renderer->height, 45.0f);
	renderer->UpdateShaderMatrices();

	DrawSkybox();
	DrawHeightmap();
	DrawWater();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawPostProcess(&bufferColourTex[0]);

	PresentScene();
}

void MountainScene::DrawSkybox() {
	renderer->viewMatrix = camera->BuildViewMatrix();

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	renderer->SetCurrentShader(skyboxShader);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	renderer->UpdateShaderMatrices();
	EnvironmentQuad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void MountainScene::DrawHeightmap() {
	renderer->SetCurrentShader(lightShader);

	offset = ((sin(oscillation) * 400)) + 400;
	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(), "offset"), offset);

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

	EnvironmentQuad->Draw();

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

void MountainScene::EnableScene() {
	camera->SetPosition(Vector3(2469.6, 1619.19, 16700));
}

void MountainScene::ResetScene() {
	camera->SetPosition(Vector3(2469.6, 1619.19, 16700));
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