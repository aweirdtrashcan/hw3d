#include "App.h"
#include "Pyramid.h"
#include <memory>

App::App()
	:
	wnd( 800,600,"The Donkey Fart Box" )
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
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
				);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
				);
			default:
				assert(false && "wtf");
				break;
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
		std::uniform_int_distribution<int> typedist{ 0, 1 };
	};

	Factory f(&wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(45.f, (float)wnd.GetWidth() / (float)wnd.GetHeight(), 0.1f, 1000.f));
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
		DoFrame();
	}
}

void App::DoFrame()
{
	const float s = sinf(timer.Peek()) / 2.5f;
	const float c = cosf(timer.Peek()) / 2.5f;
	
	wnd.Gfx().BeginFrame();
	wnd.Gfx().ClearBuffer(s, c, 1.0f);

	static float lastTime = 0.0f;

	float deltaTime = timer.Mark();

	for (auto& b : drawables)
	{
		b->Update(deltaTime);
		b->Draw(&wnd.Gfx());
	}

	wnd.Gfx().EndFrame();
}