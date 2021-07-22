#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__

#include "Module.h"
#include "Globals.h"

class Window;
class Win_Inspector;
class Win_Configuration;
class Win_Hierarchy;
class Win_Console;

class ModuleMenu : public Module
{
public:
	ModuleMenu(Application* app, bool start_enabled = true);
	~ModuleMenu();

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void AddWindow(Window* window);
	void LogFPS(float fps, float ms);
	void Log(const char* text);
	inline bool UsingKeyboard() const { return using_keyboard; }

	void Render();
public:
	bool show_demo_window;
	bool using_keyboard;
	bool using_mouse;

	std::vector<Window*> winArray;
	Win_Inspector* inspector = nullptr;
	Win_Configuration* configuration = nullptr;
	Win_Hierarchy* hierarchy = nullptr;
	Win_Console* console = nullptr;
	
};

#endif //__ModuleEditor_H__
