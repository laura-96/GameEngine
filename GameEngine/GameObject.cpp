#include "GameObject.h"

#include "Component.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"

#include "Globals.h"
#include "Color.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

using namespace std;



GameObject::GameObject(GameObject* _parent, const char* _name)
{
	GO_parent = _parent;
	name = _name;
	
	if (GO_parent != nullptr)
	{
		GO_parent->children.push_back(this);
	}
}


MeshComponent* GameObject::CreateMeshComponent()
{
	MeshComponent* ret = new MeshComponent((GameObject*)this);
	
	Component* comp = ret;
	components.push_back(comp);

	return ret;
}

MaterialComponent* GameObject::CreateMaterialComponent()
{
	MaterialComponent* ret = new MaterialComponent((GameObject*)this);

	Component* comp = ret;
	components.push_back(comp);

	return ret;
}

TransformComponent* GameObject::CreateTransformComponent()
{
	TransformComponent* ret = new TransformComponent((GameObject*)this);

	Component* comp = ret;
	components.push_back(comp);

	return ret;
}

Component* GameObject::FindComponent(const Component::ComponentType _type) const
{
	Component* ret = nullptr;
	if (!components.empty())
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->comp_type == _type)
			{
				ret = components[i];
				break;
			}
		}
	}
	
	return ret;
}

const char* GameObject::GetName() const
{
	return (name.c_str());
}

void GameObject::Update()
{
	glPushMatrix();

	TransformComponent* matrix = (TransformComponent*)FindComponent(Component::ComponentType::Transform);

	if (matrix != nullptr)
	{
		math::float4x4 mat = math::float4x4::identity;
		matrix->GetTransform(mat);

		glMultMatrixf(mat.ptr());
	}

	if (GO_parent != nullptr)
	{
		if (GO_parent->active == false)
		{
			active = GO_parent->active;
		}
	}

	if (active)
	{
		Draw();

		if (!children.empty())
		{
			for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
			{
				(*it)->Update();
			}
		}
	}
	glPopMatrix();
}

void GameObject::Draw() const
{
	glPushMatrix();
	
	MeshComponent* mesh = (MeshComponent*)FindComponent(Component::ComponentType::Mesh);

	if (mesh != nullptr && mesh->enable)
	{

		if (selected)
		{
			glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		
		if (mesh->index_material >= 0)
		{
			MaterialComponent* mat = (MaterialComponent*)FindComponent(Component::ComponentType::Material);

			if (mat != nullptr && mat->enable)
			{
				glEnableClientState(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, mat->texture[0]);
				glEnable(GL_TEXTURE_2D);
			}
		}

		if (mesh->num_uvs > 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uvs);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}

		if (mesh->num_normals > 0)
		{
			glEnable(GL_LIGHTING);
			glEnableClientState(GL_NORMAL_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);

		glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_TRIANGLES, mesh->vertices[0], mesh->num_vertex);
		
		//if (transf != nullptr)
			//glPopMatrix();

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}
}

void GameObject::Clear()
{
	name.clear();
	active = false;

	if (!children.empty())
	{
		children.clear();
	}

	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->Clear();
	}
	components.clear();

	delete (this);
}