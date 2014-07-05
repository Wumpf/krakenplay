#include <iostream>
#include <memory>
#include <csignal>

#include "networkclient.h"
#include "inputfetcher.h"

#pragma warning(push, 0)
#include <OISException.h>
#pragma warning(pop)

std::unique_ptr<Krakenplay::InputFetcher> inputFetcher;
std::unique_ptr<Krakenplay::NetworkClient> client;

static void shutdown(int signum)
{
	std::cout << "Got signal #" << signum << ", sending disconnect messages.\n";

	if (inputFetcher && client)
		inputFetcher->Disconnect(*client);

	_exit(1);
}

#if defined(_WIN32)
#include "Windows.h"

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch (CEvent)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		if (inputFetcher && client)
		{
			std::cout << "Application closed, sending disconnect messages.\n";
			inputFetcher->Disconnect(*client);
			
			inputFetcher.release();
			client.release();
		}
		break;

	}
	return TRUE;
}
#endif


int main(int argc, char** argv)
{
	try
	{
		std::cout << "Init input fetching system ...";
		inputFetcher.reset(new Krakenplay::InputFetcher);
		std::cout << "\n";

		std::cout << "Init client ... ";
		client.reset(new Krakenplay::NetworkClient);

		if (client->InitClient())
			std::cout << "done\n";
		else
		{
			std::cout << "failed\n";
			return 1;
		}

		if (argc > 1)
			client->SetServerAddress(argv[1]);
		else
		{
			std::cout << "Waiting for server identify-message ... ";
			if (client->WaitForServerIdentifyMessage())
				std::cout << "done\n";
			else
			{
				std::cout << "failed\n";
				return 1;
			}
		}
		std::cout << "Using server address " << client->GetServerAdress() << "\n\n";

		// Install signal handlers for exiting the client
		signal(SIGINT, shutdown);
		signal(SIGTERM, shutdown);
		signal(SIGSEGV, shutdown);
#if defined(_WIN32) && defined(_CONSOLE)
		if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
		{
			std::cerr << "Unable to install console handler!\n";
		}
#endif

		for (;;)
		{
			inputFetcher->Update(*client);
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