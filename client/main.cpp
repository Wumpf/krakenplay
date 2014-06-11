#include <iostream>

#include "client.h"
#include "inputfetcher.h"
#include <OISException.h>

int main()
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

		for (;;)
		{
			inputFetcher.Update(client);
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
		std::cerr << "Unkown exception occured" << std::endl;
		return 1;
	}
}