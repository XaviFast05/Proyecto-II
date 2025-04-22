#pragma once

#include <string>
#include "pugixml.hpp"
#include "Log.h"


class GuiControl;

class Module
{
public:
	
	Module(bool startEnabled) : active(startEnabled) {}

	virtual ~Module() {}

	void Init()
	{

	}

	// Called before render is available
	virtual bool Awake()
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}
	
	//L05 TODO 4a: Declare a function to read the XML parameters	
	virtual bool LoadParameters(pugi::xml_node parameters)
	{
		configParameters = parameters;
		return true;
	}


	virtual bool OnGuiMouseClickEvent(GuiControl* control)
	{
		return true;
	}

	void SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node buttonParameters) 
	{
		
	}
		
	void Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		// TODO 0: Call CleanUp() for disabling a module
		if (active)
		{
			active = false;
			CleanUp();
			
		}
	}

	inline bool IsActive() const { return active; }

public:

	std::string name;
	bool active;
	//L05 TODO 4a: Declare a pugi::xml_node to store the module configuration parameters
	pugi::xml_node configParameters;

	

};