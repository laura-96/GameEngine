#ifndef __TransformComponent_H__
#define __TransformComponent_H__

#include "Component.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

class TransformComponent : public Component
{
public:
	TransformComponent(GameObject* _GO_belong) : Component(ComponentType::Transform, _GO_belong) {};

	void SetTranslation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z, float w);

	math::float3 translation;
	math::float3 scale;
	math::Quat rotation;

};

#endif // __TransformComponent_H__
