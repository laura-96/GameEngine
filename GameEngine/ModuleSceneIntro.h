#pragma once

#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
#include "Geometry.h"

#include <list>
//#include "PhysBody3D.h"

struct PhysBody3D;

class ModuleSceneIntro : public Module
{
public:

	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);

	bool Draw();
	bool CleanUp();

	//void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	list<Geometry*> geometries;
	InCube* b;
	//PhysBody3D* b;
};
