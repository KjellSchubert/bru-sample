// copied from http://zguide.zeromq.org/cpp:rrclient

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_string.h"
#include <iostream>
#include <chrono>

int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://localhost:5560");

    for( int request = 0 ; request < 10 ; request++) {
        
        using namespace std::chrono;
        // here I wanted to use steady_clock initially, but on travis-ci's
        // current stdlib version this is not supported, see 
        //   http://stackoverflow.com/questions/16518705/probleme-with-stdchrono
        // P.S.: let's ensure travis-ci installs stdlib >= 4.8 instead.
        using std::chrono::steady_clock;
        
        auto t0 = steady_clock::now();
        std::cout << "sending request (start the server if it's not running yet)...\n";
        s_send (requester, "Hello");
        std::cout << "sent request, waiting for reply...\n";
        std::string string = s_recv (requester);
        
        std::cout << "Received reply " << request
            << " [" << string << "] after " 
            << duration_cast<milliseconds>(steady_clock::now() - t0).count()
            << " ms" << std::endl;
    }
    
    return 0;
}