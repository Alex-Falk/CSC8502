#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, TEXTURE2_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

	static Mesh * GeneratePlane(int x, int y = 0);

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }

	void SetTexture2(GLuint tex) { texture2 = tex; }
	GLuint GetTexture2() { return texture2; }

	void SetColour(Vector4 colour);

	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap() { return bumpTexture; }

	void SetBumpMap2(GLuint tex) { bumpTexture2 = tex; }
	GLuint GetBumpMap2() { return bumpTexture2; }

	void SetType(GLuint t) { type = t; }

protected:
	void BufferData();
	void GenerateNormals();

	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3 &a, const Vector3 &b,
		const Vector3 &c, const Vector2 & ta,
		const Vector2 & tb, const Vector2 & tc);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;
	GLuint numIndices;
	unsigned int* indices;

	GLuint texture;
	GLuint texture2;
	Vector2* textureCoords;

	GLuint bumpTexture;
	GLuint bumpTexture2;

	Vector3* vertices;
	Vector4* colours;

	Vector3* normals;
	Vector3 * tangents;

	
};

