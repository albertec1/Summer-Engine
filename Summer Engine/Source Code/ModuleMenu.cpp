#include "Application.h"
#include "ModuleMenu.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"

#include "Window.h"
#include "WinConsole.h"
#include "WinConfiguration.h"
//#include "Win_Inspector.h"
//#include "Win_Hierarchy.h"

#include "Dependencies/ImGui/imgui.h"
#include "Dependencies/ImGui/imgui_internal.h"
#include "Dependencies/ImGui/imgui_impl_sdl.h"
#include "Dependencies/ImGui/imgui_impl_opengl3.h"

ModuleMenu::ModuleMenu(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	console = new WinConsole(true);
	configuration = new WinConfiguration((int)App->GetFRLimit(), false);
	/*inspector = new Win_Inspector(true);
	hierarchy = new Win_Hierarchy(true);*/

	AddWindow(console);
	AddWindow(configuration);
	/*AddWindow(inspector);
	AddWindow(hierarchy);*/
}

ModuleMenu::~ModuleMenu()
{
}

bool ModuleMenu::Start()
{
	LOG("Init Editor");

	//Set all the atributes and flags for our Gui window
	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 7.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Our state
	showDemoWindow = false;

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	return true;
}

bool ModuleMenu::CleanUp()
{
	std::vector<Window*>::iterator item = winArray.begin();
	for (item; item != winArray.end(); ++item)
		(*item)->CleanUp();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

update_status ModuleMenu::Update(float dt)
{
	if ((App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN) && (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN))
	{
		App->input->quit = true;
	}

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	usingKeyboard = io.WantCaptureKeyboard;
	usingMouse = io.WantCaptureMouse;

	if (configuration->changeFPSlimit)
	{
		App->SetFRLimit(configuration->max_fps);
	}

	//Top bar menu, with an option to close the editor
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Close", "Alt+F4"))
			{
				App->Exit();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				//Create Cube
			}
			if (ImGui::MenuItem("Sphere"))
			{
				//Create Sphere
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				//Create Cylinder
			}
			if (ImGui::MenuItem("Pyramid"))
			{
				//Create Pyramid
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Console", " ", console->active))
			{
				console->SetActive();
			}
			if (ImGui::MenuItem("Configuration", " ", configuration->active))
			{
				configuration->SetActive();
			}
			/*if (ImGui::MenuItem("Inspector", " ", inspector->active))
			{ 
				inspector->SetActive();
			}
			if (ImGui::MenuItem("Hierarchy", " ", hierarchy->active))
			{
				hierarchy->SetActive();
			}
			*/
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo")) {}

			ImGui::EndMenu();		
		}
		if (ImGui::BeginMenu("About"))
		{
			if (ImGui::MenuItem("Documentation"))
				App->OpenBrowser("https://github.com/albertec1/Summer-Engine/wiki");

			if (ImGui::MenuItem("Download latest"))
				App->OpenBrowser("https://github.com/albertec1/Summer-Engine/releases");

			if (ImGui::MenuItem("Report a bug"))
				App->OpenBrowser("https://github.com/albertec1/Summer-Engine/issues");
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
		ImGui::End();
	}

	//Window with a checkbox allowing to show the demo window of ImGui
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);
	{
		ImGui::Begin("DEMO");
		ImGui::Checkbox("Demo Window", &showDemoWindow);
		ImGui::End();
	}

	std::vector<Window*>::iterator item = winArray.begin();

	for (item; item != winArray.end(); ++item)
	{
		(*item)->Draw();
	}
			
	return UPDATE_CONTINUE;
}

void ModuleMenu::AddWindow(Window* window)
{
	winArray.push_back(window);
}

void ModuleMenu::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

void ModuleMenu::LogFPS(float fps, float ms)
{
	if (configuration != nullptr)
		configuration->AddLogFPS(fps, ms);
}

void ModuleMenu::Log(const char* text)
{
	if (console != nullptr)
		console->ConsoleLog(text);
}
