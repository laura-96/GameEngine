#ifndef __Module_H__
#define __Module_H__

#include <string>
#include "cJSON.h"

class Application;
struct PhysBody3D;

class Module
{
private:
	bool enabled;
	std::string name;

public:
	Application* App;

	Module(Application* parent, const char* _name, bool start_enabled = true) : App(parent), name(_name)
	{}

	virtual ~Module()
	{}

	virtual bool Init(cJSON* node)
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}
	
	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}

	//Utility
	const char* GetName() const
	{
		return name.c_str();
	}
	
};

#endif // __Module_H__