#ifndef __TransformComponent_H__
#define __TransformComponent_H__

#include "Component.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

//Found nice documentation of mathgeolib: http://clb.demon.fi/MathGeoLib/nightly/reference.html

class TransformComponent : public Component
{
public:
	TransformComponent(GameObject* _GO_belong) : Component(ComponentType::Transform, _GO_belong) {};

	void SetTranslation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z, float w);

	//Not const because I'll modify the value of matrix transform from translation, scale and rotation
	void GetTransform(math::float4x4 &_transform) const;

private:

	math::float3 translation;
	math::float3 scale;
	math::Quat rotation = math::Quat::identity;

};

#endif // __TransformComponent_H__
