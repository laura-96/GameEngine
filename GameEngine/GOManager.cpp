#include "Application.h"
#include "Globals.h"

#include "GOManager.h"

#include "Component.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

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
		root_objects = root_GO->GatherHierarchy(root_GO);
	}
	
	else if (root_GO == nullptr)
	{
		//Collecting files is not necessary here
		//Just doing this to get all files inside in the search path (then I will be able to work on it)
		std::vector<std::string> files;
		App->file_sys->CollectFiles("Library", files);

		//LoadFBXObjects("Library/Scenes/Street.FBX.json");
		
	}

	//If right button of the mouse is pressed
	if (App->input->GetMouseButton(3) == KEY_STATE::KEY_DOWN)
	{
		create_go_pos = { (float) App->input->GetMouseX(), (float) App->input->GetMouseY() };
		create_go_editor = true;
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

void GOManager::DrawSceneObjects() const
{
	for (uint i = 0; i < created_objects.size(); i++)
	{
		created_objects[i]->Update();

		CameraComponent* cam = (CameraComponent*)created_objects[i]->FindComponent(Component::ComponentType::Camera);
		cam->FrustumCulling(root_objects);
	}
}

bool GOManager::LoadFBXObjects(const char* FBX)
{
	bool ret = false;
	char* buff = nullptr;

	std::string file("Game/");
	file.append(FBX);
	//file.append(".json\n");

	uint size = App->file_sys->Load(file.c_str(), &buff);

	if (buff != nullptr)
	{
		loaded_fbx.clear();
		loaded_fbx.append(FBX);

		cJSON* root = cJSON_Parse(buff);
	
		std::vector<GameObject*> objects_created;
		
		root_GO = CreateGo(cJSON_GetArrayItem(root, 0)->string, cJSON_GetObjectItem(cJSON_GetArrayItem(root, 0), "UID")->valuedouble, nullptr);
		cJSON* prefab = cJSON_GetObjectItem(cJSON_GetArrayItem(root, 0), "Prefab");
		
		if (prefab != nullptr)
		{
			const char* pref_string = prefab->valuestring;
			LoadComponents(pref_string, root_GO);
		}
		

		cJSON* translation = cJSON_GetObjectItem(cJSON_GetArrayItem(root, 0), "Translation");
		cJSON* rotation = cJSON_GetObjectItem(cJSON_GetArrayItem(root, 0), "Rotation");
		cJSON* scale = cJSON_GetObjectItem(cJSON_GetArrayItem(root, 0), "Scale");


		TransformComponent* transform = root_GO->CreateTransformComponent();

		transform->SetScale(cJSON_GetObjectItem(scale, "x")->valuedouble, cJSON_GetObjectItem(scale, "y")->valuedouble, cJSON_GetObjectItem(scale, "z")->valuedouble);
		transform->SetRotation(cJSON_GetObjectItem(rotation, "x")->valuedouble, cJSON_GetObjectItem(rotation, "y")->valuedouble, cJSON_GetObjectItem(rotation, "z")->valuedouble, cJSON_GetObjectItem(rotation, "w")->valuedouble);
		transform->SetTranslation(cJSON_GetObjectItem(translation, "x")->valuedouble, cJSON_GetObjectItem(translation, "y")->valuedouble, cJSON_GetObjectItem(translation, "z")->valuedouble);

		math::float4x4 matrix = math::float4x4::identity;

		transform->GetTransform(matrix);

		objects_created.push_back(root_GO);
		
		uint size = cJSON_GetArraySize(root);
		uint i = 1;

		while (i < size)
		{
			std::vector<GameObject*>::iterator it = objects_created.begin();

			uint comp_uid = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Parent UID")->valuedouble;

			for (; it != objects_created.end(); it++)
			{
				if ((*it)->GetUID() == comp_uid)
				{
					break;
				}
			}

			GameObject* go = CreateGo(cJSON_GetArrayItem(root, i)->string, cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "UID")->valuedouble, (*it));
			
			cJSON* go_prefab = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Prefab");

			if (go_prefab != nullptr)
			{
				const char* go_pref_string = go_prefab->valuestring;
				LoadComponents(go_pref_string, go);
			}

			cJSON* translation = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Translation");
			cJSON* rotation = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Rotation");
			cJSON* scale = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Scale");


			TransformComponent* transform = go->CreateTransformComponent();

			transform->SetScale(cJSON_GetObjectItem(scale, "x")->valuedouble, cJSON_GetObjectItem(scale, "y")->valuedouble, cJSON_GetObjectItem(scale, "z")->valuedouble);
			transform->SetRotation(cJSON_GetObjectItem(rotation, "x")->valuedouble, cJSON_GetObjectItem(rotation, "y")->valuedouble, cJSON_GetObjectItem(rotation, "z")->valuedouble, cJSON_GetObjectItem(rotation, "w")->valuedouble);
			transform->SetTranslation(cJSON_GetObjectItem(translation, "x")->valuedouble, cJSON_GetObjectItem(translation, "y")->valuedouble, cJSON_GetObjectItem(translation, "z")->valuedouble);

			math::float4x4 matrix = math::float4x4::identity;

			transform->GetTransform(matrix);

			objects_created.push_back(go);
			i++;
		}
		ret = true;
		cJSON_Delete(root);
	}
	
	return ret;
}

