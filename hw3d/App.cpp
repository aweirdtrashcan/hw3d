#include "App.h"

#include "Pyramid.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "Suzanne.h"

#include "GDIPlusManager.h"
#include "Surface.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui.h"

#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Scene.h"

#pragma comment (lib,"Gdiplus.lib")

GDIPlusManager gdipm;

App::App()
	:
	wnd( 1920 - 100,1080 - 100,"The Donkey Fart Box" ),
	light(&wnd.Gfx(), camera)
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(45.f, (float)wnd.GetWidth() / (float)wnd.GetHeight(), 0.1f, 10000.f));

	model = new Scene(&wnd.Gfx(), "Models\\sponza.obj");
}

App::~App()
{
	delete model;
}

int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const std::optional<int> ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
		{
			return 0;
		}

		DoFrame();
	}
}

void App::DoFrame()
{
	const float s = sinf( timer.Peek() ) / 2.5f;
	const float c = cosf( timer.Peek() ) / 2.5f;
	wnd.Gfx().SetView(camera.GetMatrix());
	light.Bind(&wnd.Gfx());

	wnd.Gfx().BeginFrame( s, c, 1.0f );

	model->Draw(&wnd.Gfx());
	model->ShowWindow("Sponza");

	light.Draw(&wnd.Gfx());
	light.SpawnControlWindow();
	SpawnImguiWindow();
	SpawnModelWindow();
	camera.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}

void App::SpawnImguiWindow()
{
	if (ImGui::Begin("Simulation Speed"))
	{
		const float framerate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / framerate, framerate);
		ImGui::Checkbox("VSync", &wnd.Gfx().GetIsVsyncEnabled());
	}
	ImGui::End();
}

void App::SpawnModelWindow()
{

}
