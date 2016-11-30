#include "Resource.h"

#include "GameObject.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#include "Globals.h"


Resource::Resource()
{}

Resource::Resource(Resource::ResourceType type, uint resource_uid, const char* resource_archive)
{
	res_type = type;
	res_uid = resource_uid;
	archive = resource_archive;

	uid_path.insert(std::pair<uint,const char*>(resource_uid, resource_archive));

}

Resource::ResourceType Resource::GetType() const
{
	return res_type;
}

uint Resource::Count(std::vector<GameObject*> &go_using_res) const
{
	go_using_res = gos_related;

	return gos_related.size();
}