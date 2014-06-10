#include "../krakenplay/krakenplay.h"
#include <iostream>

int main()
{
	std::cout << "Init server ... ";
	if(Krakenplay::Server::Instance().InitServer())
		std::cout << "done\n";
	else
	{
		std::cout << "failed\n";
		return 1;
	}


	for(;;) {}


	return 0;
}