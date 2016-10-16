#include "MeshComponent.h"

#include "GameObject.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"

#include "glmath.h"

#include "Geometry.h"

void MeshComponent::Clear()
{
	if(indices != nullptr)
		delete[] indices;
	
	if(vertices != nullptr)
		delete[] vertices;

	if(uvs != nullptr)
		delete[] uvs;
	
	if(normals != nullptr)
		delete[] normals;

	delete this;
}

	
