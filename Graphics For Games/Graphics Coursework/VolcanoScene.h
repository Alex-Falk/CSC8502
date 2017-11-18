//#pragma once
//#include "Scene.h"
//#include "Renderer.h"
//
//class Scene;
//class HeightMap;
//class Camera;
//
//#define SHADOWSIZE 2048
//class VolcanoScene : public Scene {
//public:
//	VolcanoScene(Renderer * renderer);
//	virtual ~VolcanoScene(void);
//
//	virtual void RenderScene();
//	virtual void UpdateScene(float msec);
//
//protected:
//	void DrawHeightmap();
//	void DrawWater();
//	void DrawSkybox();
//	void DrawShadowHeightmap();
//	void PresentScene();
//	void DrawSobel();
//	void DrawShadowScene();
//
//	Shader * lightShader;
//	Shader * reflectShader;
//	Shader * skyboxShader;
//	Shader * processShader;
//	Shader * quadShader;
//	Shader * shadowShader;
//
//	GLuint shadowTex;
//	GLuint shadowFBO;
//	GLuint bufferFBO;
//	GLuint processFBO;
//	GLuint bufferColourTex[2];
//	GLuint bufferDepthTex;
//	GLuint waterTex;
//
//	HeightMap * heightMap;
//	Mesh * quad2;
//
//	Light * lights[5];
//	Camera * camera;
//
//	GLuint cubeMap;
//
//	float waterRotate;
//
//	float offset;
//	float oscillation = 0.0f;
//	float randLightOsc = 0.0f;
//	int randLight;
//	float r;
//
//};
