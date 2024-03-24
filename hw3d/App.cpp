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
	wnd( 1600,900,"The Donkey Fart Box" ),
	light(wnd.GfxPtr(), camera)
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(45.f, (float)wnd.GetWidth() / (float)wnd.GetHeight(), 0.1f, 10000.f));

	model = new Scene(wnd.GfxPtr(), "Models\\sponza.obj");
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
	if (wnd.kbd.KeyIsPressed('h') || wnd.kbd.KeyIsPressed('H'))
	{
		if (wnd.CursorEnabled())
		{
			wnd.DisableCursor();
		}
		else
		{
			wnd.EnableCursor();
		}
	}

	if (wnd.Gfx().GetIsPaused()) return;

	const float s = sinf( timer.Peek() ) / 2.5f;
	const float c = cosf( timer.Peek() ) / 2.5f;
	wnd.Gfx().SetView(camera.GetMatrix());
	light.Bind(wnd.GfxPtr());

	wnd.Gfx().BeginFrame( s, c, 1.0f );

	model->Draw(wnd.GfxPtr());

	light.Draw(wnd.GfxPtr());
	if (wnd.Gfx().IsImguiEnabled())
	{
		model->ShowWindow("Sponza");
		light.SpawnControlWindow();
		SpawnImguiWindow();
		SpawnModelWindow();
		camera.SpawnControlWindow();
		ShowRawDeltaWindow();
	}

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

void App::ShowRawDeltaWindow()
{
	while (std::optional<Mouse::RawDelta> d = wnd.mouse.GetRawDelta())
	{
		rawDelta.x += d.value().x;
		rawDelta.y += d.value().y;
	}
	if (ImGui::Begin("Raw Delta"))
	{
		ImGui::Text("%d, %d", rawDelta.x, rawDelta.y);
	}
	ImGui::End();
}
