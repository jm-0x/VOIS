#pragma once
#include <asio.hpp>

using tcp = asio::ip::tcp;

class Server
{
public:
	Server(asio::io_context& context)
		:m_context(context),
		m_acceptor(context, tcp::endpoint(tcp::v4(), 1337))
	{

	}
private:
	asio::io_context m_context;
	tcp::acceptor m_acceptor;
	
};