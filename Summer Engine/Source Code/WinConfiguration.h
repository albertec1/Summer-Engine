
#include "Window.h"
#include "ImGui.h"
#include <vector>
#include "Globals.h"

#define LOG_LENGTH 50

class WinConfiguration : public Window
{
public:
	const uint default_w = 325;
	const uint default_h = 417;
	const uint default_x = 956;
	const uint default_y = 609;
public:

	WinConfiguration(int _max_fps, bool _active);
	virtual ~WinConfiguration();

	void Draw() override;
	void CleanUp() override;

	void AddLogFPS(float fps, float ms);

	bool changeFPSlimit = false;
	bool changeTitleName = false;
	int max_fps = 140;

private:
	ImGuiTextBuffer input_buf;
	bool need_scroll = false;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
};