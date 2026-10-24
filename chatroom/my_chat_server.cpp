#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "my_message.h"
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>

typedef std::deque<my_message> message_queue;
using tcp=boost::asio::ip::tcp;

class participant
{
public:
    virtual ~participant() {};
    virtual void delever(const my_message &msg) = 0;
};

typedef std::shared_ptr<participant> participant_ptr;
class chatroom
{
    public:
    //群发
    void delever(const my_message &msg)
    {
        std::cout << "Broadcast called, participants: "
          << participants.size() << std::endl;
        recent_messages.push_back(msg);
        while(recent_messages.size() > 100)
        {
            recent_messages.pop_front();
        }
        for (auto participant : participants)
        {
            participant->delever(msg);
        }
    }
    void join(participant_ptr participant)
    {
        participants.insert(participant);
        for (auto msg : recent_messages)
        {
            participant->delever(msg);
        }
    }
    void leave(participant_ptr participant)
    {
        participants.erase(participant);
    }

    private:
    std::set<participant_ptr> participants;
    std::deque<my_message> recent_messages;
};

class session :public std::enable_shared_from_this<session>,
    public participant
{
public:
    session(boost::asio::ip::tcp::socket socket, chatroom &room):socket_(std::move(socket)),room(room)
    {
    }
    void start()
    {
        room.join(shared_from_this());
        read_header();
    }

    void delever(const my_message &msg)
    {
        bool is_writing=!write_msg_.empty();
        write_msg_.push_back(msg);
        if (!is_writing)
        {
            write_message();
        }
    }
    private:
    void read_header(){
        auto self(shared_from_this());
        boost::asio::async_read(socket_,boost::asio::buffer(read_msg_.data(),my_message::header_length),
        [this,self](boost::system::error_code ec,std::size_t length){
            if(!ec && read_msg_.decode_header()){
                read_body();
            }
            else{
                room.leave(shared_from_this());
            }
        });
    }
    void read_body()
    {
        auto self(shared_from_this());
        boost::asio::async_read(socket_,boost::asio::buffer(read_msg_.body(),read_msg_.body_length()),
        [this,self](boost::system::error_code ec,std::size_t length){
            if(!ec){
                // Process the message
                room.delever(read_msg_);
                read_header();
            }
            else{
                room.leave(shared_from_this());
            }
        });
    }
    void write_message()
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_,boost::asio::buffer(write_msg_.front().data(),write_msg_.front().length()),
            [this,self](boost::system::error_code ec,std::size_t length)
            {
                if(!ec)
                {
                    write_msg_.pop_front();
                    if(!write_msg_.empty()) write_message();
                }
                else
                {
                    room.leave(shared_from_this());
                }
            });
    }
    my_message read_msg_;
    boost::asio::ip::tcp::socket socket_;
    chatroom &room;
    std::deque<my_message> write_msg_;
};


class server
{
    using tcp = boost::asio::ip::tcp;
    public:
    server(boost::asio::io_context &io_context,const tcp::endpoint& endpoint):acceptor_(io_context, endpoint){
        do_accept();
    }
    private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<session>(std::move(socket), chat_room)->start();
                }
                do_accept();
            });
    }
    chatroom chat_room;
    boost::asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        boost::asio::io_context io_context;

        std::list<server> servers;
        for (int i = 1; i < argc; ++i)
        {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            servers.emplace_back(io_context, endpoint);
        }

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
