#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"

ModuleCamera3D::ModuleCamera3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	camera = new CameraComponent(nullptr);
	position = math::float3(0, 20, 20);

	LookAt(math::float3(0, 0, 0));

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
	camera->Clear();
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	math::float3 newPos(0, 0, 0);
	float speed = 10.0f * dt;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 20.0f * dt;

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += (math::float3::unitZ * speed);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= (math::float3::unitZ * speed);


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos += (math::float3::unitX * speed);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos -= (math::float3::unitX * speed);

	position += newPos;

	// Mouse motion ----------------

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			camera->Rotate(math::float3(math::DegToRad(DeltaX), math::DegToRad(DeltaX), math::DegToRad(DeltaX)));

		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			camera->Rotate(math::float3(math::DegToRad(DeltaY), math::DegToRad(DeltaY), math::DegToRad(DeltaY)));

		}

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
