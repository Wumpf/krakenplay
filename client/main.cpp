#include <iostream>

#include "client.h"
#include "inputsystem.h"
#include <OISException.h>

int main()
{
	std::cout << "Init input system ...\n\n";
	try
	{
		InputSystem inputSystem;

		std::cout << "\nInit network client ... ";
		Krakenplay::NetworkClient client;
		if(client.InitClient())
			std::cout << "done\n";
		else
		{
			std::cout << "failed\n";
			return 1;
		}

		// setup address
		client.SetServerAddress("127.0.0.1");

		for(;;) {}
	}
	catch(OIS::Exception exep)
	{
		std::cout << "OIS Exception:\n";
		std::cerr << exep.what() << std::endl;
		return 1;
	}
}