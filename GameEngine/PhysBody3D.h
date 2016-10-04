#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include <list>
//#include "glmath.h"


class btRigidBody;
class Module;
class vec3;

// =================================================
enum sensor_type {
	COIN,
	FINISH_LINE,
	FALLEN_CAR,
	NOT_SENSOR

};

class PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	const vec3 GetPos() const;
	void SetAsSensor(bool is_sensor, sensor_type s_type);
	bool IsSensor() const;
	sensor_type GetSensorType() const;

private:
	btRigidBody* body = nullptr;
	bool is_sensor = false;
	sensor_type sens_type;

public:
	std::list<Module*> collision_listeners;
	bool can_change_colour = false;
};

#endif // __PhysBody3D_H__
