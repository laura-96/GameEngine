#pragma once

#include "glmath.h"
#include "Color.h"

enum GeometryTypes
{
	Direct_Cube,	//Cube drawn in direct mode
	In_Cube,		//Cube drawn with indices arrays
	Vx_Cube,		//Cube drawn with vertex arrays
	Model,
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
};


class VxCube : public Geometry
{
public:
	VxCube();
	VxCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};


class InCube : public Geometry
{
public:
	InCube();
	InCube(float sizeX, float sizeY, float sizeZ);
	//void InnerRender() const;
public:
	vec3 size;
};
