#include "Globals.h"
#include "Application.h"

#include "GUI.h"

#include "ModuleWindow.h"
#include "ModuleSceneIntro.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_sdl_gl3.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>


GUI::GUI(Application* app, bool start_enabled ) : Module(app, start_enabled)
{

}

GUI::~GUI()
{

}

bool GUI::Init()
{
	bool ret = true;

	ImGui_ImplSdlGL3_Init(App->window->GetWindow());
	ImGuiIO& io = ImGui::GetIO();
	LOG("Initializing GUI. Imgui library version %s", ImGui::GetVersion());

	return ret;
}

bool GUI::Start()
{
	bool ret = true;

	return ret;
}

update_status GUI::PreUpdate(float dt)
{
	
	ImGui_ImplSdlGL3_NewFrame(App->window->GetWindow());
	ImGuiIO& io = ImGui::GetIO();
	
	capture_keyboard = io.WantCaptureKeyboard;
	capture_mouse = io.WantCaptureMouse;
	
	return UPDATE_CONTINUE;
}

update_status GUI::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit"))
			{
				ret = UPDATE_STOP;
			}
				
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return ret;
}

update_status GUI::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	
	
	ImGui::Render();

	return ret;
}

bool GUI::CleanUp()
{

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}