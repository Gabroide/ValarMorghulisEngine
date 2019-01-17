#ifndef __Application_h__
#define __Application_h__

#include <list>

#include "Globals.h"
#include "Module.h"

#include "Brofiler\include\Brofiler.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleEditor;
class ModuleCamera;
class ModuleInput;
class ModuleScene;
class ModuleProgram;
class ModuleTime;
class ModuleScene;
class ModuleDebugDraw;
class ModuleFileSystem;
class ModuleLibrary;

class Application
{
	public:
		Application();
		~Application();

		bool Init();
		bool CleanUp();

		update_status Update();

		void FinishUpdate();
		void OpenWebBrowser(const char* web);

	public:
		ModuleRender* renderer = nullptr;
		ModuleWindow* window = nullptr;
		ModuleInput* input = nullptr;
		ModuleTextures* textures = nullptr;
		ModuleEditor* editor = nullptr;
		ModuleCamera* camera = nullptr;
		ModuleProgram* program = nullptr;
		ModuleTime* time = nullptr;
		ModuleScene* scene = nullptr;
		ModuleDebugDraw* debug = nullptr;
		ModuleFileSystem* fileSystem = nullptr;
		ModuleLibrary* library = nullptr;

	private:
		std::list<Module*>	modules;

};

extern Application* App;

#endif // __Application_h__