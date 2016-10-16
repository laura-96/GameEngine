#include "PanelConfiguration.h"

#include "Globals.h"
#include "Module.h"
#include "Application.h"
#include "GOManager.h"
#include "ModuleInput.h"

PanelConfiguration::PanelConfiguration() : Panel(ImVec2(SCREEN_WIDTH - 320, 50), ImVec2(300, 100))
{

}

void PanelConfiguration::Draw()
{
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Separator();
	//ImGui::Text("Hello world.");
	ImGui::Checkbox("Full screen", &fullscreen);
	ImGui::End();
}