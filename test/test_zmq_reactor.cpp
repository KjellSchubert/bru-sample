#include <string>
#include <stdexcept>
#include "foo/zmq_text_msg.h"
#include "foo/zmq_reactor.h"
#include "gtest/gtest.h" // from module googletest

TEST(zmq_reactor_test, run_reactor_with_single_message) {
    zmq::context_t context (1);
    zmq::socket_t tcpSocket (context, ZMQ_ROUTER);
    const char* tcpSocketAddr = "inproc://reactor_test"; // not tcp during testing
    tcpSocket.bind(tcpSocketAddr); 
    auto onMessage = [](const ZmqReactorRequest& req) -> bool {
        if(msg2string(*req.getPayload()) != "foo")
            throw std::runtime_error("unexpected req msg");
        req.sendResponse(string2msg("bar"));
        return true; // quit
    };
    
    zmq::socket_t clientSocket (context, ZMQ_REQ);
    clientSocket.connect(tcpSocketAddr);
    {
        auto msg = string2msg("foo");
        clientSocket.send(msg);
    }
    
    ZmqReactor reactor(context, tcpSocket, onMessage);
    reactor.run(); // stops when it gets a quit message
    
    zmq::message_t response;
    clientSocket.recv(&response);
    EXPECT_EQ(msg2string(response), "bar");
}