// copied from http://zguide.zeromq.org/cpp:rrclient

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_text_msg.h"
#include "foo/zmq_protobuf_msg.h"
#include <iostream>
#include <chrono>

int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

#if 0
    
    // here's the original string-based protocol (which could easily send json):
    for( int request = 0 ; request < 10 ; request++) {
        
        using std::chrono::steady_clock;
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        
        auto t0 = steady_clock::now();
        std::cout << "sending request (start the server if it's not running yet)...\n";
        send_text_msg(socket, "Hello");
        std::cout << "sent request, waiting for reply...\n";
        std::string string = recv_text_msg(socket);
        
        std::cout << "Received reply " << request
            << " [" << string << "] after " 
            << duration_cast<milliseconds>(steady_clock::now() - t0).count()
            << " ms" << std::endl;
    }
    
    // optionally send a 'quit' message to auto-shutdown the server (doing
    // this only to be able to test server & client in travis-ci build)
    std::cout << "sending request 'quit'\n";
    send_text_msg(socket, "quit");
    // should we wait for the ACK here? I guess so
    recv_text_msg(socket);
    
#else
    
    // here's the protobuf-based protocol for comparison:
    // (see also http://www.dotkam.com/2011/09/09/zeromq-and-google-protocol-buffers/)
    for( int request = 0 ; request < 10 ; request++) {
        
        using std::chrono::steady_clock;
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        
        auto t0 = steady_clock::now();
        std::cout << "sending request (start the server if it's not running yet)...\n";
        {
            tutorial::MessageUnion sentMsg;
            {
                auto person = sentMsg.mutable_person();
                person->set_name("bob");
                person->set_id(678);
            }
            send_protobuf_msg(socket, sentMsg);
        }
        std::cout << "sent request, waiting for reply...\n";
        auto recvMsg = recv_protobuf_msg(socket);
        
        std::cout << "Received reply " << request
            << " [" << Message2HumanReadableString(recvMsg) << "] after " 
            << duration_cast<milliseconds>(steady_clock::now() - t0).count()
            << " ms" << std::endl;
    }
    
    // optionally send a 'quit' message to auto-shutdown the server (doing
    // this only to be able to test server & client in travis-ci build)
    std::cout << "sending request 'quit'\n";
    tutorial::MessageUnion quitMsg;
    quitMsg.mutable_quit();
    send_protobuf_msg(socket, quitMsg);
    // should we wait for the ACK here? I guess so
    recv_protobuf_msg(socket);
  
#endif
    
    return 0;
}