#include "Application.h"
#include "Window.h"
#include "Win_Hierarchy.h"
#include "ImGui.h"


Win_Hierarchy::Win_Hierarchy(bool _active) : Window(_active)
{

}

Win_Hierarchy::~Win_Hierarchy()
{

}

void Win_Hierarchy::Draw()
{
	if (!active)
		return;

	if (ImGui::Begin("Hierarchy", &active))
	{

		ImGui::End();
	}
}

void Win_Hierarchy::CleanUp()
{

}