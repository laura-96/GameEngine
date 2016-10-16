#ifndef __Log_H__
#define __Log_H__

#include "Globals.h"
#include "PanelConsole.h"
#include "UIEditor.h"
#include "Application.h"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	/*if (App->imgui->config)
	{
		App->imgui->config->ConsoleLog(tm_string2);
	}*/

}

#endif // __Log_H__