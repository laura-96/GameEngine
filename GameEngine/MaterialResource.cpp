#include "MaterialResource.h"

void MaterialResource::Clear()
{
	path.clear();
	if (texture)
	{
		delete[] texture;
	}

	delete this;
}

void MaterialResource::SetPath(const char* material_path)
{
	path.clear();
	path.append(material_path);
}