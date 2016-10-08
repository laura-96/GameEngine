#ifndef __Mesh_H__
#define __Mesh_H__

#include "MathGeoLib/src/MathBuildConfig.h"
#include "MathGeoLib/src/MathGeoLib.h"

//#include "glmath.h"

#include "Globals.h"
#include "Color.h"

#include <vector>
#include <list>

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

struct mesh {
public:
	mesh() : index(0), vertex(0) {};
	mesh(uint _vertex) : vertex(_vertex) { };

	uint* indices = nullptr;
	float* vertices = nullptr;
	uint index = 0;
	uint vertex = 0;

	uint id_index = -1;
	uint id_vertex = -1;

	math::float4x4 transform = math::float4x4::identity;
	//mat4x4 transform = IdentityMatrix;
	
	Color color = White;
};

class Mesh {
public:
	Mesh();
	~Mesh();

	bool LoadMesh(const char* file);
	void Render() const;

	bool CleanUp();

	mesh* model;
};

#endif // __Mesh_H__