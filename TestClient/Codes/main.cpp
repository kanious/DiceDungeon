#include "Client.h"


int main(int argc, char* argv)
{
	Client* pClient = Client::Create();
	if (nullptr == pClient)
		return -1;

	pClient->Loop();
	pClient->Destroy();

	return 0;
}