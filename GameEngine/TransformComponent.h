#ifndef __TransformComponent_H__
#define __TransformComponent_H__

#include "Component.h"

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

class TransformComponent : public Component
{
public:
	math::float3 translation;
	math::float3 scale;
	math::Quat rotation;

};

#endif // __TransformComponent_H__
