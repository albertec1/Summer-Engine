#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

class Window;
class WinConsole;
class WinConfiguration;
//class WinInspector;
//class WinHierarchy;

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
	inline bool UsingKeyboard() const { return usingKeyboard; }

	void Render();
public:
	bool showDemoWindow;
	bool usingKeyboard;
	bool usingMouse;

	std::vector<Window*> winArray;
	WinConsole* console = nullptr;
	WinConfiguration* configuration = nullptr;

	//WinInspector* inspector = nullptr;
	//WinHierarchy* hierarchy = nullptr;
	//
};

#endif //__ModuleEditor_H__
