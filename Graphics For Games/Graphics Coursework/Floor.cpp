#include "Floor.h"



Floor::Floor(const char * texture, const char * bumpmap,int w, int h)
{
	for (int i = 0; i < w; ++i) {
		for (int j = 0; j < h; ++j) {

			Mesh * nquad = Mesh::GenerateQuad();
			nquad->SetTexture(SOIL_load_OGL_texture(texture
				, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
			nquad->SetBumpMap(SOIL_load_OGL_texture(bumpmap
				, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

			SceneNode * tile = new SceneNode();

			tile->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
			tile->SetTransform(Matrix4::Translation(Vector3(200.0f*i, 0, 200.0f*j)) * Matrix4::Rotation(90,Vector3(1,0,0)));
			tile->SetBoundingRadius(100.0f);
			tile->SetMesh(nquad);
			AddChild(tile);
		}
	}
}


Floor::~Floor()
{
}
