#include "CameraComponent.h"


#include "Globals.h"
#include "Color.h"

#include "GameObject.h"

#include <Windows.h>
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>


void CameraComponent::SetPreferences(math::float3 position, float near_plane_dist, float far_plane_dist, float horizontal_fov, float aspect_ratio)
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = position;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = near_plane_dist;
	frustum.farPlaneDistance = far_plane_dist;
	frustum.horizontalFov = horizontal_fov;
	frustum.verticalFov = horizontal_fov / aspect_ratio;
}

float CameraComponent::GetNearPlaneDist() const
{
	return frustum.nearPlaneDistance;
}

float CameraComponent::GetFarPlaneDist() const
{
	return frustum.farPlaneDistance;
}

float CameraComponent::GetHorizontalFOV() const
{
	return frustum.horizontalFov;
}

float CameraComponent::GetAspectRatio() const
{
	return (frustum.horizontalFov/frustum.verticalFov);
}

math::float4x4 CameraComponent::GetView() const
{
	return frustum.ViewMatrix();
}

void CameraComponent::SetDirection(float3 new_dir)
{
	math::float4x4 mat = math::float4x4::LookAt(frustum.front, new_dir.Normalized(), frustum.up, float3::unitY);

	frustum.front = mat.TransformDir(frustum.front).Normalized();
	frustum.up = mat.TransformDir(frustum.up).Normalized();
}

void CameraComponent::Rotate(float3 euler)
{

	math::float3x3 mat = math::float3x3::FromRS(math::Quat::FromEulerXYZ(euler.x, euler.y, euler.z) , float3(1, 1, 1));
	frustum.Transform(mat);

}

void CameraComponent::DrawFrustum() const
{
	glLineWidth(2.0f);
	glColor3f(1.f, 0.f, 0.f);

	glBegin(GL_LINE_LOOP);

	glVertex3f(frustum.CornerPoint(0).x, frustum.CornerPoint(0).y, frustum.CornerPoint(0).z);
	glVertex3f(frustum.CornerPoint(1).x, frustum.CornerPoint(1).y, frustum.CornerPoint(1).z);
	glVertex3f(frustum.CornerPoint(3).x, frustum.CornerPoint(3).y, frustum.CornerPoint(3).z);
	glVertex3f(frustum.CornerPoint(2).x, frustum.CornerPoint(2).y, frustum.CornerPoint(2).z);

	glEnd();

	glBegin(GL_LINE_LOOP);

	glVertex3f(frustum.CornerPoint(4).x, frustum.CornerPoint(4).y, frustum.CornerPoint(4).z);
	glVertex3f(frustum.CornerPoint(5).x, frustum.CornerPoint(5).y, frustum.CornerPoint(5).z);
	glVertex3f(frustum.CornerPoint(7).x, frustum.CornerPoint(7).y, frustum.CornerPoint(7).z);
	glVertex3f(frustum.CornerPoint(6).x, frustum.CornerPoint(6).y, frustum.CornerPoint(6).z);

	glEnd();


	glBegin(GL_LINE_LOOP);

	glVertex3f(frustum.CornerPoint(0).x, frustum.CornerPoint(0).y, frustum.CornerPoint(0).z);
	glVertex3f(frustum.CornerPoint(1).x, frustum.CornerPoint(1).y, frustum.CornerPoint(1).z);
	glVertex3f(frustum.CornerPoint(5).x, frustum.CornerPoint(5).y, frustum.CornerPoint(5).z);
	glVertex3f(frustum.CornerPoint(4).x, frustum.CornerPoint(4).y, frustum.CornerPoint(4).z);

	glEnd();

	glBegin(GL_LINE_LOOP);

	glVertex3f(frustum.CornerPoint(2).x, frustum.CornerPoint(2).y, frustum.CornerPoint(2).z);
	glVertex3f(frustum.CornerPoint(3).x, frustum.CornerPoint(3).y, frustum.CornerPoint(3).z);
	glVertex3f(frustum.CornerPoint(7).x, frustum.CornerPoint(7).y, frustum.CornerPoint(7).z);
	glVertex3f(frustum.CornerPoint(6).x, frustum.CornerPoint(6).y, frustum.CornerPoint(6).z);

	glEnd();
}

void CameraComponent::ActivateCulling()
{
	if (!frustum_culling)
	{
		frustum_culling = true;
	}
}

void CameraComponent::DeactivateCulling()
{
	if (frustum_culling)
	{
		frustum_culling = false;
	}
}

void CameraComponent::FrustumCulling(std::vector<GameObject*> objects) const
{
	if (frustum_culling)
	{
		std::vector<GameObject*>::iterator it = objects.begin();
		while (it != objects.end())
		{
			//If it has got a mesh component, this one, won't render
			Component* mesh = (Component*)(*it)->FindComponent(ComponentType::Mesh);

			if (mesh != nullptr)
			{
				if (!IntersectsObject((*it)))
				{
					mesh->Enable(false);
				}

				else
				{
					mesh->Enable(true);
				}

			}
			it++;
		}
	}
}

bool CameraComponent::IntersectsObject(GameObject* obj) const
{
	
	math::Plane planes[6];
	frustum.GetPlanes(planes);

	
	math::float3 corners[8];
	obj->GetBoundingBoxCorners(corners);

	//Test each of the points to check they are all under at least one of the planes (if they are, and culling is active, they are rejected)
	if (corners)
	{
		for (uint plane = 0; plane < 6; plane++)
		{
			uint corner = 0;
			uint outside = 0;

			while (corner < 8)
			{
				if (planes[plane].IsOnPositiveSide(corners[corner]) == true)
				{
					outside++;
				}

				corner++;
			}

			if (outside == 8)
			{
				return false;
			}
		}
	}
	
	return true;
}

void CameraComponent::SetFrustumPos(const math::float3 pos)
{
	frustum.pos = pos;
}

void CameraComponent::Clear()
{
	delete this;
}