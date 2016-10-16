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

void TransformComponent::Rotate(math::float3 euler_rotation)
{
	rotation = rotation.FromEulerXYX(euler_rotation.x, euler_rotation.y, euler_rotation.z);
}

float TransformComponent::GetAngleInRad() const
{
	return rotation.Angle();
}

math::float3 TransformComponent::GetScale() const
{
	return math::float3(scale.x, scale.y, scale.z);
}

math::float3 TransformComponent::GetTranslation() const
{
	return math::float3(translation.x, translation.y, translation.z);
}

void TransformComponent::GetTransform(math::float4x4 &_transform) const
{
	//FromTRS returns the matrix that corresponds to specific translation (position), rotation and scale
	_transform = math::float4x4::FromTRS(translation, rotation.ToFloat3x3(), scale);
	_transform.Transpose();
}

void TransformComponent::Clear()
{
	delete this;
}
