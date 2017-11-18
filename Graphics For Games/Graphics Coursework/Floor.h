#pragma once
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"

class Floor : public SceneNode
{
public:
	Floor(const char * texture,const char * bumpmap, int width = 4, int height = 4);
	~Floor();

};

