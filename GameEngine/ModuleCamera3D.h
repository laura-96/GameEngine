#ifndef __ModuleCamera3D_H__
#define __ModuleCamera3D_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "CameraComponent.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, const char* name, bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(cJSON* node);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void LookAt(const math::float3 &position);
	void Move(const math::float3 &Movement);
	float* GetViewMatrix();

public:

	math::float3 position;

private:
	GameObject* cam_object = nullptr;
	CameraComponent* camera = nullptr;
	TransformComponent* camera_transform = nullptr;
};

#endif // __ModuleCamera3D_H__