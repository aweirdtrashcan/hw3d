#include "App.h"

App::App()
	:
	wnd( 800,600,"The Donkey Fart Box" )
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			&wnd.Gfx(), rng, adist,
			ddist, odist, rdist
		));
	}
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

	for (auto& b : boxes)
	{
		b->Update(timer.Peek());
		b->Draw(&wnd.Gfx());
	}

	wnd.Gfx().EndFrame();
}