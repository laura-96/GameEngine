#include "Geometry.h"

#include "Globals.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

Geometry::Geometry() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(GeometryTypes::Point)
{

}

GeometryTypes Geometry::GetType() const
{
	return type;
}

void Geometry::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(1.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

void Geometry::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

void Geometry::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}


void Geometry::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

void Geometry::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// Direct Cube

DirectCube::DirectCube() : Geometry(), size(1.0f, 1.0f, 1.0f)
{
	type = GeometryTypes::Direct_Cube;
}

DirectCube::DirectCube(float sizeX, float sizeY, float sizeZ) : Geometry(), size(sizeX, sizeY, sizeZ)
{
	type = GeometryTypes::Direct_Cube;
}

void DirectCube::InnerRender() const
{
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_TRIANGLES);

	//FRONT FACE ------------------------------

	//1 triangle vx0, vx1, vx2
	glVertex3f(sx, sy, sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, -sy, sz);
	
	//2 triangle vx2, vx3, vx0
	glVertex3f(-sx, -sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, sy, sz);

	//TOP FACE --------------------------------

	//1 triangle vx0, vx5, vx6
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	//2 triangle vx6, vx1, vx0
	glVertex3f(-sx, sy, -sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(sx, sy, sz);

	//RIGHT FACE -------------------------------

	//1 triangle vx0, vx3, vx4
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, -sy, -sz);

	//2 triangle vx4, vx5, vx0
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, sy, sz);

	//BACK FACE -------------------------------

	//1 triangle vx7, vx6, vx5
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(sx, sy, -sz);

	//2 triangle vx5, vx4, vx7
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);

	//BOTTOM FACE ----------------------------

	//1 triangle vx7, vx4, vx3
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, -sy, sz);

	//2 triangle vx3, vx2, vx7
	glVertex3f(sx, -sy, sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, -sy, -sz);

	//LEFT FACE ------------------------------

	//1 triangle vx7, vx2, vx1
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, sy, sz);

	//2 triangle vx1, vx6, vx7
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(-sx, -sy, -sz);

	glEnd();
}
