#include <iostream>
#include <asio.hpp>
int main()
{
	asio::error_code ec;
	asio::io_context context;

	std::string address;
	std::cout << "Enter the endpoint addres: ";
	std::cin >> address;
	asio::ip::tcp::endpoint ep(asio::ip::make_address(address), 80);
	asio::ip::tcp::socket socket(context);
	std::cout << "attempting to connect to endpoint...\n";
	socket.connect(ep, ec);
	if (!ec)
	{
		std::cout << "Success!\n";
	}
	else
	{
		std::cerr << "Failed to connect to endpoint:\n" << ec.message() << "\n";
	}

	return 0;
}
