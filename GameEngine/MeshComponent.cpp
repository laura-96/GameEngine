#include "MeshComponent.h"

#include "GameObject.h"
#include "MaterialComponent.h"

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

#include "Geometry.h"


void MeshComponent::Draw()
{
	if (enable)
	{
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		if (index_material >= 0)
		{
			MaterialComponent* mat = (MaterialComponent*)GO_belong->FindComponent(Component::ComponentType::Material);
			
			if (mat != nullptr && mat->enable)
			{
				glBindTexture(GL_TEXTURE_2D, mat->material_id);
				glEnable(GL_TEXTURE_2D);
			}
		}

		if (num_uvs > 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, id_uvs);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
		
		if (num_normals > 0)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, id_normals);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}

		glPushMatrix();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
		//glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_BYTE, NULL);
		glDrawArrays(GL_TRIANGLES, indices[0], num_index);
		
		glPopMatrix();


		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}
}

void MeshComponent::Clear()
{
	delete[] indices;
	delete[] vertices;
	delete[] uvs;
	delete[] normals;

	delete this;
}

	
