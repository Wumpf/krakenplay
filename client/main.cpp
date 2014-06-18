#include <iostream>

#include "networkclient.h"
#include "inputfetcher.h"
#include <OISException.h>

int main(int argc, char** argv)
{
	try
	{
		std::cout << "Init input fetching system ...";
		Krakenplay::InputFetcher inputFetcher;

		std::cout << "Init client ... ";
		Krakenplay::NetworkClient client;

		if (client.InitClient())
			std::cout << "done\n";
		else
		{
			std::cout << "failed\n";
			return 1;
		}

		if(argc > 1)
			client.SetServerAddress(argv[1]);

		for (;;)
		{
			inputFetcher.Update(client);
			Sleep(200);
		}
	}
	catch(OIS::Exception exep)
	{
		std::cerr << "OIS exception:\n";
		std::cerr << exep.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown exception occurred" << std::endl;
		return 1;
	}
}