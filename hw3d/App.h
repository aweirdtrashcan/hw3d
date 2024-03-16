#pragma once
#include "ChiliTimer.h"
#include "Window.h"
#include <vector>
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
	__forceinline void SpawnImguiWindow();
private:
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	static constexpr unsigned int nDrawables = 100;
	float speedFactor = 1.0f;
	char* buffer = nullptr;
	Camera camera;
	PointLight light;
};