#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include <algorithm>

class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1,1,1,1));
	~SceneNode(void);

	void SetTransform(const Matrix4 &matrix) { transform = matrix; }
	void SetWorldTransform(const Matrix4 &matrix) { worldTransform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	void SetTexture(GLuint tex) { mesh->SetTexture(tex); }

	Vector3 GetModelScale();// { return modelScale; }
	Vector3 GetRootScale();
	void SetModelScale(Vector3 s);
	void ChangeModelScale(Vector3 s);
	void Scale(Vector3 s);

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	void AddChild(SceneNode* s);
	void RemoveChild(SceneNode* s);

	SceneNode* GetChild(int idx) { return children[idx]; }

	virtual void Update(float msec);
	virtual void Draw(); //const OGLRenderer &r

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	Shader * GetShader() { return shader; }

	static bool CompareByCameraDistance(SceneNode * a, SceneNode * b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;

	Shader * shader;

	float distanceFromCamera;
	float boundingRadius;
};

