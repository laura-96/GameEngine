#ifndef __GameObject_H__
#define __GameObject_H__

#include <vector>
#include <list>
#include <string>

#include "Component.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class MeshResource;
class MaterialResource;

class GameObject
{
public:

	GameObject(GameObject* _parent, const char* _name, uint uid);

	std::vector<GameObject*> GatherHierarchy(GameObject* root) const;

	MeshComponent* CreateMeshComponent(MeshResource* mesh_resource);
	MaterialComponent* CreateMaterialComponent(MaterialResource* material_resource);
	TransformComponent* CreateTransformComponent();
	CameraComponent* CreateCameraComponent();

	Component* FindComponent(const Component::ComponentType _type) const;
	const char* GetName() const;
	uint GetUID() const;
	const math::float3 GetPosition() const;
	
	void Update();
	void Draw() const;
	void GetBoundingBoxCorners(math::float3 ret[8]) const;
	void DrawBoundingBox() const;
	void Clear(); //Cleans up GameObject

public:

	std::vector<Component*> components;
	std::list<GameObject*> children;

	GameObject* GO_parent = nullptr;
	std::string name;
	//math::float4x4 global_transform = math::float4x4::identity;

	bool selected = false;
	bool active = true;

private:
	uint game_object_uid;
	math::AABB bounding_box;
};

#endif //__GameObject_H__
