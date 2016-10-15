#include "Application.h"
#include "Globals.h"

#include "GOManager.h"

#include "Component.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"

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
		//LoadFBXObjects("Game/Assets/Brute.fbx");
	}

	return true;
}

update_status GOManager::Update(float dt)
{
	

	return UPDATE_CONTINUE;
}

void GOManager::DrawGo() const
{
	if (load_fbx)
	{
		root_GO->Update();
	}
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
	//Loading game object's components from scene and node

	bool ret = false;

	if (go != nullptr && node != nullptr)
	{
		ret = true;

		int material = -1;

		if (node->mNumMeshes > 0)
		{
			//While node has got meshes, create a mesh component with its values stored
			MeshComponent* mesh = nullptr;

			for (uint i = 0; i < node->mNumMeshes; i++)
			{
				mesh = go->CreateMeshComponent();
				LOG("Mesh component from: %s", mesh->GetGO()->name.c_str());

				uint index_scene = node->mMeshes[i];

				aiMesh* meshes = scene->mMeshes[index_scene];
				
				//Setting mesh vertices
				mesh->num_vertex = meshes->mNumVertices;
				mesh->vertices = new float[mesh->num_vertex * 3];

				memcpy(mesh->vertices, meshes->mVertices, sizeof(float) * mesh->num_vertex * 3);

				LOG("New mesh with %d vertices", mesh->num_vertex);

				if (meshes->HasFaces())
				{
					//Setting mesh indices
					mesh->num_index = meshes->mNumFaces * 3;
					mesh->indices = new uint[mesh->num_index];

					for (uint j = 0; j < meshes->mNumFaces; ++j)
					{
						if (meshes->mFaces[j].mNumIndices != 3)
						{
							LOG("WARNING! Geometry face with %d indices", meshes->mFaces[j].mNumIndices);
						}				
						
						//Store the three indices of each face to mesh's array of indices
						memcpy(&mesh->indices[j * 3], meshes->mFaces[j].mIndices, sizeof(uint) * 3);
						
					}
				}

				if (meshes->HasTextureCoords(0))
				{
					mesh->uvs = new float[mesh->num_vertex * 2];
					mesh->num_uvs = mesh->num_vertex;
					
					//Got an acces violation if used:
					//aiVector3D* buff = &meshes->mTextureCoords[0][mesh->indices[j]];
					//Copying to mesh->uvs the pair of coords of each vertex

					//Method finally used: getting all textureCoords in an aiVector3D
					aiVector3D* buff = meshes->mTextureCoords[0];

					//Copying x and y coordinates to its suitable uvs
					for (uint i = 0; i < mesh->num_vertex * 2; i = i + 2)
					{
						
						mesh->uvs[i] = buff->x;
						mesh->uvs[i + 1] = buff->y;
						
						buff++;
						//I was getting error trying to do it through memcpy
						//memcpy(&mesh->uvs[j * 2], buff, sizeof(float) * 2);
					}
				}
				
				mesh->index_material = meshes->mMaterialIndex;
				
				material = mesh->index_material;
			}

			glGenBuffers(1, (GLuint*) &(mesh->id_index));
			glGenBuffers(1, (GLuint*) &(mesh->id_vertex));
			glGenBuffers(1, (GLuint*) &(mesh->id_uvs));

			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, mesh->vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_index, mesh->indices, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uvs);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 2, mesh->uvs, GL_STATIC_DRAW);

		}
		
		if (scene->mNumMaterials > 0 && node->mNumMeshes > 0)
		{
			aiMaterial* mat = scene->mMaterials[material];
			
			assert(mat);

			if (mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0)
			{
				aiString path;
				mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
				//TODO: Load texture path in FileSystem
				MaterialComponent* mat_component = go->CreateMaterialComponent();
				mat_component->path.clear();
				mat_component->path.append("Game/Assets/Textures/");
				
				//So as to get the name of the texture, not relative to Fbx
				//Will append to mat_component->path, the last part of the texture path, its name
				char* buffer = new char[path.length + 1];
				strcpy(buffer, path.C_Str());

				for (uint i = 0; i < path.length + 1; i++)
				{
					if (buffer[i] == '\\')
					{
						for (uint j = 0; j <= i; j++)
						{
							buffer[j] = '*';
						}
					}
				}
				buffer[path.length] = '\0';

				while (buffer[0] == '*')
				{
					for (uint i = 0; i < path.length; i++)
					{
						buffer[i] = buffer[i + 1];
					}
				}
				
				mat_component->path.append(buffer);

				delete[] buffer;
		
				mat_component->material_id = material;

				ilGenImages(1, &mat_component->id_image);
				ilBindImage(mat_component->id_image);

				ilEnable(IL_ORIGIN_SET);
				ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

				if (!ilLoadImage(mat_component->path.c_str()))
				{
					ilDeleteImages(1, &mat_component->id_image);
					return false;
				}

				ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

				mat_component->texture[0] = ilutGLBindTexImage();
			}

		}
		aiMatrix4x4 local_trans = node->mTransformation;

		aiVector3D scale;
		aiQuaterniont<float> rotation;
		aiVector3D position;

		local_trans.Decompose(scale, rotation, position);

		TransformComponent* transform = go->CreateTransformComponent();

		transform->SetScale(scale.x, scale.y, scale.z);
		transform->SetRotation(rotation.x, rotation.y, rotation.z, rotation.w);
		transform->SetTranslation(position.x, position.y, position.z);

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
