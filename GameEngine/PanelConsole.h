#ifndef __PanelConsole_H__
#define __PanelConsole_H__

#include "Panel.h"

class PanelConsole : public Panel
{
public:
	PanelConsole();

	void Draw();
	void ConsoleLog(const char* _log);

	ImGuiTextBuffer log_text;
};

#endif // __PanelConfiguration_H__