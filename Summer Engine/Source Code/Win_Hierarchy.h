#include "Application.h"
#include "Window.h"

class Window;

class Win_Hierarchy : public Window
{
public:

	Win_Hierarchy(bool _active);
	~Win_Hierarchy();

	void Draw() override;

	void CleanUp() override;

private:

};
