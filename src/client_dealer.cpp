#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_text_msg.h"
#include "foo/zmq_protobuf_msg.h"
#include "foo/zmq_util.h"
#include <iostream>
#include <chrono>

void sendMessage(zmq::socket_t& socket, const tutorial::MessageUnion& payload) {
    // the server is a ZMQ_ROUTER: when the server gets messages from a ZMQ_REQ
    // then there will be an empty delimiter frame in the envelope. This the
    // server is supposed to talk to both ZMQ_REQ and ZMQ_DEALER clients we
    // have to send the same funky empty delimiter frame in the envelope here:
    zmq::message_t emptyDelimiter;
    socket.send(emptyDelimiter, ZMQ_SNDMORE);
    auto msg = protobuf2msg(payload);
    socket.send(msg);
}

int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    // TODO: the point of ZMQ_DEALER is that it can receive messages any time,
    // not just in response to a previously sent message. This impl here doesn't
    // allow for that atm.
    zmq::socket_t tcpSocket(context, ZMQ_DEALER); // could be ZMQ_REQ or ZMQ_DEALER
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
            sendMessage(tcpSocket, sentMsg);
        }
        std::cout << "sent request, waiting for reply...\n";
        auto recvMsg = readMultiFrameMessage(tcpSocket);
        assert(recvMsg.frames.size() == 2); // note ZMQ_REQ saw a single frame only
        auto payload = msg2protobuf(*recvMsg.frames.back());
        
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
        sendMessage(tcpSocket, quitMsg);
    }
    // should we wait for the ACK here? Not necessary I think (only necessary
    // for ZMQ_REQ?)

    return 0;
}