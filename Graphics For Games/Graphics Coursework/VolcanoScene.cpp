#include "VolcanoScene.h"

VolcanoScene::VolcanoScene(Renderer * renderer) : Scene(renderer) {
	
	// ------------------------------------------------------------------------------------------------------------------
	// SCENE SET UP
	// ------------------------------------------------------------------------------------------------------------------

	camera->SetPosition(Vector3(144, 63, 1540));
	camera->SetPitch(11);
	camera->SetYaw(318);
	emitter = new SmokeEmitter(SOIL_load_OGL_texture(TEXTUREDIR"lava.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 5);
	emitter->SetPosition(Vector3(793, YSCALE-50, 780));
	emitter->SetParticleRate(4.0f);
	emitter->SetParticleSize(2.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(2.0f);
	emitter->SetParticleLifetime(5.0f);
	emitter->SetParticleSpeed(Vector3(0.1f, 0.2f, 0));

	// ------------------------------------------------------------------------------------------------------------------
	// SHADERS
	// ------------------------------------------------------------------------------------------------------------------

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

	waterTessShader = new Shader(SHADERDIR"watertes/water_tess_vert.glsl",
		SHADERDIR"watertes/water_tess_frag.glsl",
		SHADERDIR"watertes/water_tess_ctrl.glsl",
		SHADERDIR"watertes/water_tess_eval.glsl",
		SHADERDIR"watertes/water_tess_geom.glsl");

	if (!waterTessShader->LinkProgram()) {
		return;
	}

	combineShader = new Shader(SHADERDIR"combinevert.glsl",
		SHADERDIR"combinefrag.glsl");

	if (!combineShader->LinkProgram()) {
		return;
	}

	pointLightShader = new Shader(SHADERDIR"pointlightvertex.glsl",
		SHADERDIR"pointlightfragment.glsl");

	if (!pointLightShader->LinkProgram()) {
		return;
	}


	// ------------------------------------------------------------------------------------------------------------------
	// TEXTURES AND MESHES
	// ------------------------------------------------------------------------------------------------------------------
	float patches = NPATCHES;
	heightMap	= Mesh::GeneratePlane(patches);
	heightMap->SetType(GL_PATCHES);
	quad2		= Mesh::GeneratePlane(patches*2);
	quad2->SetType(GL_PATCHES);
	combineQuad = Mesh::GenerateQuad();
	skybox		= Mesh::GenerateQuad();
	sphere		= new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj")) {
		return;
	}

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"lava.jpg"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture2(SOIL_load_OGL_texture(TEXTUREDIR"Rock.jpg"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"barren RedsDOT3.jpg"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	quad2->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.jpg",
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

	glGenFramebuffers(1, &buffer2FBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &combinedFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	GenerateScreenTexture(buffer2DepthTex,true);
	GenerateScreenTexture(buffer2NormalTex);
	GenerateScreenTexture(buffer2ColourTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(combinedColorTex);

	glBindFramebuffer(GL_FRAMEBUFFER, buffer2FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer2ColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, buffer2NormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer2DepthTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

VolcanoScene ::~VolcanoScene(void) {
	delete	tessShader;
	delete	skyboxShader;
	delete	testShader;
	delete	waterTessShader;
	delete	particleShader;
	delete	combineShader;
	delete	pointLightShader;

	delete	emitter;
	delete	heightMap;
	delete	combineQuad;
	delete quad2;
	delete skybox;
	delete sphere;

	glDeleteTextures(1, &buffer2ColourTex);
	glDeleteTextures(1, &buffer2DepthTex);
	glDeleteTextures(1, &buffer2NormalTex);
	glDeleteTextures(1, &combinedColorTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteFramebuffers(1, &buffer2FBO);
	glDeleteFramebuffers(1, &combinedFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
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
		float angle = (2*PI*time) / maxTime;
		if (!manualControl) {
			camera->SetPosition(Vector3(793 + cameraDistance*cos(angle), 100 + 40 * sin(angle), 780 + cameraDistance*sin(angle)));
			camera->SetYaw(90 - RadToDeg(angle));
		}

	}
	else if (time > maxTime+2 && time < maxTime+4) {
		if (emitter->GetEnabled()) {
			emitter->Toggle();
		}
	}
	else {
		coolingRatio += msec * 0.0001f;
	}
}

void VolcanoScene::RenderScene() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	FillBuffers();
	DrawPointLights();
	CombineBuffers();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawPostProcess(&combinedColorTex);

	PresentScene();
}

void VolcanoScene::FillBuffers() {

	glBindFramebuffer(GL_FRAMEBUFFER, buffer2FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);
	renderer->UpdateShaderMatrices();

	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawLava();


	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VolcanoScene::DrawSkybox() {
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
	skybox->Draw();
	quad2->Draw();

	glUseProgram(0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
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

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"coolingRatio"), coolingRatio);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, terrainTex);

	//renderer->SetSingleShaderLight(*light);

	renderer->viewMatrix = camera->BuildViewMatrix();
	renderer->modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) *
		Matrix4::Scale(Vector3(100, 100, 1));
	renderer->UpdateShaderMatrices();

	heightMap->Draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void VolcanoScene::DrawWater() {
	renderer->SetCurrentShader(waterTessShader);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"waterTex"), 0);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"waterBumpTex"), 2);

	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"time"), time);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(),
		"cubeTex"), 4);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	renderer->modelMatrix =
		Matrix4::Translation(Vector3(-50*16, 40, -50*16)) *
		Matrix4::Scale(Vector3(100, 1, 100)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	renderer->textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f));

	//renderer->SetSingleShaderLight(*light);

	renderer->UpdateShaderMatrices();

	quad2->Draw();

	glUseProgram(0);
}

