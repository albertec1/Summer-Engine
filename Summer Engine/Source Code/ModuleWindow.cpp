#include "Application.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL.h"
#include "OpenGL.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if (WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (WIN_MAXIMIZED == true)
		{
			flags |= SDL_WINDOW_MAXIMIZED;
		}

		if (WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window_flags = (SDL_WindowFlags)(flags);
		window = SDL_CreateWindow(App->GetTitleName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			gl_context = SDL_GL_CreateContext(window);
		}
		
		glViewport(0, 0, width, height);

		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	}

	
	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	SDL_GL_DeleteContext(gl_context);

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

uint ModuleWindow::GetWidth() const
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	return width;
}

uint ModuleWindow::GetHeight() const
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	return height;
}

void ModuleWindow::SetWidth(uint width)
{
	SDL_SetWindowSize(window, width, GetHeight());
}

void ModuleWindow::SetHeight(uint height)
{
	SDL_SetWindowSize(window, GetWidth(), height);
}

bool ModuleWindow::IsFullscreen() const
{
	return isFullScreen;
}

void ModuleWindow::SetFullscreen(bool set)
{
	if (set != isFullScreen)
	{
		isFullScreen = set;
		if (isFullScreen == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
			{
				LOG("Could not switch to fullscreen: %s\n", SDL_GetError());
			}
				
			isFullScreenDesktop = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
			{
				LOG("Could not exit fullscreen: %s\n", SDL_GetError());
			}
				
		}
	}
}

bool ModuleWindow::IsFullscreenDesktop() const
{
	return isFullScreenDesktop;
}

void ModuleWindow::SetFullScreenDesktop(bool set)
{
	if (set != isFullScreenDesktop)
	{
		isFullScreenDesktop = set;
		if (isFullScreenDesktop == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
				LOG("Could not switch to fullscreen desktop: %s\n", SDL_GetError());
			isFullScreen = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
				LOG("Could not exit full screen desktop: %s\n", SDL_GetError());
		}
	}
}

bool ModuleWindow::IsResizable() const
{
	return isResizable;
}

void ModuleWindow::SetResizable(bool set)
{
	isResizable = set;
}

bool ModuleWindow::IsBorderless() const
{
	return isBorderless;
}

void ModuleWindow::SetBorderless(bool set)
{
	if (set != isBorderless && isFullScreen == false && isFullScreenDesktop == false)
	{
		isBorderless = set;
		SDL_SetWindowBordered(window, (SDL_bool)!isBorderless);
	}
}
