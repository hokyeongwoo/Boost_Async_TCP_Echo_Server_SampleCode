
#include <iostream>
#include "TCP_Server.h"

using namespace::std;

int main()
{
	try
	{
		{
			boost::asio::io_service io;
			CTCP_Server server(io);
			io.run();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}