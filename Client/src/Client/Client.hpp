#pragma once
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <asio.hpp>

class chat_message
{
public:
    enum { header_length = 4 };
    enum { max_body_length = 512 };

    chat_message()
        : body_length_(0)
    {
    }

    const char* data() const
    {
        return data_;
    }

    char* data()
    {
        return data_;
    }

    std::size_t length() const
    {
        return header_length + body_length_;
    }

    const char* body() const
    {
        return data_ + header_length;
    }

    char* body()
    {
        return data_ + header_length;
    }

    std::size_t body_length() const
    {
        return body_length_;
    }

    void body_length(std::size_t new_length)
    {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }

    bool decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, data_, header_length);
        body_length_ = std::atoi(header);
        if (body_length_ > max_body_length)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode_header()
    {
        char header[header_length + 1] = "";
        std::sprintf(header, "%4d", static_cast<int>(body_length_));
        std::memcpy(data_, header, header_length);
    }

private:
    char data_[header_length + max_body_length];
    std::size_t body_length_;
};

class chat_client
{
public:
    chat_client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
        : _io_context(io_context), _socket(io_context)
    {
        connect(endpoints);
    }

    void write(const chat_message& msg)
    {
        asio::post(_io_context, [this, msg]() 
            {
                bool write_in_progress = !write_msgs.empty();
                write_msgs.push_back(msg);
                if (!write_in_progress)
                {
                    do_write();
                }
            });
    }
    void close()
    {
        asio::post(_io_context, [this]() {_socket.close(); });
    }
private:
    void connect(const asio::ip::tcp::resolver::results_type& endpoints)
    {
        asio::async_connect(_socket, endpoints,
            [this](std::error_code ec, asio::ip::tcp::endpoint) 
            {
                if(!ec)
                {
                    read_header();
                }
            });
    }
    void read_header()
    {
        asio::async_read(_socket, asio::buffer(read_msg.data(), chat_message::header_length),
            [this](std::error_code ec, std::size_t)
            {
                if (!ec && read_msg.decode_header())
                {
                    read_body();
                }
                else
                {
                    _socket.close();
                }
            });
    }
    void read_body()
    {
        asio::async_read(_socket, asio::buffer(read_msg.body(), read_msg.body_length()),
            [this](std::error_code ec, std::size_t) 
            {
                if (!ec)
                {
                    std::cout.write(read_msg.body(), read_msg.body_length());
                    std::cout << "\n";
                    read_header();
                }
                else
                {
                    _socket.close();
                }
            });
    }
    void do_write()
    {
        asio::async_write(_socket, asio::buffer(write_msgs.front().data(), write_msgs.front().length()),
            [this](std::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    write_msgs.pop_front();
                    if (!write_msgs.empty())
                    {
                        do_write();
                    }
                }
                else
                {
                    _socket.close();
                }
                
            });
    }
    asio::io_context& _io_context;
    asio::ip::tcp::socket _socket;
    chat_message read_msg;
    std::deque<chat_message> write_msgs;
};