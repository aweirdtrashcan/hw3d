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

#pragma comment (lib,"Gdiplus.lib")

GDIPlusManager gdipm;

App::App()
	:
	wnd( 1280,720,"The Donkey Fart Box" ),
	light(&wnd.Gfx(), camera)
{
	class Factory
	{
	public:
		Factory(Graphics* gfx): gfx(gfx) {}
		std::unique_ptr<Drawable> operator()() 
		{
			switch (typedist(rng))
			{
			case 0:
				DirectX::XMFLOAT3 color = { cdist(rng), cdist(rng), cdist(rng) };
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist, color
				);
			case 1:
				return std::make_unique<Suzanne>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
				);
			default:
				DebugBreak();
				return nullptr;
			}
		}
	private:
		Graphics* gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, 3.1415f * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, 3.1415f * 2.0f };
		std::uniform_real_distribution<float> odist{ 0.0f, 3.1415f * 0.3f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> latdist{ 5, 20 };
		std::uniform_int_distribution<int> longdist{ 10, 40 };
		std::uniform_int_distribution<int> typedist{ 0, 1 };
	};

	Factory f( &wnd.Gfx() );
	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ), nDrawables, f );

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(45.f, (float)wnd.GetWidth() / (float)wnd.GetHeight(), 0.1f, 1000.f));

	const Surface s = Surface::FromFile( "Images\\kappa50.png" );

	buffer = (char*)malloc( 1000 );
	memset( buffer, 0, 1000 );

	int id = 0;
	for (const std::unique_ptr<Drawable>& d : drawables)
	{
		if (Box* box = dynamic_cast<Box*>(d.get()))
		{
			boxes.push_back(box);
			box->SetId(id++);
		}
	}
}

App::~App()
{
	if ( buffer ) {
		free( buffer );
		buffer = nullptr;
	}
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

	float deltaTime = timer.Mark() * speedFactor;

	for ( size_t i = 0; i < drawables.size(); i++ )
	{
		std::unique_ptr<Drawable>& b = drawables[i];
		b->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : deltaTime );
		b->Draw( &wnd.Gfx() );
	}

	light.Draw(&wnd.Gfx());
	light.SpawnControlWindow();
	SpawnImguiWindow();
	SpawnBoxWindowControl();
	camera.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}

void App::SpawnImguiWindow()
{
	if (ImGui::Begin("Simulation Speed"))
	{
		const float framerate = ImGui::GetIO().Framerate;
		ImGui::SliderFloat("Speed Factor", &speedFactor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / framerate, framerate);
		ImGui::InputText("Butts", buffer, 1000);
		ImGui::Checkbox("VSync", &wnd.Gfx().GetIsVsyncEnabled());
	}
	ImGui::End();
}

void App::SpawnBoxWindowControl()
{
	if (ImGui::Begin("Boxes"))
	{
		char previewStr[50];

		if (selectedBoxIndex != -1)
		{
			snprintf(previewStr, sizeof(previewStr), "Box ID: %d", selectedBoxIndex);
		}
		else
		{
			snprintf(previewStr, sizeof(previewStr), "Select a box...");
		}

		if (ImGui::BeginCombo("Box Control", previewStr))
		{
			for (const Box* box : boxes)
			{
				bool selected = box->GetId() == selectedBoxIndex;
				char selectableStr[50];
				snprintf(selectableStr, sizeof(selectableStr), "Box ID: %d", box->GetId());
				if (ImGui::Selectable(selectableStr, selected))
				{
					selectedBoxIndex = box->GetId();
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Spawn Control Window"))
		{
			if (selectedBoxIndex != -1)
			{
				boxIds.insert(selectedBoxIndex);
			}
		}
	}
	ImGui::End();

	for (std::set<int>::iterator boxId = boxIds.begin(); boxId != boxIds.end();)
	{
		if (!boxes[*boxId]->SpawnControlWindow(&wnd.Gfx()))
		{
			boxId = boxIds.erase(boxId);
		}
		else
		{
			boxId++;
		}
	}
}
