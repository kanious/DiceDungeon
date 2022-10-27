#include "Client.h"
#include <crtdbg.h>

int main(int argc, char* argv)
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtSetBreakAlloc(34277);

	system("mode con: cols=80 lines=25");

	Client* pClient = Client::Create();
	if (nullptr == pClient)
		return -1;

	pClient->Loop();
	pClient->Destroy();

	return 0;
}