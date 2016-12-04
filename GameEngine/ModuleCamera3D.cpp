#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

ModuleCamera3D::ModuleCamera3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	cam_object = new GameObject(nullptr, "Camera", 0);

	camera = new CameraComponent(cam_object);
	camera_transform = new TransformComponent(cam_object);

	camera->SetPreferences(position, 20, 1000, math::pi/4, SCREEN_WIDTH / SCREEN_HEIGHT);

	//As it is the editor, camera may not have culling
	camera->DeactivateCulling();

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::Init(cJSON* node)
{
	
	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");
	cam_object->Clear();
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	math::float3 newPos(0, 0, 0);
	float speed = 10.0f * dt;

	math::float3 right_normal = camera->frustum.RightPlane().normal;
	math::float3::Orthonormalize(camera->frustum.front, right_normal);

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 20.0f * dt;

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += (camera->frustum.front * speed);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= (camera->frustum.front * speed);

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= (right_normal * speed);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += (right_normal * speed);

	position += newPos;

	// Mouse motion ----------------

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -(App->input->GetMouseXMotion());
		int dy = -(App->input->GetMouseYMotion());


		math::Ray ray = camera->frustum.UnProject(float2(dx/2, dy/2));
		camera->frustum.front = ray.dir.Normalized();

		math::float3::Orthonormalize(camera->frustum.front, camera->frustum.up);

	}

	camera->SetFrustumPos(position);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const math::float3 &pos)
{
	float3 dir = pos - position;

	camera->SetDirection(dir);
}


void ModuleCamera3D::Move(const math::float3 &Movement)
{
	position += Movement;
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	math::float4x4 view_mat = camera->GetView().Transposed();

	return view_mat.ptr();
}
