#pragma once
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"

class UFO : public SceneNode
{
public:
	UFO(void);
	~UFO(void) {};
	virtual void Update(float msec);

	static void CreateCube() {
		OBJMesh * m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cube.obj");
		cube = m;
	}

	static void DeleteCube() { delete cube; }

protected:
	static Mesh * cube;
	SceneNode * head;
	SceneNode * leftArm;
	SceneNode * leftUnderArm;
	SceneNode * rightArm;
	SceneNode * rightUnderArm;
	SceneNode * leftLeg;
	SceneNode * rightLeg;
	SceneNode * hip;
	SceneNode * hat;
	SceneNode * leftShoulder;
	SceneNode * rightShoulder;
};

