#include "App.h"

App::App()
	:
	wnd( 800,600,"The Donkey Fart Box" )
{}

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
	wnd.Gfx().ClearBuffer(s, c, 1.0f);
	wnd.Gfx().EndFrame();
}