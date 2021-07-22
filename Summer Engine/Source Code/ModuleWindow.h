#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "Dependencies/SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);
	uint GetWidth() const;
	uint GetHeight() const;
	void SetWidth(uint width);
	void SetHeight(uint height);

	bool IsFullscreen() const;
	void SetFullscreen(bool set);
	bool IsFullscreenDesktop() const;
	void SetFullScreenDesktop(bool set);
	bool IsResizable() const;
	void SetResizable(bool set);
	bool IsBorderless() const;
	void SetBorderless(bool set);

public:
	//The window we'll be rendering to
	SDL_Window* window;
	SDL_WindowFlags window_flags;
	SDL_GLContext gl_context;

	//The surface contained by the window
	SDL_Surface* screen_surface;
	bool isFullScreen = false;
	bool isFullScreenDesktop = false;
	bool isResizable = true;
	bool isBorderless = false;
};

#endif // __ModuleWindow_H__