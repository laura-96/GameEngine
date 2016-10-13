#include "Globals.h"
#include "Application.h"

#include "UIEditor.h"

#include "ModuleWindow.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_sdl_gl3.h"
#include "Imgui/imgui_impl_sdl.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>


UIEditor::UIEditor(Application* app, const char* name, bool start_enabled ) : Module(app, name, start_enabled)
{

}

UIEditor::~UIEditor()
{

}

bool UIEditor::Init(cJSON* node)
{
	bool ret = true;

	ImGui_ImplSdlGL3_Init(App->window->GetWindow());
	ImGuiIO& io = ImGui::GetIO();
	LOG("Initializing GUI. Imgui library version %s", ImGui::GetVersion());
	return ret;
}

bool UIEditor::Start()
{
	bool ret = true;

	return ret;
}

update_status UIEditor::PreUpdate(float dt)
{
	
	ImGui_ImplSdlGL3_NewFrame(App->window->GetWindow());
	ImGuiIO& io = ImGui::GetIO();
	
	capture_keyboard = io.WantCaptureKeyboard;
	capture_mouse = io.WantCaptureMouse;
	
	return UPDATE_CONTINUE;
}

update_status UIEditor::Update(float dt)
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

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Repository"))
			{
				const char* link = "https://github.com/laura-96/GameEngine";
				ShellExecuteA(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
			}
			if (ImGui::MenuItem("Report bugs"))
			{
				const char* link1 = "https://github.com/laura-96/GameEngine/issues";
				ShellExecuteA(NULL, "open", link1, NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	return ret;
}

update_status UIEditor::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	
	return ret;
}

bool UIEditor::Draw()
{

	ImGui::Render();

	return true;
}

bool UIEditor::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();

	return true;
}