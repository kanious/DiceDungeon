#include "Server.h"
#include "ClientInfo.h"
#include "Timer.h"
#include <conio.h>

#define FPS 120

using namespace std;
int main(int argc, char** argv)
{
	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_SHOW);

	Server server;
	printf("Server Opening . . . \n");
	int result = server.ServerOpen();
	if (result != 0)
		return result;
	printf("Server Opened.\n\n");


	Engine::CTimer* pTimer = Engine::CTimer::GetInstance();
	pTimer->SetFrameRate(FPS);

	while (true)
	{
		if (_kbhit())
		{
			int ch = _getch();

			if (ch == 27) // ESC
				break;
		}

		pTimer->Update();
		if (pTimer->IsUpdateAvailable())
		{
			_float dt = pTimer->GetTimeDelta();
			server.RecvFrom(dt);
			server.Update(dt);
			server.SendTo(dt);
		}
	}

	server.Close();
	SafeDestroy(pTimer);

	return 0;
}