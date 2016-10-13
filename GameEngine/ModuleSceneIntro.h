#ifndef __ModuleSceneIntro_H__
#define __ModuleSceneIntro_H__

#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
#include "Geometry.h"
#include "Mesh.h"

#include <list>
//#include "PhysBody3D.h"

struct PhysBody3D;

class ModuleSceneIntro : public Module
{
public:

	ModuleSceneIntro(Application* app, const char* name, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Init(cJSON* node);
	bool Start();
	update_status Update(float dt);

	bool Draw();
	bool CleanUp();

	//void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	
	//Mesh* m;
	//list<Geometry*> geometries;
	InCube* b;
	//PhysBody3D* b;
};

#endif // __ModuleSceneIntro_H__