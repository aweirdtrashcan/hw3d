#pragma once
#include "ChiliTimer.h"
#include "Window.h"
#include <vector>
#include "Box.h"

class App
{
public:
	App();
	// master frame / message loop
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	unsigned int nDrawables = 100;
};