#include "Globals.h"
#include "ModuleInput.h"
#include "Application.h"
#include "MeshImporter.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "MaterialImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "SDL.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput() : Module(), mouse({ 0, 0 }), mouse_motion({ 0,0 }) {
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

ModuleInput::~ModuleInput() {
	delete[](keyboard);
	keyboard = nullptr;
}

bool ModuleInput::Init() {
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

update_status ModuleInput::PreUpdate() {
	BROFILER_CATEGORY("InputPreUpdate()", Profiler::Color::Chocolate);
	static SDL_Event event;

	mouse_motion = { 0, 0 };
	mouse_wheel = 0;
	memset(windowEvents, false, WE_COUNT * sizeof(bool));

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i) {

		if (keys[i] == 1) {
			if (keyboard[i] == KEY_IDLE) { 
				keyboard[i] = KEY_DOWN;
			} else { 
				keyboard[i] = KEY_REPEAT;
			}
		} else {
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) {
				keyboard[i] = KEY_UP;
			} else {
				keyboard[i] = KEY_IDLE;
			}
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i) {
		if (mouse_buttons[i] == KEY_DOWN) {
			mouse_buttons[i] = KEY_REPEAT;
		}

		if (mouse_buttons[i] == KEY_UP) {
			mouse_buttons[i] = KEY_IDLE;
		}
	}

	while (SDL_PollEvent(&event) != 0) {

		App->editor->ProcessInputEvent(&event);
		switch (event.type) {

			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {

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

					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						App->window->WindowResized(event.window.data1, event.window.data2);
						break;
				}
				break;

			case SDL_DROPFILE:
			{
				char* fileDroppedPath = event.drop.file;
				FileDropped(fileDroppedPath);
				SDL_free(fileDroppedPath);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
				break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = (float)event.motion.xrel / (float)App->window->width;
				mouse_motion.y = (float)event.motion.yrel / (float)App->window->height;
				mouse.x = (float)event.motion.x;
				mouse.y = (float)event.motion.y;
				break;

			case SDL_MOUSEWHEEL:
				mouse_wheel = event.wheel.y;
				break;

		}
	}

	if (GetWindowEvent(EventWindow::WE_QUIT) == true || GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		return UPDATE_STOP;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp() {
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::FileDropped(const char* fileDroppedPath) {
	std::string fileName(fileDroppedPath);
	std::string extension(fileName.substr(fileName.length() - 3));

	std::size_t found = fileName.find("Models");
	fileName = fileName.substr(found, fileName.length());

	//TODO: we should be able to drop fbx and png from outside the project
	if (extension == "png" || extension == "tif") {
		App->fileSystem->ChangePathSlashes(fileName);
		MaterialImporter::Import(fileName.c_str());
	} else if (extension == "fbx" || extension == "FBX") {
		App->fileSystem->ChangePathSlashes(fileName);
		MeshImporter::ImportFBX(fileName.c_str());
	} else {
		LOG("Error: The file you are trying to drop is not accepted.");
	}
}

// ---------
bool ModuleInput::GetWindowEvent(EventWindow ev) const {
	return windowEvents[ev];
}

const fPoint& ModuleInput::GetMousePosition() const {
	return mouse;
}

const int ModuleInput::GetMouseWheel() const {
	return mouse_wheel;
}

const fPoint& ModuleInput::GetMouseMotion() const {
	return mouse_motion;
}

void ModuleInput::DrawGUI() {
	ImGui::Text("Mouse position:");
	ImGui::BulletText("X:%.2f | Y:%.2f", mouse.x * App->window->width, mouse.y * App->window->height);
}
