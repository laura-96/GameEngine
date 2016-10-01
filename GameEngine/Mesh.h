#pragma once

#include "glmath.h"
#include "Globals.h"
#include "Color.h"

#include <vector>
#include <list>

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

//using namespace std;

class mesh {
public:
	mesh() : index(0), vertex(0) {};
	mesh(uint _vertex) : vertex(_vertex) { };

	uint* indices = nullptr;
	float* vertices = nullptr;
	uint index = 0;
	uint vertex = 0;

	uint id_index = 0;
	uint id_vertex = 0;

	mat4x4 transform;
	Color color = White;
};

class Mesh {
public:
	Mesh();
	~Mesh();

	bool LoadMesh(const char* file);
	void Render() const;

	bool CleanUp();

	//list<mesh*> models;
	mesh* model;
};