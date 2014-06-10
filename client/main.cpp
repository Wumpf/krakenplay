#include <iostream>
#include "client.h"

int main()
{
	std::cout << "Init client ... ";
	if(Krakenplay::Client::Instance().InitClient())
		std::cout << "done\n";
	else
	{
		std::cout << "failed\n";
		return 1;
	}


	for(;;) {}


}