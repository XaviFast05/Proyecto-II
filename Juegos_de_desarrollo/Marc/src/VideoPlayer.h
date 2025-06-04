#pragma once

#include "Module.h"
#include "Animation.h"
#include "GuiControlButton.h"
//#include "Render.h"
#include "GuiControlSlider.h"

#include <list>
#include <map>


struct SDL_Texture;

class VideoPlayer : public Module
{
public:

	VideoPlayer(bool startEnabled);

	~VideoPlayer();

	bool Start() override;

	bool Update(float dt) override;

	bool CleanUp() override;


	bool PostUpdate() override;

	void SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node buttonParameters);

	void SetVideoNum(int videoNum);

	void SetVideoPlayed(bool videoPlayed);

	void SetModuleToGo(Module* module);
public:

	
	pugi::xml_document configFile;
	pugi::xml_node rootNode;
	int videoNum;
	bool videoPlayed;
	Module* moduleToGo;
};
