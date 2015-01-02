// copied partially from http://zguide.zeromq.org/cpp:rrserver

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_string.h"
#include <iostream>
#include <thread>
#include <chrono>

int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    zmq::socket_t responder(context, ZMQ_REP);
    responder.connect("tcp://localhost:5560");
    std::cout << "listening...\n";

    while(1)
    {
        //  Wait for next request from client
        std::string string = s_recv (responder);
        
        std::cout << "Received request: " << string << std::endl;
        
        // Do some 'work'
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        //  Send reply back to client
        s_send (responder, "World");
    }
    
    return 0;
}