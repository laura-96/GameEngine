#ifndef __CameraComponent_H__
#define __CameraComponent_H__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class CameraComponent : public Component
{
public:
	CameraComponent(GameObject* go) : Component(ComponentType::Camera, go) { };
	void SetPreferences(math::float3 position, float near_plane_dist, float far_plane_dist, float horizontal_fov, float aspect_ratio);
	void DrawFrustum() const;

private:
	math::Frustum frustum;
};

#endif