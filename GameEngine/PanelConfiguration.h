#ifndef __PanelConfiguration_H__
#define __PanelConfiguration_H__

#include "Panel.h"

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration();

	void Draw();
private:
	bool fullscreen = false;
};

#endif // __PanelConfiguration_H__