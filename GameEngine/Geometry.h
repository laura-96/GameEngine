#ifndef __Geometry_H__
#define __Geometry_H__

#include "glmath.h"
#include "Color.h"

#include "Globals.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64


enum GeometryTypes
{
	Direct_Cube,	//Cube drawn in direct mode
	In_Cube,		//Cube drawn with indices arrays
	Vx_Cube,		//Cube drawn with vertex arrays
	Point
};

class Geometry
{
public:

	Geometry();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	GeometryTypes	GetType() const;

public:

	Color color;
	mat4x4 transform;
	bool axis, wire;

protected:
	GeometryTypes type;
};

class DirectCube : public Geometry
{
public:
	DirectCube();
	DirectCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	GLuint imageName = -1;
};


class VxCube : public Geometry
{
public:
	VxCube();
	VxCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;

public:
	vec3 size;

private:
	GLfloat vertices[108];
	uint vertex = -1;
};


class InCube : public Geometry
{
public:
	InCube();
	InCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
private:
	GLubyte indices[36];
	GLfloat vertices[24];
	uint index = -1;
	uint vertex = -1;
};

#endif //__Geometry_H__