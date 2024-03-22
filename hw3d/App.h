#pragma once
#include "ChiliTimer.h"
#include "Window.h"
#include <vector>
#include <set>
#include "Box.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"

class App
{
public:
	App();
	~App();
	// master frame / message loop
	int Go();
private:
	void DoFrame();
	void SpawnImguiWindow();
private:
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	Camera camera;
	PointLight light;
	class Model* model = nullptr;
};