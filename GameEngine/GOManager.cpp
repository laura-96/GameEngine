#include "Application.h"
#include "Globals.h"

#include "GOManager.h"

#include "Component.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "MeshResource.h"
#include "MaterialResource.h"
#include "ModuleSceneImporter.h"
#include "ModuleResourceManager.h"
#include "ModuleInput.h"
#include "ModuleFileSystem.h"

#include "Imgui/imgui.h"



GOManager::GOManager(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

GOManager::~GOManager()
{}

bool GOManager::Init(cJSON* node)
{
	
	return true;
}

update_status GOManager::Update(float dt)
{
	

	if (load_fbx && root_GO != nullptr)
	{
		EditorContent();
	}
	
	else if (root_GO == nullptr)
	{
		//Collecting files is not necessary here
		//Just doing this to get all files inside in the search path (then I will be able to work on it)
		std::vector<std::string> files;
		App->file_sys->CollectFiles("Library", files);

		LoadFBXObjects("Library/Scenes/Street.FBX.json");
	}
	//If right button of the mouse is pressed
	if (App->input->GetMouseButton(3) == KEY_STATE::KEY_DOWN)
	{
		create_go_pos = { (float) App->input->GetMouseX(), (float) App->input->GetMouseY() };
		create_go_editor = true;
		LOG("MOUSE PRESSED");
	}

	if (create_go_editor)
	{
		CreateGOEditor(create_go_pos);
	}


	return UPDATE_CONTINUE;
}

void GOManager::Draw() const
{
	if (load_fbx && root_GO != nullptr)
	{
		root_GO->Update();
	}
}

bool GOManager::LoadFBXObjects(const char* FBX)
{
	bool ret = false;
	char* buff = nullptr;

	std::string file;
	file.append(FBX);
	//file.append(".json\n");

	uint size = App->file_sys->Load(file.c_str(), &buff);

	if (buff != nullptr)
	{
		cJSON* root = cJSON_Parse(buff);
		cJSON* gos = root->child;
		std::vector<GameObject*> objects_created;
		
		root_GO = CreateGo(gos->string, cJSON_GetObjectItem(gos, "UID")->valueint, nullptr);
		objects_created.push_back(root_GO);

		gos = gos->next;

		while (gos)
		{
			std::vector<GameObject*>::iterator it = objects_created.begin();

			uint comp_uid = cJSON_GetObjectItem(gos, "Parent UID")->valueint;
			
			while (it != objects_created.end() && (*it)->GetUID() != comp_uid)
			{
				it++;
			}

			GameObject* go = CreateGo(gos->string, cJSON_GetObjectItem(gos, "UID")->valueint, (*it));
			LoadComponents(gos, go);

			objects_created.push_back(go);

			gos = gos->next;
		}
		ret = true;
	}
	
	return ret;
}

bool GOManager::LoadComponents(cJSON* components, GameObject* go) const
{
	
	bool ret = false;
	
	if (go != nullptr)
	{
		ret = true;
		
		//We look for the mesh and material associated to a prefab, through getting their UIDs, from the stored information in the prefab archive
		cJSON* prefab = cJSON_GetObjectItem(components, "Prefab");
		
		if (prefab)
		{
			char* buffer = nullptr;
			std::string file("Game/Library/Prefab/");
			file.append(prefab->valuestring);

			uint size = App->file_sys->Load(file.c_str(), &buffer);
			
			uint uids[3];
			memcpy(uids, buffer, size);

			//We get the suitable resource from resource manager functions
			MeshResource* mesh = App->resource_manager->GetMeshResource(uids[2]);
			//MaterialResource* material = App->resource_manager->GetMaterialResource(material_uid);

			if (mesh)
			{
				MeshComponent* mesh_comp = go->CreateMeshComponent(mesh);
				//Loading buffers for resources
				mesh->LoadBuffers();
			}
			//MaterialComponent* material_comp = go->CreateMaterialComponent(material);


			//mesh->gos_related.push_back(go);
			//material->gos_related.push_back(go);


			
		}

		cJSON* translation = cJSON_GetObjectItem(components, "Translation");
		cJSON* rotation = cJSON_GetObjectItem(components, "Rotation");
		cJSON* scale = cJSON_GetObjectItem(components, "Scale");

		TransformComponent* transform = go->CreateTransformComponent();

		transform->SetScale(cJSON_GetObjectItem(scale, "x")->valuedouble, cJSON_GetObjectItem(scale, "y")->valuedouble, cJSON_GetObjectItem(scale, "z")->valuedouble);
		transform->SetRotation(cJSON_GetObjectItem(rotation, "x")->valuedouble, cJSON_GetObjectItem(rotation, "y")->valuedouble, cJSON_GetObjectItem(rotation, "z")->valuedouble, cJSON_GetObjectItem(rotation, "w")->valuedouble);
		transform->SetTranslation(cJSON_GetObjectItem(translation, "x")->valuedouble, cJSON_GetObjectItem(translation, "y")->valuedouble, cJSON_GetObjectItem(translation, "z")->valuedouble);
		
		math::float4x4 matrix = math::float4x4::identity;
		
		transform->GetTransform(matrix);

	}

	return ret;
}



GameObject* GOManager::CreateGo(const char* name, uint uid, GameObject* parent) const
{
	GameObject* ret = new GameObject(parent, name, uid);

	return ret;
}

void GOManager::CreateGOEditor(math::float2 editor_pos)
{
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos(ImVec2(editor_pos.x, editor_pos.y));
	
	if (ImGui::Begin("Create GO", NULL, flags))
	{
	
		if (ImGui::TreeNodeEx("Create GO"))
		{
			if (ImGui::IsItemClicked(0))
			{
//				CreateGo("void object", root_GO);
			}

			ImGui::TreePop();
		}
	}
	
	ImGui::End();

	if (App->input->GetMouseButton(1) == KEY_STATE::KEY_DOWN && !ImGui::IsMouseHoveringAnyWindow())
	{
		create_go_editor = false;
	}

}

void GOManager::EditorContent()
{
	ImGui::SetNextWindowPos(ImVec2(20, 20));

	if (ImGui::Begin("Game Objects' Hierarchy:"))
	{
		if (root_GO)
		{
			ShowToEditor(root_GO);
		}
	}
	ImGui::End();

	if (selected != nullptr)
	{
		(*selected).selected = true;

		ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 300, 20));
		ImGui::SetNextWindowSize(ImVec2(250, 300));

		if (ImGui::Begin("Attribute editor"))
		{
			ImGui::TextColored(ImVec4(255, 255, 0, 1), (*selected).name.c_str());

			ImGui::Separator();

			ImGui::Checkbox("Enable", &enable);
			
			(*selected).active = enable;

			ImGui::Separator();
			
			MeshComponent* mesh = (MeshComponent*)(*selected).FindComponent(Component::ComponentType::Mesh);
			
			if (mesh != nullptr)
			{
				ImGui::Checkbox("- Mesh", &enable_mesh);
				mesh->Enable(enable_mesh);
			}

			MaterialComponent* material = (MaterialComponent*)(*selected).FindComponent(Component::ComponentType::Material);
			if (material != nullptr)
			{
				ImGui::Checkbox("- Material", &enable_material);			
				material->Enable(enable_material);

				ImGui::Text("	Texture path:");
				//ImGui::TextColored(ImVec4(0, 0, 255, 1), material->path.c_str());

				//ImGui::Image((ImTextureID)material->texture[0], ImVec2(100, 100));
			}

			ImGui::Separator();
			TransformComponent* transform = (TransformComponent*)(*selected).FindComponent(Component::ComponentType::Transform);

			if (transform != nullptr)
			{
				ImGui::Text("Translate");
				
				if (!translate)
				{
					original_translation = transform->GetTranslation();
					_translation = transform->GetTranslation();
				}

				if (ImGui::SliderFloat("x_translation", &_translation.x, -1000, 1000))
				{
					translate = true;
					transform->SetTranslation(_translation.x, _translation.y, _translation.z);
				}
				
				if (ImGui::SliderFloat("y_translation", &_translation.y, -1000, 1000))
				{
					translate = true;
					transform->SetTranslation(_translation.x, _translation.y, _translation.z);
				}

				if (ImGui::SliderFloat("z_translation", &_translation.z, -1000, 1000))
				{
					translate = true;
					transform->SetTranslation(_translation.x, _translation.y, _translation.z);
				}

				ImGui::Text("Scale");

				if (!scale)
				{
					original_scale = transform->GetScale();
					_scale = transform->GetScale();
				}

				if (ImGui::SliderFloat("x_scale", &_scale.x, -100, 100))
				{
					scale = true;
					transform->SetScale(_scale.x, _scale.y, _scale.z);
				}

				if (ImGui::SliderFloat("y_scale", &_scale.y, -100, 100))
				{
					scale = true;
					transform->SetScale(_scale.x, _scale.y, _scale.z);
				}

				if (ImGui::SliderFloat("z_scale", &_scale.z, -100, 100))
				{
					scale = true;
					transform->SetScale(_scale.x, _scale.y, _scale.z);
				}

				ImGui::Text("Rotate");

				if (!rotate)
				{
					//Should do from initial rotation, but it was to check if Rotation worked
					euler = {0, 0, 0};
				}

				if (ImGui::SliderAngle("x_rotate", &euler.x))
				{
					rotate = true;
					transform->Rotate(euler);
				}

				if (ImGui::SliderAngle("y_rotate", &euler.y))
				{
					rotate = true;
					transform->Rotate(euler);
				}

				if (ImGui::SliderAngle("z_rotate", &euler.z))
				{
					rotate = true;
					transform->Rotate(euler);
				}

				if(ImGui::Button("Reset translation"))
				{
					transform->SetTranslation(original_translation.x, original_translation.y, original_translation.z);
					_translation = original_translation;
				}

				if (ImGui::Button("Reset scale"))
				{
					transform->SetScale(original_scale.x, original_scale.y, original_scale.z);
					_scale = original_scale;
				}

				if (ImGui::Button("Reset rotation"))
				{
					transform->Rotate(math::float3(0, 0, 0));
					euler = {0, 0, 0};
				}
				
			}
		}

		ImGui::End();
	}

}

