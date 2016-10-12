#include "Application.h"
#include "Globals.h"

#include "GOManager.h"


#include "Component.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Devil/include/il.h"
#include "Devil/include/ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

GOManager::GOManager(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

GOManager::~GOManager()
{}

bool GOManager::Init(cJSON* node)
{
	
	if (load_fbx == true)
	{
		ilInit();
		ilutInit();

		ilutRenderer(ILUT_OPENGL);

		struct aiLogStream stream;
		stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
		aiAttachLogStream(&stream);

		LoadFBXObjects("Game/Assets/Town/Street.FBX");
	}

	return true;
}

update_status GOManager::Update(float dt)
{
	if (load_fbx)
	{
		root_GO->Update();
	}

	return UPDATE_CONTINUE;
}

void GOManager::DrawGOs() const
{
	//root_GO->GetChildAt(0)->Update();
}

bool GOManager::LoadFBXObjects(const char* FBX)
{
	bool ret = true;
	const aiScene* scene = aiImportFile(FBX, aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->mRootNode != nullptr && scene->HasMeshes())
	{
		aiNode* node = scene->mRootNode;
		std::vector<aiNode*> closed_nodes;
	
		GameObject* new_go = CreateGo(node->mName.C_Str(), nullptr);

		if (!LoadComponents(scene, node, new_go))
		{
			ret = false;
		}
		root_GO = &(*new_go);

		while (node != nullptr)
		{
			if (node->mNumChildren > 0)
			{
				uint i = 0;

				for (uint j = 0; j < closed_nodes.size(); j++)
				{
					if (closed_nodes[j] == node->mChildren[i])
					{
						i++;
					}
				}
				
				if (i >= node->mNumChildren)
				{
					closed_nodes.push_back(node);
				}

				else
				{
					node = node->mChildren[i];
					new_go = CreateGo(node->mName.C_Str(), new_go);

					if (!LoadComponents(scene, node, new_go))
					{
						ret = false;
					}

					if (node->mParent != nullptr)
					{
						LOG("Parent: %s ---- Node name: %s", node->mParent->mName.C_Str(), node->mName.C_Str());
						LOG("GO Parent: %s ---- Node name: %s", new_go->GO_parent->GetName(), new_go->GetName());
					}
					else
					{
						LOG("Node name: %s", node->mName.C_Str());
					}
				}
			}

			else
			{
				closed_nodes.push_back(node);
			}

			for (uint j = 0; j < closed_nodes.size(); j++)
			{
				if (closed_nodes[j] == node)
				{
					node = node->mParent;
					new_go = new_go->GO_parent;
				}
			}
		}
		closed_nodes.clear();
	}

	else
	{
		ret = false;
		LOG("Error loading scene: %s", FBX);
	}

	aiReleaseImport(scene);

	return ret;
}

bool GOManager::LoadComponents(const aiScene* scene, const aiNode* node, GameObject* go) const
{
	bool ret = false;

	if (go != nullptr && node != nullptr)
	{
		ret = true;

		uint material = -1;

		if (node->mNumMeshes > 0)
		{
			MeshComponent* mesh = go->CreateMeshComponent();

			for (uint i = 0; i < node->mNumMeshes; i++)
			{
				uint index_scene = node->mMeshes[i];

				aiMesh* meshes = scene->mMeshes[index_scene];
				
				mesh->num_vertex = meshes->mNumVertices;
				mesh->vertices = new float[mesh->num_vertex * 3];

				memcpy(mesh->vertices, meshes->mVertices, sizeof(float) * mesh->num_vertex * 3);

				LOG("New mesh with %d vertices", mesh->num_vertex);

				if (meshes->HasFaces())
				{
					mesh->num_index = meshes->mNumFaces * 3;
					mesh->indices = new uint[mesh->num_index];

					for (uint j = 0; j < meshes->mNumFaces; ++j)
					{
						if (meshes->mFaces[j].mNumIndices != 3)
						{
							LOG("WARNING! Geometry face with %d indices", meshes->mFaces[j].mNumIndices);
						}

						else
						{
							memcpy(&mesh->indices[j * 3], meshes->mFaces[j].mIndices, 3 * sizeof(uint));
						}
					}
				}
				/*if (meshes->HasTextureCoords(0))
				{
					mesh->uvs = new float[mesh->num_vertex * 3];
				
					memcpy(mesh->uvs, meshes->mTextureCoords[0], sizeof(float) * mesh->num_vertex * 3);

				}*/
				/*
				mesh->index_material = meshes->mMaterialIndex;
				if (mesh->index_material >= 0)
				{
					mesh->has_material = true;
				}
				material = mesh->index_material;*/
			}

			glGenBuffers(1, (GLuint*) &(mesh->id_index));
			glGenBuffers(1, (GLuint*) &(mesh->id_vertex));
			//glGenBuffers(1, (GLuint*) &(mesh->id_uvs));

			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, mesh->vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_index, mesh->indices, GL_STATIC_DRAW);

			//glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uvs);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, mesh->uvs, GL_STATIC_DRAW);

		}
		/*
		if (scene->mNumMaterials > 0 && node->mNumMeshes > 0)
		{
			aiMaterial* mat = scene->mMaterials[material];
			
			assert(mat);

			if (mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0)
			{
				aiString path;
				mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);

				MaterialComponent* mat_component = go->CreateMaterialComponent();
				mat_component->path.clear();

				if (mat_component->path.length() < path.length)
				{
					mat_component->path.reserve(path.length);
				}
				else if (mat_component->path.length() > path.length)
				{
					mat_component->path.resize(path.length);
				}

				mat_component->path = path.C_Str();
				mat_component->material_id = material;

				LOG("MATERIAL 1 - %s ___ 2 - %s", mat_component->path.c_str(), path.C_Str());
				
				ilGenImages(1, &mat_component->id_image);
				ilBindImage(mat_component->id_image);

				ilEnable(IL_ORIGIN_SET);
				ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

				ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

				mat_component->texture[0] = ilutGLBindTexImage();

			}

		}*/
	}

	return ret;
}



GameObject* GOManager::CreateGo(const char* name, GameObject* parent) const
{
	GameObject* ret = new GameObject(parent, name);

	return ret;
}

bool GOManager::CleanUp()
{
	GameObject* it = root_GO;
	while (it != root_GO)
	{
		while (!it->children.empty())
		{
			it = (*it->children.begin());
		}

		GameObject* parent = it->GO_parent;

		std::list<GameObject*>::iterator i = parent->children.begin();
		while (i != parent->children.end())
		{
			(*i)->Clear();
			i++;
		}

		it = parent;
	}
	
	it->Clear();

	return true;
}
