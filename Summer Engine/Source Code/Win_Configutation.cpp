
#include "Win_Configuration.h"
#include "OpenGl.h"
#include "ImGui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Color.h"
#include "Globals.h"
#include "Dependencies/mmgr/mmgr.h"
#include <vector>

using namespace std;

// ---------------------------------------------------------
Win_Configuration::Win_Configuration(int _max_fps, bool _active) : Window(_active),
fps_log(LOG_LENGTH), ms_log(LOG_LENGTH)
{}

Win_Configuration::~Win_Configuration()
{}

void Win_Configuration::CleanUp()
{
}


void Win_Configuration::AddLogFPS(float fps, float ms)
{
	static uint count = 0;

	if (count == LOG_LENGTH)
	{
		for (uint i = 0; i < LOG_LENGTH - 1; ++i)
		{
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}
	}
	else
		++count;

	fps_log[count - 1] = fps;
	ms_log[count - 1] = ms;
}

void Win_Configuration::Draw()
{
	if (!active)
		return;

	if (ImGui::Begin("Configuration", &active))
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			char appName[100];
			strcpy_s(appName, 100, App->GetTitleName());

			if (ImGui::InputText("Project Name", appName, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				App->SetTitleName(appName);
			}

			if (ImGui::SliderInt("Max FPS", &max_fps, 0, 140))
			{
				changeFPSlimit = true;
			}


			ImGui::Text("Limit Framerate:");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%i", max_fps);

			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));



			sMStats stats = m_getMemoryStatistics();
			static int speed = 0;
			static vector<float> memory(100);
			if (++speed > 20)
			{
				speed = 0;
				if (memory.size() == 100)
				{
					for (uint i = 0; i < 100 - 1; ++i)
						memory[i] = memory[i + 1];

					memory[100 - 1] = (float)stats.totalReportedMemory;
				}
				else
					memory.push_back((float)stats.totalReportedMemory);
			}

			ImGui::PlotHistogram("##memory", &memory[0], memory.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

			ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
			ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
			ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
			ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
			ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
			ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
			ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
			ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
			ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);



		}
		if (ImGui::CollapsingHeader("Render"))
		{
			/*bool enabled = App->renderer3D->depthEnabled;
			if (ImGui::Checkbox("Depth Buffer", &enabled))
			{
				App->renderer3D->SetDepthBufferEnabled(enabled);
			}*/
			if (ImGui::Checkbox("Depth Buffer", &App->renderer3D->depthEnabled))
			{ 
				App->renderer3D->SetDepthBufferEnabled();
			}
			if (ImGui::Checkbox("Wireframe Mode", &App->renderer3D->wireframeMode)) {}
	
			bool vsync = App->renderer3D->GetVSync();
			if (ImGui::Checkbox("Vertical Sync", &vsync))
			{
					changeFPSlimit = true;
					App->renderer3D->SetVSync(vsync);
			}
			
		}
		if (ImGui::CollapsingHeader("Input"))
		{
			ImGui::Text("Mouse position: %i, %i", App->input->GetMouseX(), App->input->GetMouseY());
			ImGui::Text("Mouse motion: %i, %i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
			ImGui::Text("Mouse wheel: %i", App->input->GetMouseZ());
		}
		if (ImGui::CollapsingHeader("Window"))
		{
			int w = App->window->GetWidth();
			int h = App->window->GetHeight();
			if (ImGui::SliderInt("Width", &w, 0, 1920)) { App->window->SetWidth(w); }
			if (ImGui::SliderInt("Height", &h, 0, 1080)) { App->window->SetHeight(h); }

			bool fullscreen = App->window->IsFullscreen();
			bool resizable = App->window->IsResizable();
			bool borderless = App->window->IsBorderless();
			bool full_desktop = App->window->IsFullscreenDesktop();

			if (ImGui::Checkbox("Fullscreen", &fullscreen))
			{
				//App->window->SetFullscreen(fullscreen);			//Doesn't work properly
			}
				
			ImGui::SameLine();
			if (ImGui::Checkbox("Fullscreen Desktop", &full_desktop))
			{
				App->window->SetFullScreenDesktop(full_desktop);
			}
				
			if (ImGui::Checkbox("Resizable ", &resizable))
			{
				App->window->SetResizable(resizable);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Must restart to apply");
			}
				
			ImGui::SameLine();
			if (ImGui::Checkbox("Borderless", &borderless))
			{
				App->window->SetBorderless(borderless);
			}
				
		}
		if (ImGui::CollapsingHeader("Software"))
		{
			ImGui::Text("ImGui Version:");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%s", IMGUI_VERSION);

			SDL_version sdl_version;
			SDL_GetVersion(&sdl_version);
			ImGui::Text("SDL Version:");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);

			ImGui::Text("OpenGL Version:");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%s", glGetString(GL_VERSION));

			ImGui::Text("DevIL Version:");
			ImGui::SameLine();
			ImGui::TextColored(YELLOW, "%s", IMGUI_VERSION);		//change ImGui for Devil
		}


		ImGui::End();
	}
}