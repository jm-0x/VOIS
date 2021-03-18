#include <spdlog/spdlog.h>
#include "Server/Server.hpp"
#include <iostream>


int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: chat_server <port>" << '\n';
			return 1;
		}
		asio::io_context io_context;
		std::list<chat_server> servers;
		for (int i = 1; i < argc; i++)
		{
			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), std::atoi(argv[i]));
			servers.emplace_back(io_context, endpoint);
		}
		io_context.run();
	}
	catch (std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << '\n';
	}
	return 0;
}