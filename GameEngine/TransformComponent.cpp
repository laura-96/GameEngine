#include "TransformComponent.h"

void TransformComponent::SetTranslation(float x, float y, float z)
{
	translation.x = x;
	translation.y = y;
	translation.z = z;
}
void TransformComponent::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void TransformComponent::SetRotation(float x, float y, float z, float w)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	rotation.w = w;
}

void TransformComponent::GetTransform(math::float4x4 &_transform) const
{
	//FromTRS returns the matrix that corresponds to specific translation (position), rotation and scale
	_transform = math::float4x4::FromTRS(translation, rotation.ToFloat3x3(), scale);
	_transform.Transpose();
}
