#include <crtdbg.h>
#include "Client.h"

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	Client* pClient = new Client();
	RESULT result = pClient->Ready();
	if (result != PK_NOERROR) return result;

	pClient->Loop();
	pClient->Destroy();

	return 0;
}