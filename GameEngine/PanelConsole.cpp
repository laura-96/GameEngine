#include "PanelConsole.h"

#include "Globals.h"


PanelConsole::PanelConsole() : Panel(ImVec2(10, SCREEN_HEIGHT - 150), ImVec2(SCREEN_WIDTH - 20, 100))
{

}

void PanelConsole::Draw()
{
	ImGui::Begin("Console", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Separator();
	ImGui::Text("Hello world.");

	ImGui::End();
}