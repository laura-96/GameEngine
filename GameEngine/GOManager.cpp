#include "Application.h"
#include "Globals.h"

#include "GOManager.h"


#include "Component.h"
#include "GameObject.h"
#include "MeshComponent.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

GOManager::GOManager(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

GOManager::~GOManager()
{}

bool GOManager::Init(cJSON* node)
{
	GOs_node = node;
	while (node->valuestring == "FBXs")
	{
		FBXs.push_back(node->child->valuestring);
		node = node->next;
	}

	return true;
}

bool GOManager::LoadFBXObjects(const char* FBX)
{
	bool ret = true;
	const aiScene* scene = aiImportFile(FBX, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->mRootNode != nullptr && scene->HasMeshes())
	{
		aiNode* rec_node = scene->mRootNode;


		GameObject* new_GO = CreateGo(rec_node->mName.C_Str(), nullptr);
		new_GO->id_tree.push_back(0);

		root_GO = new_GO;

		if (rec_node->mNumMeshes > 0)
		{
			for (uint i = 0; i < scene->mNumMeshes; i++)
			{
				aiMesh* meshes = scene->mMeshes[i];

				MeshComponent* mesh = new_GO->CreateMeshComponent();
				
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
			}
		}
		std::list<aiNode*> closed_nodes;
		GameObject* last_parent = nullptr;

		while (rec_node != nullptr)
		{

			//Close node if it's got no children
			if (rec_node->mNumChildren == 0)
			{
				closed_nodes.push_back(rec_node);
			}

			else
			{
				//Check if children are closed
				int i = 0;
				bool closed_children = false;

				for (std::list<aiNode*>::iterator it = closed_nodes.begin(); it != closed_nodes.end() && !closed_children; it++)
				{
					if ((*it) == rec_node->mChildren[i])
					{
						i++;
						it = closed_nodes.begin();

						if (i >= rec_node->mNumChildren)
						{
							closed_children = true;
						}
					}
				}
				
				//If they are, close node
				if (closed_children)
				{
					closed_nodes.push_back(rec_node);
				}

				//If not, node is children node
				else
				{
					rec_node = rec_node->mChildren[i];

					last_parent = new_GO;
					new_GO = CreateGo(rec_node->mName.C_Str(), last_parent);
					
					new_GO->id_tree = last_parent->id_tree;
					new_GO->id_tree.push_back(i);

				}
			}

			//Check if node is closed
			for (std::list<aiNode*>::iterator it = closed_nodes.begin(); it != closed_nodes.end(); it++)
			{
				if ((*it) == rec_node)
				{			
					rec_node = rec_node->mParent;
					new_GO = FindGoParent(new_GO);
					it = closed_nodes.begin();
				}
			}
		}	
	}
	else
	{
		ret = false;
		LOG("Error loading scene: %s", FBX);
	}
}



GameObject* GOManager::CreateGo(const char* name, GameObject* parent) const
{
	GameObject* ret = new GameObject(parent, name);

	return ret;
}

GameObject* GOManager::FindGoParent(GameObject* child) const
{
	GameObject* ret = root_GO;
	
	uint size = child->id_tree.size() - 1;

	for (uint i = 1; i < size; i++)
	{
		ret = ret->GetChildAt(child->id_tree[i]);
	}
	
	return ret;
}

/*

const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
if (scene != nullptr && scene->HasMeshes())
{


}
//models.push_back(model);
}

aiReleaseImport(scene);

glGenBuffers(1, (GLuint*) &(model->id_index));
glGenBuffers(1, (GLuint*) &(model->id_vertex));

glBindBuffer(GL_ARRAY_BUFFER, model->id_vertex);
glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->vertex * 3, model->vertices, GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->id_index);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * model->index, model->indices, GL_STATIC_DRAW);

}


}


*/