void VolcanoScene::DrawLava()
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

void VolcanoScene::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	renderer->SetCurrentShader(pointLightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, buffer2DepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, buffer2NormalTex);

	glUniform3fv(glGetUniformLocation(renderer->currentShader->GetProgram(), "cameraPos"),
		1, (float *)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(renderer->currentShader->GetProgram(), "pixelSize"),
		1.0f / renderer->width, 1.0f / renderer->height);

	vector<Particle *> particles = emitter->GetParticles();
	for (int i = 0; i < particles.size(); ++i) {
		Particle *p = particles[i];
		pointLights.push_back(Light(p->position, Vector4(0.698, 0.133, 0.133, 1), Vector4(0.698, 0.133, 0.133, 1), 50));
	}

	pointLights.push_back(Light(emitter->GetPosition()+Vector3(0,40,0), Vector4(0.698, 0.133, 0.133, 1), Vector4(0.698, 0.133, 0.133, 1), 400));
	pointLights.push_back(Light(Vector3(0, 3000, 0), Vector4(0.7, 0.7, 0.7, 1), Vector4(1, 1, 1, 1), 8000));

	for (int i = 0; i < pointLights.size(); ++i) {
		Light l = pointLights[i];
		float r = l.GetRadius();

		renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f, (float)renderer->width / (float)renderer->height, 45.0f);
		renderer->modelMatrix = Matrix4::Translation(l.GetPosition()) *
			Matrix4::Scale(Vector3(r, r, r));
		 
		l.SetPosition(renderer->modelMatrix.GetPositionVector());

		renderer->SetSingleShaderLight(l);

		renderer->UpdateShaderMatrices();

		float dist = (l.GetPosition() - camera->GetPosition()).Length();
		if (dist < r) {
			glCullFace(GL_FRONT);
		}
		else {
			glCullFace(GL_BACK);
		}
		sphere->Draw();
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	pointLights.clear();
}

void VolcanoScene::CombineBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, combinedFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, combinedColorTex, 0);
	renderer->SetCurrentShader(combineShader);

	renderer->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	renderer->UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "diffuseTex"), 4);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "emissiveTex"), 5);
	glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "specularTex"), 6);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, buffer2ColourTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	combineQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void VolcanoScene::EnableScene() {

}

void VolcanoScene::ResetScene() {
	time = 0.0f;
	scalefactor = 0.0f;
	coolingRatio = 0.0f;
	waterRotate = 0.0f;
	camera->SetPosition(Vector3(144, 63, 1540));
	camera->SetPitch(11);
	camera->SetYaw(318);
	emitter->ClearParticles();
	emitter->SetPosition(Vector3(793, (scalefactor)* YSCALE - 20, 780));
	cameraDistance = (Vector3(144, 0, 1540) - Vector3(793, 0, 780)).Length();

	if (!emitter->GetEnabled()) {
		emitter->Toggle();
	}
}

void VolcanoScene::GenerateScreenTexture(GLuint & into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, renderer->width, renderer->height, 0, depth
		? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}