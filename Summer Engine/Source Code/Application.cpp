#include "Application.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
//#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleMenu.h" 
//#include "MeshImporter.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	//audio = new ModuleAudio(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);
	menu = new ModuleMenu(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(menu);
	AddModule(camera);
	AddModule(input);

	//AddModule(audio);
	AddModule(physics);
	
	// Scenes
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);

	title_name = TITLE;
	contFPS = 0;
	frames = 0;
	miliseconds = 1000 / 60;
	last_fps = -1;
	last_ms = -1;
}

Application::~Application()
{
	for (std::list<Module*>::iterator e = list_modules.begin(); e != list_modules.end(); e++)
	{
		Module* module = *e;
		delete module;
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (std::list<Module*>::iterator e = list_modules.begin(); e != list_modules.end(); e++)
	{
		Module* module = *e;
		ret = module->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (std::list<Module*>::iterator e = list_modules.begin(); e != list_modules.end(); e++)
	{
		Module* module = *e;
		ret = module->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	++frames;
	++contFPS;

	if (fps_timer.Read() >= 1000)
	{
		last_fps = contFPS;
		contFPS = 0;
		fps_timer.Start();
	}

	last_ms = ms_timer.Read();

	if (miliseconds > 0 && (last_ms < miliseconds))
	{
		SDL_Delay(miliseconds - last_ms);
	}
	//menu->LogFPS((float)last_fps, (float)last_ms);
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (std::list<Module*>::iterator e = list_modules.begin(); e != list_modules.end(); e++)
	{
		Module* module = *e;
		ret = module->PreUpdate(dt);
	}

	for (std::list<Module*>::iterator e = list_modules.begin(); e != list_modules.end(); e++)
	{
		Module* module = *e;
		ret = module->Update(dt);
	}

	for (std::list<Module*>::iterator e = list_modules.begin(); e != list_modules.end(); e++)
	{
		Module* module = *e;
		ret = module->PostUpdate(dt);
	}

	FinishUpdate();

	if (exit)
	{
		ret = update_status::UPDATE_STOP;
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (std::list<Module*>::iterator e = list_modules.begin(); e != list_modules.end(); e++)
	{
		Module* module = *e;
		module->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::OpenBrowser(const char* url) const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void Application::Exit()
{
	exit = true;
}

uint Application::GetFRLimit() const
{
	if (miliseconds > 0)
		return (uint)((1.0f / (float)miliseconds) * 1000.0f);
	else
		return 0;
}

void Application::SetFRLimit(uint max_framerate)
{
	if (max_framerate > 0)
		miliseconds = 1000 / max_framerate;
	else
		miliseconds = 0;
}



void Application::Log(const char* text)
{
	//menu->Log(text);
}

const char* Application::GetTitleName() const
{
	return title_name.c_str();
}

void Application::SetTitleName(const char* name)
{
	if (name != nullptr && name != title_name)
	{
		title_name = name;
		window->SetTitle(name);
	}
}

const char* Application::GetOrganizationName() const
{
	return organization_name.c_str();
}

void Application::SetOrganizationName(const char* name)
{
	if (name != nullptr && name != organization_name)
	{
		organization_name = name;
	}
}