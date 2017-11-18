#include "UFO.h"

Mesh * UFO::cube = NULL;

UFO::UFO(void)
{
	SceneNode * body = new SceneNode(cube, Vector4(1, 0, 0, 1));
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	body->SetBoundingRadius(15.0f);
	AddChild(body);

	hip = new SceneNode(cube, Vector4(1, 1, 1, 1));
	hip->SetModelScale(Vector3(10, 5, 5));
	hip->SetTransform(Matrix4::Translation(Vector3(0, -10, 0)));
	hip->SetBoundingRadius(5.0f);
	body->AddChild(hip);

	head = new SceneNode(cube, Vector4(0, 1, 0, 1));
	head->SetModelScale(Vector3(5, 5, 5));
	head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	head->SetBoundingRadius(5.0f);
	body->AddChild(head);

	hat = new SceneNode(cube, Vector4(0.5f, 0.35f, 0.05f, 1));
	hat->SetModelScale(Vector3(8, 0.5f, 8));
	hat->SetTransform(Matrix4::Translation(Vector3(0, 10, 0)));
	hat->SetBoundingRadius(5.0f);
	head->AddChild(hat);

	SceneNode * hatTop = new SceneNode(cube, Vector4(0.5f, 0.35f, 0.05f, 1));
	hatTop->SetModelScale(Vector3(5, 2, 5));
	hatTop->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	hatTop->SetBoundingRadius(5.0f);
	hat->AddChild(hatTop);

	leftShoulder = new SceneNode(cube, Vector4(0, 0.5f, 1, 1));
	leftShoulder->SetModelScale(Vector3(4, -4, 4));
	leftShoulder->SetTransform(Matrix4::Translation(Vector3(-14, 30, -1)));
	leftShoulder->SetBoundingRadius(18.0f);
	body->AddChild(leftShoulder);

	leftArm = new SceneNode(cube, Vector4(0, 0, 1, 1));
	leftArm->SetModelScale(Vector3(3, -9, 3));
	leftArm->SetTransform(Matrix4::Translation(Vector3(0, -1, 0)));
	leftArm->SetBoundingRadius(18.0f);
	leftShoulder->AddChild(leftArm);

	leftUnderArm = new SceneNode(cube, Vector4(0, 0, 1, 1));
	leftUnderArm->SetModelScale(Vector3(3, -9, 3));
	leftUnderArm->SetTransform(Matrix4::Translation(Vector3(0, -18, 0)) * Matrix4::Rotation(-45.0f, Vector3(1, 0, 0)));
	leftUnderArm->SetBoundingRadius(18.0f);
	leftArm->AddChild(leftUnderArm);

	rightShoulder = new SceneNode(cube, Vector4(0, 0.5f, 1, 1));
	rightShoulder->SetModelScale(Vector3(4, -4, 4));
	rightShoulder->SetTransform(Matrix4::Translation(Vector3(14, 30, -1)));
	rightShoulder->SetBoundingRadius(18.0f);
	body->AddChild(rightShoulder);

	rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1));
	rightArm->SetModelScale(Vector3(3, -9, 3));
	rightArm->SetTransform(Matrix4::Translation(Vector3(0, -1, 0)));
	rightArm->SetBoundingRadius(18.0f);
	rightShoulder->AddChild(rightArm);

	rightUnderArm = new SceneNode(cube, Vector4(0, 0, 1, 1));
	rightUnderArm->SetModelScale(Vector3(3, -9, 3));
	rightUnderArm->SetTransform(Matrix4::Translation(Vector3(0, -18, 0)) * Matrix4::Rotation(-45.0f, Vector3(1, 0, 0)));
	rightUnderArm->SetBoundingRadius(18.0f);
	rightArm->AddChild(rightUnderArm);

	leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1));
	leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	leftLeg->SetBoundingRadius(18.0f);
	hip->AddChild(leftLeg);

	rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1));
	rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	rightLeg->SetBoundingRadius(18.0f);
	hip->AddChild(rightLeg);
}

void UFO::Update(float msec) {
	transform = transform * Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));

	hat->SetTransform(hat->GetTransform() * Matrix4::Rotation(-msec / 5.0f, Vector3(0, 1, 0)));

	leftShoulder->SetTransform(leftShoulder->GetTransform() * Matrix4::Translation(Vector3(-4, -4, 0)) * Matrix4::Rotation(msec / 5.0f, Vector3(1, 0, 0)) * Matrix4::Translation(Vector3(4, 4, 0)));

	rightShoulder->SetTransform(rightShoulder->GetTransform() * Matrix4::Translation(Vector3(-4, -4, 0)) * Matrix4::Rotation(msec / 5.0f, Vector3(1, 0, 0)) * Matrix4::Translation(Vector3(4, 4, 0)));

	hip->SetTransform(hip->GetTransform() * Matrix4::Rotation(-msec / 5.0f, Vector3(0, 1, 0)));

	//leftLeg->SetTransform(leftLeg->GetTransform() * Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));

	//rightLeg->SetTransform(rightLeg->GetTransform() * Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));

	SceneNode::Update(msec);
}
