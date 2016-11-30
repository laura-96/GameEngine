#ifndef __MaterialResource_H__
#define __MaterialResource_H__

#include "Resource.h"
#include "Globals.h"
#include "Color.h"

#include <string>
#include <map>

class MaterialResource : public Resource
{
public:
	MaterialResource(uint mat_uid, const char* archive) : Resource(ResourceType::Material, mat_uid, archive) {};
	void Clear();

	void SetPath(const char* material_path);

	uint material_id = -1;

	uint id_image = -1;
	uint texture[1];

private:
	std::string path;

};


#endif //__MaterialResource_H__
