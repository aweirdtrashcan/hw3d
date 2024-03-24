#pragma once
#include "ChiliTimer.h"
#include "Window.h"
#include <vector>
#include <set>
#include "Box.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Log.h"
#include "Mouse.h"

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
	void SpawnModelWindow();
	void ShowRawDeltaWindow();
private:
	Log log;
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	Camera camera;
	PointLight light;
	class Scene* model = nullptr;
	Mouse::RawDelta rawDelta{};
};