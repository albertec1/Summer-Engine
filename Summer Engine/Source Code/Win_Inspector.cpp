#include "Application.h"
#include "Window.h"
#include "Win_Inspector.h"
#include "ImGui.h"


Win_Inspector::Win_Inspector(bool _active) : Window(_active)
{

}

Win_Inspector::~Win_Inspector()
{

}

void Win_Inspector::Draw()
{
	if (!active)
		return;

	if (ImGui::Begin("Inspector", &active))
	{

		ImGui::End();
	}
}

void Win_Inspector::CleanUp()
{

}