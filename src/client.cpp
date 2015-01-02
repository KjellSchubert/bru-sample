// copied from http://zguide.zeromq.org/cpp:rrclient

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_text_msg.h"
#include <iostream>
#include <chrono>

int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

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
    
    return 0;
}