bool GOManager::LoadComponents(const char* prefab, GameObject* go) const
{
	
	bool ret = false;
	
	if (go != nullptr)
	{
		ret = true;
		
		if (prefab != nullptr)
		{
			char* buffer = nullptr;
			std::string file("Game/Library/Prefab/");
			file.append(prefab);

			uint size = App->file_sys->Load(file.c_str(), &buffer);
			
			uint uids[3];
			memcpy(uids, buffer, size);

			//We get the suitable resource from resource manager functions
			MeshResource* mesh = App->resource_manager->GetMeshResource(uids[1]);
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
		ImGuiWindowFlags_NoMove;
		//ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos(ImVec2(editor_pos.x, editor_pos.y));
	
	if (ImGui::Begin("Create GO", NULL, flags))
	{
	
		if (ImGui::TreeNodeEx("Create GO"))
		{
			
				if (ImGui::TreeNode("Void object"))
				{
					if (ImGui::IsItemClicked(0))
					{
						math::LCG uid = math::LCG();
						GameObject* go = CreateGo("Void object", uid.IntFast(), nullptr);
						TransformComponent* transform = go->CreateTransformComponent();

						transform->SetTranslation(0, 0, 0);
						transform->SetScale(1, 1, 1);
						transform->SetRotation(0, 0, 0, 1);

						created_objects.push_back(go);
						create_go_editor = false;
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("With camera component"))
				{
					if (ImGui::IsItemClicked(0))
					{
						math::LCG uid = math::LCG();
						GameObject* go = CreateGo("Camera", uid.IntFast(), nullptr);
						TransformComponent* transform = go->CreateTransformComponent();

						transform->SetTranslation(0, 0, 0);
						transform->SetScale(1, 1, 1);
						transform->SetRotation(0, 0, 0, 1);

						CameraComponent* camera = go->CreateCameraComponent();
						camera->SetPreferences(transform->GetTranslation(), 5, 20, math::pi / 4, 2);
						camera->ActivateCulling();

						created_objects.push_back(go);
						create_go_editor = false;
					}

					ImGui::TreePop();
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

		if (ImGui::TreeNode("Scene"))
		{
			if(root_GO)
				ShowToEditor(root_GO);

			for (uint i = 0; i < created_objects.size(); i++)
			{
				ShowToEditor(created_objects[i]);
			}

			ImGui::TreePop();
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
				enable_mesh = mesh->enable;

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
			
			CameraComponent* camera = (CameraComponent*)(*selected).FindComponent(Component::ComponentType::Camera);

			if (camera != nullptr)
			{
				ImGui::Checkbox("- Camera", &enable_camera);
				camera->Enable(enable_camera);

				ImGui::Checkbox("	Frustum culling", &camera_culling);
				
				if (camera_culling)
				{
					camera->ActivateCulling();
				}

				else
				{
					camera->DeactivateCulling();
				}

				if (ImGui::SliderFloat("Near Plane distance", &near_dist, 0, far_dist))
				{
					
				}

				if (ImGui::SliderFloat("Far Plane distance", &far_dist, near_dist, 100))
				{

				}

				if (ImGui::SliderFloat("Aspect Ratio", &aspect_ratio, 0, 10))
				{

				}



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
