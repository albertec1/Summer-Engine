
#include "Application.h"
#include "Window.h"

class Window;

class Win_Inspector : public Window
{
public:

	Win_Inspector(bool _active);
	~Win_Inspector();

	void Draw() override;

	void CleanUp() override;

private:

};
