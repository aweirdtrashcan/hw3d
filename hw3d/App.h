#pragma once
#include "ChiliTimer.h"
#include "Window.h"
#include <vector>
#include <set>
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Log.h"
#include "Mouse.h"
#include "Scene.h"

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
	void ProcessKeys(float dt);
private:
	Log log;
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	Camera camera;
	PointLight light;
	Scene* model;
	bool showDemoWindow = false;
	void* square = nullptr;
};