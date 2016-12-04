#include "MaterialResource.h"

void MaterialResource::Clear()
{
	path.clear();

	delete this;
}

void MaterialResource::SetPath(const char* material_path)
{
	path.clear();
	path.append(material_path);
}