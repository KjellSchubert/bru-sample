// copied partially from http://zguide.zeromq.org/cpp:rrserver

#include "zmq.hpp" // from module cppzmq
#include "foo/zmq_util.h"
#include "foo/zmq_text_msg.h"
#include "foo/zmq_protobuf_msg.h"
#include "foo/zmq_reactor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <array>
#include <thread>

int main () {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t tcpSocket (context, ZMQ_ROUTER);
    {
        // ZMQ_ROUTER_MANDATORY is recommended to get an exception when
        // messing up the router message frame, without it the router will
        // drop flawed messages silently.        
        // not the most convenient wrapper...
        int value = 1;
        tcpSocket.setsockopt(ZMQ_ROUTER_MANDATORY, &value, sizeof(int)); 
        // optional (for debugging?)
        //std::string id = "my_router";
        //router.setsockopt(ZMQ_IDENTITY, &id[0], id.length());
    }
    tcpSocket.bind ("tcp://*:5555");
    std::cout << "listening...\n";

    auto onMessage = [](const ZmqReactorRequest& req) -> bool {
        const auto& payload = msg2protobuf(*req.getPayload());
        std::cout << "received message from tcpSocket: "
            << Message2HumanReadableString(payload) << '\n';

        if (payload.msg_case() == tutorial::MessageUnion::kQuit) {
            std::cout << "quitting server\n";
            // only if the client is a ZMQ_REQ an ack is necessary/recommended.
            req.sendResponse(string2msg("ack"));
            return true;
        }
        
        // creating one thread per request won't scale well, but this
        // is just an example. TODO: simulate asyncio with an external
        // REST API server here.
        auto worker = std::thread([req]() {
            // do some 'work', pretend this is a REST API call to some 
            // HTTP server here.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

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
            req.sendResponse(protobuf2msg(response));
        });
        worker.detach();
        
        return false;
    };

    ZmqReactor reactor(context, tcpSocket, onMessage);
    reactor.run(); // stops when it gets a quit message

    return 0;
}
