#ifndef __Panel_H__
#define __Panel_H__

#include <vector>
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_sdl_gl3.h"
#include "Imgui/imgui_impl_sdl.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

class Panel
{

public:
	Panel(ImVec2 _pos, ImVec2 _size);
	virtual void Draw() {};
	ImVec2 GetPosition() const;
	ImVec2 GetSize() const;

	bool SetActive(bool _active);

	bool active = true;

private:
	ImVec2 pos;
	ImVec2 size;


};


#endif // __Panel_H__
