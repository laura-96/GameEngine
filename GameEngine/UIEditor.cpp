#include "Globals.h"
#include "Application.h"

#include "UIEditor.h"

#include "ModuleWindow.h"
#include "GOManager.h"

#include "Panel.h"
#include "PanelConfiguration.h"
#include "PanelConsole.h"
#include "ModuleFileSystem.h"
/*
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_sdl_gl3.h"
#include "Imgui/imgui_impl_sdl.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>*/


UIEditor::UIEditor(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
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

//	conf = new PanelConfiguration();
//	console = new PanelConsole();

//	panels.push_back(conf);
//	panels.push_back(console);

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
	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.48f, 0.72f, 1.0f, 0.49f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);

	return UPDATE_CONTINUE;
}

update_status UIEditor::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load", "Ctrl + L"))
			{
				select_load = true;
			}

			if (ImGui::MenuItem("Save", "Ctrl + S"))
			{
				App->Save();
			}

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

	if (select_load)
	{
		ImGui::SetNextWindowPosCenter();
		ImGui::SetNextWindowSize(ImVec2(300, 300));
		if (ImGui::Begin("FILES: "))
		{
			std::vector<std::string> scenes;
			App->file_sys->CollectFiles("Library/Scenes", scenes);

			std::vector<std::string>::iterator it = scenes.begin();

			while (it != scenes.end())
			{
				std::string file;
				App->file_sys->GetFileFromDir((*it).c_str(), file);

				if (ImGui::Button(file.c_str(), ImVec2(100, 30)))
				{

					App->go_manager->LoadFBXObjects((*it).c_str());
					select_load = false;
				}

				it++;
			}

			if (ImGui::Button("Close", ImVec2(100, 30)))
			{
				select_load = false;
			}
		}
		ImGui::End();
	}

	/*for (uint i = 0; i < panels.size(); i++)
	{
		if (panels[i]->active)
		{
			ImGui::SetNextWindowPos(panels[i]->GetPosition());
			ImGui::SetNextWindowSize(panels[i]->GetSize());

			panels[i]->Draw();

		}

	}*/

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

	/*for (uint i = 0; i < panels.size(); i++)
	{
		delete panels[i];
	}*/
	return true;
}