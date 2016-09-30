#pragma once

#include "glmath.h"
#include "Color.h"

enum GeometryTypes
{
	Direct_Cube,
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

/*
class Cube : public Geometry
{
public:
	Cube();
	Cube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};
*/