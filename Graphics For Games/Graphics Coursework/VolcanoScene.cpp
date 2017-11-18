//#include "VolcanoScene.h"
//
//VolcanoScene::VolcanoScene(Renderer * renderer) : Scene(renderer)
//{
//	camera = renderer->camera;
//
//	heightMap = new HeightMap(TEXTUREDIR"testTerrain3.data");
//	quad2 = Mesh::GenerateQuad();
//
//	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f,
//		RAW_HEIGHT*HEIGHTMAP_Y, RAW_WIDTH * HEIGHTMAP_X));
//
//	camera->Setmaxheight(2 * RAW_HEIGHT);
//
//	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
//		SHADERDIR"reflectFragment.glsl");
//	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",
//		SHADERDIR"skyboxFragment.glsl");
//	quadShader = new Shader(SHADERDIR"TexturedVertex.glsl",
//		SHADERDIR"TexturedFragment.glsl");
//	processShader = new Shader(SHADERDIR"processVertex.glsl",
//		SHADERDIR"sobelFrag.glsl");
//	//shadowShader = new Shader(SHADERDIR"shadowVert.glsl",
//	//	SHADERDIR"shadowFrag.glsl");
//
//	if (!reflectShader->LinkProgram()) {
//		return;
//	}
//
//	lightShader = new Shader(SHADERDIR"BumpVertex.glsl",
//		SHADERDIR"BumpFragment.glsl");
//
//	if (!lightShader->LinkProgram()) {
//		return;
//	}
//	if (!skyboxShader->LinkProgram()) {
//		return;
//	}
//	if (!quadShader->LinkProgram()) {
//		return;
//	}
//	if (!processShader->LinkProgram()) {
//		return;
//	}
//	//if (!shadowShader->LinkProgram()) {
//	//	return;
//	//}
//
//	quad2->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.JPG",
//		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
//	heightMap->SetTexture(SOIL_load_OGL_texture(
//		TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO,
//		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
//	heightMap->SetTexture2(SOIL_load_OGL_texture(
//		TEXTUREDIR"grass.png", SOIL_LOAD_AUTO,
//		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
//
//	heightMap->SetBumpMap(SOIL_load_OGL_texture(
//		TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO,
//		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
//
//	heightMap->SetBumpMap2(SOIL_load_OGL_texture(
//		TEXTUREDIR"grassbumps.png", SOIL_LOAD_AUTO,
//		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
//
//	cubeMap = SOIL_load_OGL_cubemap(
//		TEXTUREDIR"skybox/Right.bmp", TEXTUREDIR"skybox/Left.bmp",
//		TEXTUREDIR"skybox/Top.bmp", TEXTUREDIR"skybox/Bottom.bmp",
//		TEXTUREDIR"skybox/Front.bmp", TEXTUREDIR"skybox/Back.bmp",
//		SOIL_LOAD_RGB,
//		SOIL_CREATE_NEW_ID, 0
//	);
//
//	if (!cubeMap || !heightMap->GetTexture() ||
//		!heightMap->GetTexture2() || !quad2->GetTexture()) {
//		return;
//	}
//
//	renderer->SetTextureRepeating(quad2->GetTexture(), true);
//	renderer->SetTextureRepeating(heightMap->GetTexture(), true);
//	renderer->SetTextureRepeating(heightMap->GetTexture2(), true);
//	renderer->SetTextureRepeating(heightMap->GetBumpMap(), true);
//	renderer->SetTextureRepeating(heightMap->GetBumpMap2(), true);
//
//	for (int i = 0; i < 4; ++i) {
//		lights[i] = new Light(Vector3(1000.0f + i*2000.0f,
//			500.0f, 1000.0f + i*2000.0f),
//			Vector4(0, 0, 0, 1), Vector4(0, 0, 0, 1), 100.0f);
//	}
//	lights[0]->SetPosition(Vector3(0.0f, 500.0f, 0.0f));
//	lights[0]->SetRadius(1000.0f);
//
//	r = 2 * RAW_WIDTH * HEIGHTMAP_X;
//	lights[1]->SetColour(Vector4(1, 1, 1, 1));
//	lights[1]->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X, 0.0f, RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
//	lights[1]->SetRadius(RAW_WIDTH * HEIGHTMAP_X * 8.0f);
//
//
//	lights[2]->SetColour(Vector4(0.2, 0.2, 0.25, 1));
//	lights[2]->SetPosition(Vector3(-RAW_WIDTH * HEIGHTMAP_X, 0.0f, RAW_WIDTH * HEIGHTMAP_Z / 2.0f));
//	lights[2]->SetRadius(RAW_WIDTH * HEIGHTMAP_X * 8.0f);
//
//	renderer->projMatrix = Matrix4::Perspective(1.0f, 22999.0f,
//		(float)renderer->width / (float)renderer->height, 45.0f);
//
//	// Generate our scene depth texture ...
//	glGenTextures(1, &bufferDepthTex);
//	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, renderer->width, renderer->height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
//
//	// And our colour texture ...
//	for (int i = 0; i < 2; ++i) {
//		glGenTextures(1, &bufferColourTex[i]);
//		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderer->width, renderer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//	}
//
//	//glGenTextures(1, &shadowTex);
//	//glBindTexture(GL_TEXTURE_2D, shadowTex);
//	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
//	//glBindTexture(GL_TEXTURE_2D, 0);
//
//	glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
//									  //glGenFramebuffers(1, &shadowFBO);
//	glGenFramebuffers(1, &processFBO); // And do post processing in this
//
//
//	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
//	// We can check FBO attachment success using this command !
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
//		return;
//	}
//
//	//glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
//	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
//	//glDrawBuffer(GL_NONE);
//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	renderer->init = true;
//	waterRotate = 0.0f;
//
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//
//}
//
//
//VolcanoScene::~VolcanoScene()
//{
//}
