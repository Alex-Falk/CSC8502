#include "VolcanoScene.h"

VolcanoScene::VolcanoScene(Renderer * renderer) : Scene(renderer) {
		
	light = new Light(Vector3(0, 600, 0), Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1), 1000);

	camera->SetPosition(Vector3(0, 50, 0));
	emitter = new SmokeEmitter(SOIL_load_OGL_texture(TEXTUREDIR"lava.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 5);
	emitter->SetPosition(Vector3(793, YSCALE-50, 780));
	emitter->SetParticleRate(10.0f);
	emitter->SetParticleSize(2.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(2.0f);
	emitter->SetParticleLifetime(5.0f);
	emitter->SetParticleSpeed(Vector3(0.125f, 0.25f, 0));

	tessShader = new Shader(SHADERDIR"terraintes/terrain_tess_vert.glsl",
		SHADERDIR"terraintes/terrain_tess_frag.glsl",
		SHADERDIR"terraintes/terrain_tess_ctrl.glsl",
		SHADERDIR"terraintes/terrain_tess_eval.glsl",
		SHADERDIR"terraintes/terrain_tess_geom.glsl");

	if (!tessShader->LinkProgram()) {
		return;
	}

	particleShader = new Shader(SHADERDIR"particlevertex.glsl",
		SHADERDIR"snowfrag.glsl",
		SHADERDIR"particlegeometry.glsl");

	if (!particleShader->LinkProgram()) {
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

	reflectShader = new Shader(SHADERDIR"watertes/water_tess_vert.glsl",
		SHADERDIR"watertes/water_tess_frag.glsl",
		SHADERDIR"watertes/water_tess_ctrl.glsl",
		SHADERDIR"watertes/water_tess_eval.glsl",
		SHADERDIR"watertes/water_tess_geom.glsl");

	if (!reflectShader->LinkProgram()) {
		return;
	}
	float patches = NPATCHES;
	heightMap	= Mesh::GeneratePlane(patches);
	heightMap->SetType(GL_PATCHES);
	quad2		= Mesh::GeneratePlane(patches*2);
	quad2->SetType(GL_PATCHES);
	skybox		= Mesh::GenerateQuad();

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture2(SOIL_load_OGL_texture(TEXTUREDIR"Rock.jpg"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"barren RedsDOT3.jpg"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	quad2->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	quad2->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"water2bump.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	glEnable(GL_DEPTH_TEST);

	if (!heightMap->GetTexture() || !quad2->GetTexture() || !heightMap->GetTexture2()) {
		return;
	}

	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"volcano2.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);



	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"sor_sea/sea_ft.jpg", TEXTUREDIR"sor_sea/sea_bk.jpg",
		TEXTUREDIR"sor_sea/sea_up.jpg", TEXTUREDIR"sor_sea/sea_dn.jpg",
		TEXTUREDIR"sor_sea/sea_rt.jpg", TEXTUREDIR"sor_sea/sea_lf.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);


	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
	renderer->SetTextureRepeating(heightMap->GetTexture2(), true);
	renderer->SetTextureRepeating(heightMap->GetBumpMap(), true);
	renderer->SetTextureRepeating(quad2->GetTexture(), true);
	renderer->SetTextureRepeating(quad2->GetBumpMap(), true);
	renderer->init = true;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

VolcanoScene ::~VolcanoScene(void) {

}

void VolcanoScene::UpdateScene(float msec) {
	time += msec * 0.001f;
	waterRotate += msec / 4000.0f;

	camera->UpdateCamera(msec);
	
	camera->BuildViewMatrix();
	emitter->Update(msec);

	if (time < maxTime) {
		scalefactor = time / maxTime;
		emitter->SetPosition(Vector3(793, (scalefactor) * YSCALE - 20, 780));
	}
	else {
		if (emitter->GetEnabled()) {
			emitter->Toggle();
		}
	}
	
	


	

}

void VolcanoScene::RenderScene() {

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);
	renderer->UpdateShaderMatrices();

	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawSmoke();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawPostProcess();

	PresentScene();
}

void VolcanoScene::DrawSkybox() {
	renderer->viewMatrix = camera->BuildViewMatrix();

	glDepthMask(GL_FALSE);
	renderer->SetCurrentShader(skyboxShader);

	renderer->UpdateShaderMatrices();
	skybox->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void VolcanoScene::DrawHeightmap() {
	renderer->SetCurrentShader(tessShader);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), 
		"rockTex"), 0);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"grassTex"), 1);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"rockBumpTex"), 2);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"terrainTex"), 4);

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"heightfrac"), scalefactor);

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"npatches"), NPATCHES);

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"yscale"), YSCALE);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, terrainTex);

	renderer->SetSingleShaderLight(*light);

	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) *
		Matrix4::Scale(Vector3(100, 100, 1));
	renderer->UpdateShaderMatrices();

	heightMap->Draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}


void VolcanoScene::DrawWater() {
	renderer->SetCurrentShader(reflectShader);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"waterTex"), 0);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"waterBumpTex"), 2);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cubeTex"), 4);

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"time"), time);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	renderer->modelMatrix =
		Matrix4::Translation(Vector3(-50*16, 40, -50*16)) *
		Matrix4::Scale(Vector3(100, 1, 100)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	renderer->textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f));

	renderer->SetSingleShaderLight(*light);

	renderer->UpdateShaderMatrices();

	quad2->Draw();

	glUseProgram(0);
}

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

void VolcanoScene::DrawSmoke()
{
	glClearColor(0, 0, 0, 1);
	renderer->SetCurrentShader(particleShader);

	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->modelMatrix.ToIdentity();
	renderer->textureMatrix.ToIdentity();
	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(emitter->GetParticleSize());
	renderer->UpdateShaderMatrices();

	emitter->Draw();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(0);
}

void VolcanoScene::EnableScene() {

}

void VolcanoScene::ResetScene() {

}