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
		root_objects = root_GO->GatherHierarchy(root_GO);
		std::vector<GameObject*>::iterator it = root_objects.begin();
		while (it != root_objects.end())
		{
			CameraComponent* cam = (CameraComponent*)(*it)->FindComponent(Component::ComponentType::Camera);

			if (cam != nullptr)
			{
				near_dist = cam->GetNearPlaneDist();
				far_dist = cam->GetFarPlaneDist();
				aspect_ratio = cam->GetAspectRatio();
			}
			
		}
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

bool GOManager::Save(cJSON* node)
{
	bool ret = false;

	cJSON_AddStringToObject(node, "Scene saved", loaded_fbx.c_str());

	std::vector<GameObject*> objects = root_GO->GatherHierarchy(root_GO);

	char* buffer = nullptr;
	uint size_load = App->file_sys->Load(loaded_fbx.c_str(), &buffer);

	cJSON* root = cJSON_Parse(buffer);

	if (root != nullptr)
	{
		uint size = cJSON_GetArraySize(root);
		uint i = 0;

		while (i < size)
		{
			std::vector<GameObject*>::iterator it = objects.begin();
			uint comp_uid = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "UID")->valuedouble;

			while (it != objects.end())
			{
				if ((*it)->GetUID() == comp_uid)
				{
					break;
				}

				it++;
			}

			cJSON* translation = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Translation");
			cJSON* rotation = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Rotation");
			cJSON* scale = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Scale");

			TransformComponent* transform = (TransformComponent*)(*it)->FindComponent(Component::ComponentType::Transform);
			if (transform)
			{
				cJSON_GetObjectItem(translation, "x")->valuedouble = transform->GetTranslation().x;
				cJSON_GetObjectItem(translation, "y")->valuedouble = transform->GetTranslation().y;
				cJSON_GetObjectItem(translation, "z")->valuedouble = transform->GetTranslation().z;

				cJSON_GetObjectItem(rotation, "x")->valuedouble = transform->rotation.x;
				cJSON_GetObjectItem(rotation, "y")->valuedouble = transform->rotation.y;
				cJSON_GetObjectItem(rotation, "z")->valuedouble = transform->rotation.z;
				cJSON_GetObjectItem(rotation, "w")->valuedouble = transform->rotation.w;

				cJSON_GetObjectItem(scale, "x")->valuedouble = transform->GetScale().x;
				cJSON_GetObjectItem(scale, "y")->valuedouble = transform->GetScale().y;
				cJSON_GetObjectItem(scale, "z")->valuedouble = transform->GetScale().z;
			}

			i++;
		}

		for (std::vector<GameObject*>::iterator it = created_objects.begin(); it != created_objects.end(); it++)
		{
			TransformComponent* transform_comp = (TransformComponent*)(*it)->FindComponent(Component::ComponentType::Transform);
			CameraComponent* camera_comp = (CameraComponent*)(*it)->FindComponent(Component::ComponentType::Camera);

			cJSON* item = cJSON_CreateObject();
			cJSON_AddItemToObject(root, (*it)->name.c_str(), item);

			cJSON_AddNumberToObject(item, "UID", (*it)->GetUID());
			if ((*it)->GO_parent != nullptr)
			{
				cJSON_AddNumberToObject(item, "Parent UID", (*it)->GO_parent->GetUID());
			}
			else
			{
				cJSON_AddNumberToObject(item, "Parent UID", NULL);
			}

			cJSON* translation = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "Translation", translation);
			cJSON_AddNumberToObject(translation, "x", transform_comp->GetTranslation().x);
			cJSON_AddNumberToObject(translation, "y", transform_comp->GetTranslation().y);
			cJSON_AddNumberToObject(translation, "z", transform_comp->GetTranslation().z);


			cJSON* rotation = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "Rotation", rotation);
			cJSON_AddNumberToObject(rotation, "x", transform_comp->rotation.x);
			cJSON_AddNumberToObject(rotation, "y", transform_comp->rotation.y);
			cJSON_AddNumberToObject(rotation, "z", transform_comp->rotation.z);
			cJSON_AddNumberToObject(rotation, "w", transform_comp->rotation.w);

			cJSON* scale = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "Scale", scale);
			cJSON_AddNumberToObject(scale, "x", transform_comp->GetScale().x);
			cJSON_AddNumberToObject(scale, "y", transform_comp->GetScale().y);
			cJSON_AddNumberToObject(scale, "z", transform_comp->GetScale().z);

			cJSON* cam = cJSON_CreateObject();
			cJSON_AddItemToObject(item, "Camera", cam);
			cJSON_AddNumberToObject(cam, "Near Plane", camera_comp->GetNearPlaneDist());
			cJSON_AddNumberToObject(cam, "Far Plane", camera_comp->GetFarPlaneDist());
			cJSON_AddNumberToObject(cam, "FOV", camera_comp->GetHorizontalFOV());
			cJSON_AddNumberToObject(cam, "Aspect Ratio", camera_comp->GetAspectRatio());

		}


		char* save = cJSON_Print(root);
		uint size_file = strlen(save);

		cJSON_Parse(save);

		std::string file_to_save;
		App->file_sys->GetFileFromDir(loaded_fbx.c_str(), file_to_save);

		if (!save_tmp_file)
		{
			App->file_sys->SaveInDir("Game/Library/Scenes", file_to_save.c_str(), save, size_file);
		}

		else
		{
			App->file_sys->SaveInDir("Game/Library/Temp", file_to_save.c_str(), save, size_file);
		}
		ret = true;
	}

	return ret;
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

			GameObject* go = nullptr;
			uint comp_uid = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Parent UID")->valuedouble;
			
			if (comp_uid != NULL)
			{
				for (; it != objects_created.end(); it++)
				{
					if ((*it)->GetUID() == comp_uid)
					{
						break;
					}
				}
				go = CreateGo(cJSON_GetArrayItem(root, i)->string, cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "UID")->valuedouble, (*it));

			}

			if(comp_uid == NULL)
			{
				go = CreateGo(cJSON_GetArrayItem(root, i)->string, cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "UID")->valuedouble, nullptr);
			}
			

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

			cJSON* camera = cJSON_GetObjectItem(cJSON_GetArrayItem(root, i), "Camera");

			if (camera != nullptr)
			{
				CameraComponent* cam = go->CreateCameraComponent();

				cam->SetPreferences(transform->GetTranslation(), cJSON_GetObjectItem(camera, "Near Plane")->valuedouble, cJSON_GetObjectItem(camera, "Far Plane")->valuedouble, cJSON_GetObjectItem(camera, "FOV")->valuedouble, cJSON_GetObjectItem(camera, "Aspect Ratio")->valuedouble);
				cam->ActivateCulling();
			}

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
			MaterialResource* material = App->resource_manager->GetMaterialResource(uids[2]);

			if (material)
			{
				MaterialComponent* material_comp = go->CreateMaterialComponent(material);
			}

			if (mesh)
			{
				MeshComponent* mesh_comp = go->CreateMeshComponent(mesh);
				//Loading buffers for resources
				mesh->LoadBuffers();
			}
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
						GameObject* go = CreateGo("Void object", uid.IntFast(), root_GO);
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
						GameObject* go = CreateGo("Camera", uid.IntFast(), root_GO);
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

				ImGui::Image((ImTextureID)material->GetTexture(), ImVec2(100, 100));
			}
			
			CameraComponent* camera = (CameraComponent*)(*selected).FindComponent(Component::ComponentType::Camera);

			

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
					if (camera)
					{
						camera->Rotate(euler);
					}
				}

				if (ImGui::SliderAngle("y_rotate", &euler.y))
				{
					rotate = true;
					transform->Rotate(euler);
					if (camera)
					{
						camera->Rotate(euler);
					}
				}

				if (ImGui::SliderAngle("z_rotate", &euler.z))
				{
					rotate = true;
					transform->Rotate(euler);
					if (camera)
					{
						camera->Rotate(euler);
					}
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
					if (camera)
					{
						camera->Rotate(math::float3(0, 0, 0));
					}
				}
				
			}

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
					camera->SetPreferences(transform->GetTranslation(), near_dist, far_dist, camera->GetHorizontalFOV(), aspect_ratio);
				}

				if (ImGui::SliderFloat("Far Plane distance", &far_dist, near_dist, 1000))
				{
					camera->SetPreferences(transform->GetTranslation(), near_dist, far_dist, camera->GetHorizontalFOV(), aspect_ratio);
				}

				if (ImGui::SliderFloat("Aspect Ratio", &aspect_ratio, 0, 10))
				{
					camera->SetPreferences(transform->GetTranslation(), near_dist, far_dist, camera->GetHorizontalFOV(), aspect_ratio);
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

	created_objects.clear();

	root_objects.clear();

	return true;
}
