#include "Engine.h"
#include "Input.h"
#include "Window.h"
#include "Log.h"
#include "tracy/Tracy.hpp"
#include "Module.h"

#define MAX_KEYS 300

Input::Input(bool startEnabled) : Module(startEnabled)
{
	name = "input";

	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouseButtons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);


}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Awake()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);



	if(SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else {
		if (SDL_NumJoysticks() > 0) {
			if (SDL_IsGameController(0)) {
				controller = SDL_GameControllerOpen(0);
			}
			else {
				LOG("Joystick 0 is not a game controller");
			}
		}
		else {
			LOG("No gamepads detected");
		}
	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	ZoneScoped;
	static SDL_Event event;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouseButtons[i] == KEY_DOWN)
			mouseButtons[i] = KEY_REPEAT;

		if(mouseButtons[i] == KEY_UP)
			mouseButtons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouseButtons[event.button.button - 1] = KEY_DOWN;
			break;

			case SDL_MOUSEBUTTONUP:
				mouseButtons[event.button.button - 1] = KEY_UP;
			break;

			case SDL_MOUSEMOTION:
				int scale = Engine::GetInstance().window.get()->GetScale();
				mouseMotionX = event.motion.xrel / scale;
				mouseMotionY = event.motion.yrel / scale;
				mouseX = event.motion.x / scale;
				mouseY = event.motion.y / scale;
			break;
		}
	}

	// Gamepad button states update
	if (controller != nullptr)
	{
		for (int i = 0; i < MAX_GAMEPAD_BUTTONS; ++i)
		{
			bool isPressed = SDL_GameControllerGetButton(controller, static_cast<SDL_GameControllerButton>(i));

			switch (gamepadButtons[i])
			{
			case KEY_IDLE:
				if (isPressed)
					gamepadButtons[i] = KEY_DOWN;
				break;
			case KEY_DOWN:
				gamepadButtons[i] = isPressed ? KEY_REPEAT : KEY_UP;
				break;
			case KEY_REPEAT:
				if (!isPressed)
					gamepadButtons[i] = KEY_UP;
				break;
			case KEY_UP:
				gamepadButtons[i] = isPressed ? KEY_DOWN : KEY_IDLE;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_GAMEPAD_BUTTONS; ++i)
			gamepadButtons[i] = KEY_IDLE;
	}

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	
	if (controller != nullptr) {
		SDL_GameControllerClose(controller);
		controller = nullptr;
	}
	return true;
}


bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[ev];
}

Vector2D Input::GetMousePosition()
{
	return Vector2D(mouseX, mouseY);
}

Vector2D Input::GetMouseMotion()
{
	return Vector2D(mouseMotionX, mouseMotionY);
}

KeyState Input::GetGamepadButton(SDL_GameControllerButton button) const
{
	return gamepadButtons[button];
}