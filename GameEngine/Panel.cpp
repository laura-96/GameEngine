#include "Panel.h"

Panel::Panel(ImVec2 _pos, ImVec2 _size)
{
	pos = _pos;
	size = _size;

}

ImVec2 Panel::GetPosition() const
{
	return pos;
}

ImVec2 Panel::GetSize() const
{
	return size;
}

bool Panel::SetActive(bool _active)
{
	active = _active;
	return active;
}
