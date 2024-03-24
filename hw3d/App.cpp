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
	const float dt = timer.Mark();

	ProcessKeys(dt);

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
		camera.SpawnControlWindow();
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

void App::ProcessKeys(float dt)
{
	while (true)
	{
		const float dt = 1000.f / ImGui::GetIO().Framerate;
		Keyboard::Event e = wnd.kbd.ReadKey();
		if (!e.IsValid()) break;

		if (e.IsPress()) break;

		switch (e.GetCode())
		{
		case 'H':
		{
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
			}
			else
			{
				wnd.EnableCursor();
			}
		} break;
		case VK_F1:
		{
			showDemoWindow = !showDemoWindow;
			break;
		}
		}
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			camera.Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			camera.Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			camera.Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			camera.Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			camera.Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_CONTROL))
		{
			camera.Translate({ 0.0f, -dt, 0.0f });
		}

		while (std::optional<Mouse::RawDelta> d = wnd.mouse.GetRawDelta())
		{
			if (d.has_value())
			{
				camera.Rotate(d.value().x, d.value().y);
			}
		}
	}
}