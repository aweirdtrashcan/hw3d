#pragma once
#include "ChiliTimer.h"
#include "Window.h"
#include <vector>
#include "Box.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>

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
	__forceinline void SpawnBoxWindowControl();
private:
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	static constexpr unsigned int nDrawables = 30;
	float speedFactor = 1.0f;
	char* buffer = nullptr;
	Camera camera;
	PointLight light;
	std::set<int> boxIds;
	std::vector<Box*> boxes;
	int selectedBoxIndex = -1;
};