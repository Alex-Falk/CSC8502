#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "../../nclgl/Mesh.h"

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
//#define HEIGHTMAP_Y 5.0f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

class HeightMap : public Mesh
{
public:
	HeightMap(std::string name,int RAW_WIDTH = 1080, int RAW_HEIGHT = 1080, float HEIGHTMAP_Y = 5.0f);
	~HeightMap(void);

	void Scale(float factor);

	Vector3* originalVertices;

	int RAW_HEIGHT;
	int RAW_WIDTH;
	float HEIGHTMAP_Y;
};

