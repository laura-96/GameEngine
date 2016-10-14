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
