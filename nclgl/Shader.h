#pragma once
#include "OGLRenderer.h"

#define SHADER_VERTEX 0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2
#define SHADER_CONTROL 2
#define SHADER_EVALUATION 3
#define SHADER_GEOMETRY2 4


using namespace std;

class Shader
{
public:
	Shader(string vertex, string fragment, string geometry = "");
	Shader(string vertex, string fragment, string control, string evaluation, string geometry = "");
	~Shader(void);

	GLuint GetProgram() { return program; }
	bool LinkProgram();

protected:
	void SetDefaultAttributes();
	bool LoadShaderFile(string from, string &into);
	GLuint GenerateShader(string from, GLenum type);

	GLuint objects[5];
	GLuint program;

	bool loadFailed;
};

