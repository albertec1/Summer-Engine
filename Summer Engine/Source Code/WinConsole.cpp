#include "WinConsole.h"
#include "ImGui.h"
#include "Globals.h"

WinConsole::WinConsole(bool _active) : Window(_active)
{
	scrollToBottom = false;
}
	
WinConsole::~WinConsole()
{}

void WinConsole::Draw()
{
	if (!active)
		return;
	if (ImGui::Begin("Console", &active))
	{
		for (uint i = 0; i < buffer.size(); i++)
		{
			const char* item = buffer[i];
			ImGui::TextUnformatted(item);
		}

		if (scrollToBottom)
		{
			ImGui::SetScrollHere(1.0f);
			scrollToBottom = false;
		}
		
		ImGui::End();
	}
}

void WinConsole::CleanUp()
{
	for (int i = 0; i < buffer.size(); i++)
		free(buffer[i]);
	buffer.clear();
	scrollToBottom = true;
}

void WinConsole::ConsoleLog(const char* text)
{
	buffer.push_back(_strdup(text));
	scrollToBottom = true;
}