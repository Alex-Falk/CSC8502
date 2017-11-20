#include "VolcanoScene.h"

VolcanoScene::VolcanoScene(Renderer * renderer) : Scene(renderer) {

	camera->SetPosition(Vector3(0, 50, 0));

	tessShader = new Shader(SHADERDIR"terrain_tess_vert.glsl",
		SHADERDIR"terrain_tess_frag.glsl",
		SHADERDIR"terrain_tess_ctrl.glsl",
		SHADERDIR"terrain_tess_eval.glsl",
		SHADERDIR"terrain_tess_geom.glsl");

	if (!tessShader->LinkProgram()) {
		return;
	}

	testShader = new Shader(SHADERDIR"terrain_tess_vert.glsl",
		SHADERDIR"terrain_tess_frag.glsl");

	if (!testShader->LinkProgram()) {
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

	heightMap	= Mesh::GenerateQuad();
	heightMap->SetType(GL_PATCHES);
	quad2		= Mesh::GenerateQuad();

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"grass.png"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	glEnable(GL_DEPTH_TEST);

	if (!heightMap->GetTexture()) {
		return;
	}

	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"volcano.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"skybox/Right.bmp", TEXTUREDIR"skybox/Left.bmp",
		TEXTUREDIR"skybox/Top.bmp", TEXTUREDIR"skybox/Bottom.bmp",
		TEXTUREDIR"skybox/Front.bmp", TEXTUREDIR"skybox/Back.bmp",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
	renderer->init = true;
}

VolcanoScene ::~VolcanoScene(void) {

}

void VolcanoScene::UpdateScene(float msec) {

	camera->UpdateCamera(msec);
	camera->BuildViewMatrix();
	
	if (growthTime < maxTime) {
		growthTime += msec * 0.001f;
	}

}

void VolcanoScene::RenderScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);
	renderer->UpdateShaderMatrices();

	DrawSkybox();
	DrawHeightmap();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawPostProcess();

	PresentScene();
}

void VolcanoScene::DrawSkybox() {
	renderer->viewMatrix = camera->BuildViewMatrix();

	glDepthMask(GL_FALSE);
	renderer->SetCurrentShader(skyboxShader);

	renderer->UpdateShaderMatrices();
	quad2->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void VolcanoScene::DrawHeightmap() {
	renderer->SetCurrentShader(tessShader);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), 
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"terrainTex"), 4);

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"heightfrac"), growthTime / maxTime);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, terrainTex);

	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) *
		Matrix4::Scale(Vector3(10000, 10000, 1));
	renderer->UpdateShaderMatrices();

	heightMap->Draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}


//void VolcanoScene::DrawWater() {
//	renderer->SetCurrentShader(reflectShader);
//	renderer->SetShaderLight(lights);
//
//	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(), "offset"), offset);
//
//	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
//		"cameraPos"), 1, (float *)& camera->GetPosition());
//
//	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
//		"diffuseTex"), 0);
//
//	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
//		"cubeTex"), 2);
//
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
//
//	float heightX = (heightMap->RAW_WIDTH * HEIGHTMAP_X / 2.0f);
//
//	float heightY = 256 * heightMap->HEIGHTMAP_Y / 30.0f + (offset / 10.0f);
//
//	float heightZ = (heightMap->RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
//
//	renderer->modelMatrix =
//		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
//		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
//		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
//
//	renderer->textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
//		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
//
//	renderer->UpdateShaderMatrices();
//
//	quad2->Draw();
//
//	glUseProgram(0);
//}

void VolcanoScene::PresentScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, renderer->bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->bufferColourTex[2], 0);
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

void VolcanoScene::EnableScene() {

}

void VolcanoScene::ResetScene() {

}