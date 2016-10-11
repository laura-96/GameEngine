#include "Component.h"

#include "GameObject.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"

#include "Globals.h"

Component::Component()
{}

Component::Component(ComponentType type, GameObject* _GO_belong)
{
	comp_type = type;
	GO_belong = _GO_belong;

}

Component::ComponentType Component::GetType() const
{
	return comp_type;
}

const GameObject* Component::GetGO() const
{
	return(GO_belong);
}