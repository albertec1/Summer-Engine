#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"

#include <list>
#include <string>

class Module;
class ModuleWindow;
class ModuleInput;
//class ModuleAudio;
class ModuleSceneIntro;
//class ModuleRenderer3D;
class ModuleCamera3D;
class ModulePhysics3D;
//class ModuleMenu;
//class ModuleImporter;

class Application
{
public:
	ModuleWindow* window;
	//ModuleMenu* menu;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;
	//ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	//MeshImporter* imp;

private:

	Timer	ms_timer;
	Timer	fps_timer;
	float	dt;
	std::list<Module*> list_modules;
	bool exit;
	int	miliseconds;
	int contFPS;
	int	last_ms;
	int	last_fps;
	Uint32 frames;
	std::string title_name;
	std::string organization_name;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void OpenBrowser(const char* url) const;
	void Exit();
	uint GetFRLimit() const;
	void SetFRLimit(uint max_framerate);
	void Log(const char* text);
	const char* GetTitleName() const;
	void SetTitleName(const char* name);
	const char* GetOrganizationName() const;
	void SetOrganizationName(const char* name);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};
extern Application* App;

#endif //__APPLICATION_H__