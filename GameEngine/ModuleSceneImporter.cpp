#include "Application.h"
#include "Globals.h"

#include "ModuleSceneImporter.h"

#include "ModuleFileSystem.h"

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

ModuleSceneImporter::ModuleSceneImporter(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

ModuleSceneImporter::~ModuleSceneImporter()
{}

bool ModuleSceneImporter::Init(cJSON* node)
{
	ilInit();
	ilutInit();

	ilutRenderer(ILUT_OPENGL);

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleSceneImporter::Start()
{
	
	return  true;
}

bool ModuleSceneImporter::ImportScene(const char* file)
{
	return true;
}