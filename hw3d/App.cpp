#include "App.h"

#include "Pyramid.h"
#include "Melon.h"
#include "Sheet.h"
#include "SkinnedBox.h"

#include "GDIPlusManager.h"
#include "Surface.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui.h"

#include <memory>

#pragma comment (lib,"Gdiplus.lib")

GDIPlusManager gdipm;

App::App()
	:
	wnd( 1280,720,"The Donkey Fart Box" ),
	light(&wnd.Gfx())
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
				return std::make_unique<Box>(
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
		std::uniform_int_distribution<int> latdist{ 5, 20 };
		std::uniform_int_distribution<int> longdist{ 10, 40 };
		std::uniform_int_distribution<int> typedist{ 0, 0 };
	};

	Factory f( &wnd.Gfx() );
	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ), nDrawables, f );

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(45.f, (float)wnd.GetWidth() / (float)wnd.GetHeight(), 0.1f, 1000.f));

	const Surface s = Surface::FromFile( "Images\\kappa50.png" );

	buffer = (char*)malloc( 1000 );
	memset( buffer, 0, 1000 );
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

	for ( auto& b : drawables )
	{
		b->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : deltaTime );
		b->Draw( &wnd.Gfx() );
	}

	light.Draw(&wnd.Gfx());
	light.SpawnControlWindow();
	SpawnImguiWindow();
	camera.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}

void App::SpawnImguiWindow()
{
	if (ImGui::Begin("Simulation Speed"))
	{
		const float framerate = ImGui::GetIO().Framerate;
		ImGui::SliderFloat("Speed Factor", &speedFactor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", framerate / 1000.f, framerate);
		ImGui::InputText("Butts", buffer, 1000);
	}
	ImGui::End();
}
