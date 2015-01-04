// copied partially from http://zguide.zeromq.org/cpp:rrserver

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_util.h"
#include "foo/zmq_text_msg.h"
#include "foo/zmq_protobuf_msg.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <array>
#include <thread>

// TODO
class ZmqReactor {
    public:
        void setMessageHandler(const std::function<void(const ZmqRouterMsg& msg)>& handler);
        void start();
        void stop();
};

int main () {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t tcpSocket (context, ZMQ_ROUTER);
    tcpSocket.bind ("tcp://*:5555");
    std::cout << "listening...\n";

    // see section 'Multithreading with ZeroMQ' in the guide: use ZMQ_PAIR
    // for inter-thread communication, don't have shared state.
    // Worker threads will post their results to the inprocSocket, from which
    // the result will get forwarded back to the tcpSocket.
    zmq::socket_t inprocSocket (context, ZMQ_PAIR);
    auto inprocSocketAddr = "inproc://pair1";
    inprocSocket.bind(inprocSocketAddr);

    std::vector<zmq::pollitem_t> pollItems { // why cant I use std::array here?
        { tcpSocket, 0, ZMQ_POLLIN, 0 },
        { inprocSocket, 0, ZMQ_POLLIN, 0 }
    };
    
    while (true) {
        //  Wait for next request from client, or a response from a worker:
        zmq::poll (&pollItems[0], pollItems.size(), -1);
        if (pollItems[0].revents & ZMQ_POLLIN) {
            ZmqRouterMsg msg { readMultiFrameMessage(tcpSocket) };

            const auto& payload = msg2protobuf(*msg.getPayload());
            std::cout << "received message from tcpSocket: " 
                << Message2HumanReadableString(payload) << '\n';
            if (payload.msg_case() == tutorial::MessageUnion::kQuit) {
                std::cout << "quitting server\n";
                msg.sendResponse(tcpSocket, string2msg("ack"));
                break;
            }
            
            // creating one thread per request won't scale well, but this
            // is just an example. TODO: simulate asyncio with an external
            // REST API server here.
            auto worker = std::thread([&context, msg, inprocSocketAddr]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
                tutorial::MessageUnion response;
                auto addressBook = response.mutable_addressbook();
                auto somePerson = addressBook->add_person();
                somePerson->set_name("santa");
                somePerson->set_id(2);
                
                // Send reply back to client
                // Which for ZMQ_ROUTER again requires 3 frames/parts.
                // Note that sockets must not be shared between threads, so
                // here we connect to an inproc socket and send the result to
                // that.
                std::cout << "worker sending message to inprocSocket\n";
                zmq::socket_t inprocSocket (context, ZMQ_PAIR);
                inprocSocket.connect(inprocSocketAddr);
                msg.sendResponse(inprocSocket, protobuf2msg(response));
            });
            worker.detach();
        }
        if (pollItems[1].revents & ZMQ_POLLIN) {
            // forward a response to from inprocSocket to tcpSocket
            std::cout << "forwarding msg from inprocSocket to tcpSocket\n";
            auto msg = readMultiFrameMessage(inprocSocket);
            writeMultiFrameMessage(tcpSocket, msg);
        }
    }

    return 0;
}
