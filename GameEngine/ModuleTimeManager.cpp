#include "Application.h"
#include "ModuleTimeManager.h"
#include "ModuleFileSystem.h"
#include "GOManager.h"

ModuleTimeManager::ModuleTimeManager(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{

}


void ModuleTimeManager::ManagementTimeEditor()
{
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize;
	
	ImGui::SetNextWindowPosCenter();
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x, 20));

	if (ImGui::Begin("", NULL, flags))
	{
		if (ImGui::Button("Play", ImVec2(100, 30)))
		{
			char* buffer;
			uint size = App->file_sys->Load(App->go_manager->loaded_fbx.c_str(), &buffer);
			std::string file;
			App->file_sys->GetFileFromDir(App->go_manager->loaded_fbx.c_str(), file);

			App->file_sys->SaveInDir("Game/Library/Temp", file.c_str(), buffer, size);

		}  ImGui::SameLine();

		if (ImGui::Button("Pause", ImVec2(100, 30)))
		{
			dt = 0;
		} ImGui::SameLine();

		if (ImGui::Button("Stop", ImVec2(100, 30)))
		{
			std::string dir("Library/Temp/");
			std::string file;
			App->file_sys->GetFileFromDir(App->go_manager->loaded_fbx.c_str(), file);

			dir.append(file);
			App->go_manager->LoadFBXObjects(dir.c_str());
		}
	}
	ImGui::End();

}

update_status ModuleTimeManager::Update(float dt)
{
	ManagementTimeEditor();

	return UPDATE_CONTINUE;
}