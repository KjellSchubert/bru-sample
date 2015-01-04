#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_text_msg.h"
#include "foo/zmq_protobuf_msg.h"
#include "foo/zmq_util.h"
#include <iostream>
#include <chrono>

int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    zmq::socket_t tcpSocket(context, ZMQ_REQ); // could be ZMQ_REQ or ZMQ_DEALER
    tcpSocket.connect("tcp://localhost:5555");

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
            auto msg = protobuf2msg(sentMsg);
            tcpSocket.send(msg);
        }
        std::cout << "sent request, waiting for reply...\n";
        auto recvMsg = readMultiFrameMessage(tcpSocket);
        assert(recvMsg.frames.size() == 1);
        auto payload = msg2protobuf(*recvMsg.frames.front());
        
        std::cout << "Received reply " << request
            << " [" << Message2HumanReadableString(payload) << "] after " 
            << duration_cast<milliseconds>(steady_clock::now() - t0).count()
            << " ms" << std::endl;
    }
    
    // optionally send a 'quit' message to auto-shutdown the server (doing
    // this only to be able to test server & client in travis-ci build)
    std::cout << "sending request 'quit'\n";
    {
        tutorial::MessageUnion quitMsg;
        quitMsg.mutable_quit();
        auto msg = protobuf2msg(quitMsg);
        tcpSocket.send(msg);
    }
    // should we wait for the ACK here? I guess so
    zmq::message_t ack;
    tcpSocket.recv(&ack);
  
    return 0;
}