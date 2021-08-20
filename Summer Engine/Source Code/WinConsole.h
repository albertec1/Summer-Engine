
#include "Window.h"
#include <vector>

class WinConsole : public Window
{
public:
	WinConsole(bool _active);
	virtual ~WinConsole();

	void Draw() override;

	void CleanUp() override;
	void ConsoleLog(const char* text);

private:

	std::vector<char*>	buffer;
	bool scrollToBottom;
};