#include "Component.h"

#include "GameObject.h"
#include "MeshComponent.h"

#include "Globals.h"

Component::Component()
{}

Component::Component(ComponentType type, GameObject* _GO_belong)
{
	comp_type = type;
	GO_belong = GO_belong;

}

Component::ComponentType Component::GetType() const
{
	return comp_type;
}

