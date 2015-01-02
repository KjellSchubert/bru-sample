// copied partially from http://zguide.zeromq.org/cpp:rrserver

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_text_msg.h"
#include "foo/zmq_protobuf_msg.h"
#include <iostream>
#include <thread>
#include <chrono>

int main () {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");
    std::cout << "listening...\n";

    #if 0
    
    // here's the original string-based protocol (which could easily send json):
    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        std::string msgText = recv_text_msg(socket);
        std::cout << "Received message [" << msgText << ']' << std::endl;

        //  Do some 'work'
        if (msgText == "quit") {
            std::cout << "quitting server\n";
            send_text_msg(socket, "ack");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        //  Send reply back to client
        send_text_msg(socket, "World");
    }
    #else
    
    // here's the protobuf-based protocol for comparison:
    // (see also http://www.dotkam.com/2011/09/09/zeromq-and-google-protocol-buffers/)
    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        tutorial::MessageUnion readMsg = recv_protobuf_msg(socket);
        std::cout << "Received message [" << Message2HumanReadableString(readMsg) 
            << ']' << std::endl;

        //  Do some 'work'
        if (readMsg.msg_case() == tutorial::MessageUnion::kQuit) {
            std::cout << "quitting server\n";
            send_text_msg(socket, "ack");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        //  Send reply back to client
        tutorial::MessageUnion sentMsg;
        auto addressBook = sentMsg.mutable_addressbook();
        auto somePerson = addressBook->add_person();
        somePerson->set_name("santa");
        somePerson->set_id(2);
        send_protobuf_msg(socket, sentMsg); // send pointless empty address book
    }
    
    #endif
    
    return 0;
}