void GOManager::ShowToEditor(GameObject* go)
{
	if (ImGui::TreeNodeEx(go->name.c_str()))
	{
		if (!go->children.empty())
		{
			for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); it++)
			{
				ShowToEditor(*it);
			}
		}
		if (ImGui::IsItemClicked(0))
		{
			if (selected != nullptr)
			{
				(*selected).selected = false;
			}
			selected = go;
		}

		ImGui::TreePop();
	}
	

}

bool GOManager::CleanUp()
{
	if (root_GO)
	{
		GameObject* it = root_GO;
		std::vector<GameObject*> clear_nodes;

		while (it != nullptr)
		{
			if (!it->children.empty())
			{
				bool closed_child = false;
				std::list<GameObject*>::iterator i = it->children.begin();

				for (uint j = 0; j < clear_nodes.size(); j++)
				{
					if (clear_nodes[j] == (*i))
					{
						if ((*i) == it->children.back())
						{
							closed_child = true;
						}
						else
						{
							i++;
						}
					}
				}
				if (!closed_child)
				{
					it = (*i);
				}
				else
				{
					clear_nodes.push_back(it);
				}
			}
			else
			{
				clear_nodes.push_back(it);
			}

			for (uint j = 0; j < clear_nodes.size(); j++)
			{
				if (clear_nodes[j] == it)
				{
					it = it->GO_parent;
				}
			}
		}

		for (uint i = 0; i < clear_nodes.size(); i++)
		{
			clear_nodes[i]->Clear();
		}

		clear_nodes.clear();
	}
	
	return true;
}
