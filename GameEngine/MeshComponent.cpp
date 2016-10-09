#include "MeshComponent.h"

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

void MeshComponent::Draw()
{
	glPushMatrix();

	glColor4f(0, 0, 0, 1);

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
}