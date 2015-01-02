// copied partially from http://zguide.zeromq.org/cpp:rrserver

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_text_msg.h"
#include <iostream>
#include <thread>
#include <chrono>

int main () {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");
    std::cout << "listening...\n";

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
    return 0;
}
