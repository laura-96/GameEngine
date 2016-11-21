#include "Resource.h"

#include "GameObject.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include "Globals.h"

Resource::Resource(Resource::ResourceType type, uint resource_uid)
{
	res_type = type;
	res_uid = resource_uid;
}

Resource::ResourceType Resource::GetType() const
{
	return res_type;
}

uint Resource::Count(std::vector<GameObject*> &go_using_res) const
{
	return NULL;
}