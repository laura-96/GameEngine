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



GameObject::GameObject(GameObject* _parent, const char* _name, uint uid)
{
	GO_parent = _parent;
	name = _name;
	game_object_uid = uid;

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

uint GameObject::GetUID() const
{
	return (game_object_uid);
}

const math::float3 GameObject::GetPosition() const
{

	TransformComponent* transform = (TransformComponent*)FindComponent(Component::ComponentType::Transform);
	return transform->GetTranslation();
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
	bounding_box.SetNegativeInfinity();

	MeshComponent* mesh = (MeshComponent*)FindComponent(Component::ComponentType::Mesh);

	//if (mesh != nullptr)
		//bounding_box.Enclose((float3*)mesh->vertices, mesh->num_vertex);

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

		else
		{
			glColor4f(White.r, White.g, White.b, White.a);
		}
/*
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
		*/
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);

		glLineWidth(2.0f);
		glColor3f(0.f, 1.f, 0.f);

		glBegin(GL_LINE_LOOP);

		glVertex3f(bounding_box.CornerPoint(0).x, bounding_box.CornerPoint(0).y, bounding_box.CornerPoint(0).z);
		glVertex3f(bounding_box.CornerPoint(1).x, bounding_box.CornerPoint(1).y, bounding_box.CornerPoint(1).z);
		glVertex3f(bounding_box.CornerPoint(3).x, bounding_box.CornerPoint(3).y, bounding_box.CornerPoint(3).z);
		glVertex3f(bounding_box.CornerPoint(2).x, bounding_box.CornerPoint(2).y, bounding_box.CornerPoint(2).z);

		glEnd();

		glBegin(GL_LINE_LOOP);

		glVertex3f(bounding_box.CornerPoint(4).x, bounding_box.CornerPoint(4).y, bounding_box.CornerPoint(4).z);
		glVertex3f(bounding_box.CornerPoint(5).x, bounding_box.CornerPoint(5).y, bounding_box.CornerPoint(5).z);
		glVertex3f(bounding_box.CornerPoint(7).x, bounding_box.CornerPoint(7).y, bounding_box.CornerPoint(7).z);
		glVertex3f(bounding_box.CornerPoint(6).x, bounding_box.CornerPoint(6).y, bounding_box.CornerPoint(6).z);

		glEnd();

		/*
		glBegin(GL_LINE_LOOP);

		glVertex3f(bounding_box.CornerPoint(5).x, bounding_box.CornerPoint(5).y, bounding_box.CornerPoint(5).z);
		glVertex3f(bounding_box.CornerPoint(0).x, bounding_box.CornerPoint(0).y, bounding_box.CornerPoint(0).z);
		glVertex3f(bounding_box.CornerPoint(6).x, bounding_box.CornerPoint(6).y, bounding_box.CornerPoint(6).z);
		glVertex3f(bounding_box.CornerPoint(1).x, bounding_box.CornerPoint(1).y, bounding_box.CornerPoint(1).z);

		glEnd();

		glBegin(GL_LINE_LOOP);

		glVertex3f(bounding_box.CornerPoint(4).x, bounding_box.CornerPoint(4).y, bounding_box.CornerPoint(4).z);
		glVertex3f(bounding_box.CornerPoint(7).x, bounding_box.CornerPoint(7).y, bounding_box.CornerPoint(7).z);
		glVertex3f(bounding_box.CornerPoint(2).x, bounding_box.CornerPoint(2).y, bounding_box.CornerPoint(2).z);
		glVertex3f(bounding_box.CornerPoint(3).x, bounding_box.CornerPoint(3).y, bounding_box.CornerPoint(3).z);

		glEnd();*/
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