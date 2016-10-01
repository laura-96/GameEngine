#include "Mesh.h"
#include "Globals.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

Mesh::Mesh()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	model = new mesh();
}

Mesh::~Mesh()
{
	CleanUp();
}

bool Mesh::LoadMesh(const char* file)
{
	bool ret = true;
	const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* meshes = scene->mMeshes[i];
			
			model->vertex = meshes->mNumVertices;
			model->vertices = new float[model->vertex * 3];
	
			memcpy(model->vertices, meshes->mVertices, sizeof(float) * model->vertex * 3);

			LOG("New mesh with %d vertices", model->vertex);

			if (meshes->HasFaces())
			{
				model->index = meshes->mNumFaces * 3;
				model->indices = new uint[model->index];

				for (uint j = 0; j < meshes->mNumFaces; ++j)
				{
					if (meshes->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING! Geometry face with %d indices", meshes->mFaces[j].mNumIndices);
					}

					else
					{
						memcpy(&model->indices[j * 3], meshes->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

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

	else 
	{
		ret = false;
		LOG("Error loading scene: %s", file);
	}
	

	return ret;
}

void Mesh::Render() const
{
	
		glPushMatrix();
		glMultMatrixf(model->transform.M);

		glColor4f(model->color.r, model->color.g, model->color.b, 1);
		
		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, model->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->id_index);

		glDrawElements(GL_TRIANGLES, model->index, GL_UNSIGNED_INT, NULL);

		glPopMatrix();

		glDisableClientState(GL_VERTEX_ARRAY);
	
}

bool Mesh::CleanUp()
{
		aiDetachAllLogStreams();

		delete model->vertices;
		delete model->indices;
		delete model;

	return true;
}