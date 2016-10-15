#include "MaterialComponent.h"

void MaterialComponent::Clear()
{
	path.clear();
	delete this;
}