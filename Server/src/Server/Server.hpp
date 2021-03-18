#pragma once
#include <asio.hpp>
#include <set>
#include <deque>


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
class chat_participant
{
public:
	virtual ~chat_participant() {}
	virtual void deliver(const chat_message& msg) = 0;
};


class chat_room
{

public:
    void join(std::shared_ptr<chat_participant> participant)
    {
        participants.insert(participant);
        for (auto msg : recent_messages) 
        {
            participant->deliver(msg);
        }
    }
    void leave(std::shared_ptr<chat_participant> participant)
    {
        participants.erase(participant);
    }
    void deliver(const chat_message& msg) 
    {
        recent_messages.push_back(msg);
        while (recent_messages.size() > max_recent_msgs)
        {
            recent_messages.pop_front();
        }
        for (auto participant : participants)
        {
            participant->deliver(msg);
        }
    }

private:
    std::set <std::shared_ptr<chat_participant>> participants;
    enum { max_recent_msgs = 100 };
    std::deque<chat_message> recent_messages;
};

class chat_session : public chat_participant, public std::enable_shared_from_this<chat_session>
{
public:
    chat_session(asio::ip::tcp::socket socket, chat_room& room) : _socket(std::move(socket)), _room(room)
    {

    }
    void start()
    {
        _room.join(shared_from_this());
        read_header();
    }
    void deliver(const chat_message& msg)
    {
        bool write_in_progress = !write_msgs.empty();
        write_msgs.push_back(msg);
        if (!write_in_progress)
        {
            write();
        }
    }

private:
    void read_header()
    {
        auto self(shared_from_this());
        asio::async_read(_socket, asio::buffer(read_msg.data(), chat_message::header_length),
            [this, self](std::error_code ec, std::size_t)
            {
                if (!ec && read_msg.decode_header())
                {
                    read_body();
                }
                else
                {
                    _room.leave(shared_from_this());
                }
            });
    }
    void read_body()
    {
        auto self(shared_from_this());
        asio::async_read(_socket, asio::buffer(read_msg.body(), read_msg.body_length()),
            [this, self](std::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    _room.deliver(read_msg);
                    read_header();
                }
                else
                {
                    _room.leave(shared_from_this());
                }
            });
    }
    void write()
    {
        auto self(shared_from_this());
        asio::async_write(_socket,
            asio::buffer(write_msgs.front().data(),
                write_msgs.front().length()),
            [this, self](std::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    write_msgs.pop_front();
                    if (!write_msgs.empty())
                    {
                        write();
                    }
                }
                else
                {
                    _room.leave(shared_from_this());
                }
            });
    }
    asio::ip::tcp::socket _socket;
    chat_room& _room;
    chat_message read_msg;
    std::deque<chat_message> write_msgs;
};

class chat_server
{
public:
    chat_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint) : _acceptor(io_context, endpoint)
    {
        accept();
    }
private:
    void accept()
    {
        _acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<chat_session>(std::move(socket), _room)->start();
                }
                accept();
            });
    }
    asio::ip::tcp::acceptor _acceptor;
    chat_room _room;
};