#include "Scene.h"
#include "Renderer.h"

Scene::Scene(Renderer * renderer)
{
	this->renderer = renderer;
	root = new SceneNode;
	quad = Mesh::GenerateQuad();

	camera = camera = new Camera(0.0f, 0.0f, 0.0f, 0.0f, Vector3(0, 0, 0));

	quadShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");

	if (!quadShader->LinkProgram()) {
		return;
	}

	sobelShader = new Shader(SHADERDIR"processVertex.glsl",
		SHADERDIR"sobelFrag.glsl");

	if (!sobelShader->LinkProgram()) {
		return;
	}

	contrastShader = new Shader(SHADERDIR"processVertex.glsl",
		SHADERDIR"contrastFrag.glsl");

	if (!contrastShader->LinkProgram()) {
		return;
	}

	combineProcessShader = new Shader(SHADERDIR"processVertex.glsl",
		SHADERDIR"combineprocessfrag.glsl");

	if (!combineProcessShader->LinkProgram()) {
		return;
	}


	GenerateScreenTexture(bufferDepthTex, true);

	for (int i = 0; i < 3; ++i) {
		GenerateScreenTexture(bufferColourTex[i]);
	}

	GenerateScreenTexture(processColourTex);

	glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
	glGenFramebuffers(1, &processFBO); // And do post processing in this


	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	// We can check FBO attachment success using this command !
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

void Scene::BuildNodeLists(SceneNode * from) {
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

void Scene::SortNodeLists() {
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Scene::DrawNodes() {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
}

void Scene::DrawNode(SceneNode * n) {
	if (n->GetMesh()) {
		renderer->modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		renderer->tempMatrix = renderer->shadowMatrix * renderer->modelMatrix;

		renderer->UpdateShaderMatrices();

		n->Draw();
	}
}

void Scene::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Scene::DrawPostProcess(GLuint * texture) {
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (renderer->sobelOn) {
		renderer->SetCurrentShader(sobelShader);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);

		renderer->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		renderer->modelMatrix.ToIdentity();
		renderer->textureMatrix.ToIdentity();
		renderer->viewMatrix.ToIdentity();
		renderer->UpdateShaderMatrices();

		quad->SetTexture(*texture);
		quad->Draw();
	}

	if (renderer->contrastOn) {
		renderer->SetCurrentShader(contrastShader);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[2], 0);

		renderer->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		renderer->modelMatrix.ToIdentity();
		renderer->textureMatrix.ToIdentity();
		renderer->viewMatrix.ToIdentity();
		renderer->UpdateShaderMatrices();

		glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(), "contrast"), 2.0f);
		glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(), "brightness"), 0.5f);

		quad->SetTexture(*texture);
		quad->Draw();
	}

	if (renderer->sobelOn || renderer->contrastOn) {
		renderer->SetCurrentShader(combineProcessShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "isSobel"), renderer->sobelOn);
		glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "isContrast"), renderer->contrastOn);

		glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "diffuseTex"), 0);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "sobelTex"), 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[2]);
		glUniform1i(glGetUniformLocation(renderer->currentShader->GetProgram(), "contrastTex"), 5);
	}
	else {
		renderer->SetCurrentShader(quadShader);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex, 0);

	renderer->projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	renderer->modelMatrix.ToIdentity();
	renderer->textureMatrix.ToIdentity();
	renderer->viewMatrix.ToIdentity();
	renderer->UpdateShaderMatrices();

	quad->SetTexture(*texture);
	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Scene::SetShaderParticleSize(float f)
{
	glUniform1f(glGetUniformLocation(renderer->currentShader->GetProgram(), "particleSize"), f);
}

void Scene::GenerateScreenTexture(GLuint & into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH24_STENCIL8 : GL_RGBA8, renderer->width, renderer->height, 0, depth
		? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}