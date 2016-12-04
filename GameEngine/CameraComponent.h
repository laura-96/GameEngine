#ifndef __CameraComponent_H__
#define __CameraComponent_H__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class CameraComponent : public Component
{
public:
	CameraComponent(GameObject* go) : Component(ComponentType::Camera, go) { };
	void SetPreferences(math::float3 position, float near_plane_dist, float far_plane_dist, float horizontal_fov, float aspect_ratio);
	float GetNearPlaneDist() const;
	float GetHorizontalFOV() const;
	float GetFarPlaneDist() const;
	float GetAspectRatio() const;
	math::float4x4 GetView() const;

	void SetDirection(float3 new_dir);
	void Rotate(float3 euler);

	void DrawFrustum() const;
	void ActivateCulling();
	void DeactivateCulling();
	void Clear();

	void SetFrustumPos(const math::float3 pos);

	//Culls every object outside the frustum if culling is active
	void FrustumCulling(std::vector<GameObject*> game_objects) const;
	bool IntersectsObject(GameObject* obj) const;

private:
	bool frustum_culling = false;
	math::Frustum frustum;
};

#endif