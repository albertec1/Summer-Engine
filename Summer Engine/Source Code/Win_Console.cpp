#include "Win_Console.h"
#include "ImGui.h"
#include "Globals.h"

Win_Console::Win_Console(bool _active) : Window(_active)
{
	scrollToBottom = false;
}
	
Win_Console::~Win_Console()
{}

void Win_Console::Draw()
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
		}

		scrollToBottom = false;

		ImGui::End();
	}
	

}

void Win_Console::CleanUp()
{
	for (int i = 0; i < buffer.size(); i++)
		free(buffer[i]);
	buffer.clear();
	scrollToBottom = true;
}

void Win_Console::ConsoleLog(const char* text)
{
	buffer.push_back(_strdup(text));
	scrollToBottom = true;
}