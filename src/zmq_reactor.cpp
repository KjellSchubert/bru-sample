#include "foo/zmq_reactor.h"
#include <iostream>

ZmqReactorRequest::ZmqReactorRequest(ZmqRouterRequest&& request_, 
    zmq::context_t& context_,
    zmq::socket_t& tcpSocket_,
    const std::string& inprocSocketAddr_)
:
    request(request_),
    context(context_),
    tcpSocket(tcpSocket_),
    inprocSocketAddr(inprocSocketAddr_),
    tcpSocketThreadId(std::this_thread::get_id())
{
}

void ZmqReactorRequest::sendResponse(const zmq::message_t& response) const {
    if (std::this_thread::get_id() == tcpSocketThreadId) {
        // this branch is an optional optimization, we could instead always
        // go thru inprocSocket, paying with some extra msg copies.
        std::cout << "reactor sending message to tcpSocket directly\n";
        request.sendResponse(response, tcpSocket);
    }
    else {
        std::cout << "reactor worker thread sending message to inprocSocket\n";
        zmq::socket_t inprocSocket (context, ZMQ_PAIR); //xxx not PAIR!
        inprocSocket.connect(inprocSocketAddr.c_str());
        request.sendResponse(response, inprocSocket);
    }
}

ZmqReactor::ZmqReactor(
    zmq::context_t& context_,
    zmq::socket_t& tcpSocket_,
    const std::function<bool(const ZmqReactorRequest& req)>& onMessage_) 
    :
    context(context_),
    tcpSocket(tcpSocket_),
    onMessage(onMessage_)
{
}

void ZmqReactor::run() {

    // see section 'Multithreading with ZeroMQ' in the guide: use ZMQ_PAIR
    // for inter-thread communication, don't have shared state.
    // Worker threads will post their results to the inprocSocket, from which
    // the result will get forwarded back to the tcpSocket.
    zmq::socket_t inprocSocket (context, ZMQ_PAIR); //xxx tru sub/pub instead! pair can fail with multiple worker threads
    auto inprocSocketAddr = "inproc://reactor1"; // fixed name makes it effectively a singleton
    inprocSocket.bind(inprocSocketAddr);

    std::vector<zmq::pollitem_t> pollItems { // why cant I use std::array here?
        { tcpSocket, 0, ZMQ_POLLIN, 0 },
        { inprocSocket, 0, ZMQ_POLLIN, 0 }
    };
    
    while (true) {
        //  Wait for next request from client, or a response from a worker:
        zmq::poll (&pollItems[0], pollItems.size(), -1);
        if (pollItems[0].revents & ZMQ_POLLIN) {
            // process a TCP client request
            ZmqReactorRequest req { 
                ZmqRouterRequest(readMultiFrameMessage(tcpSocket)),
                context, tcpSocket, inprocSocketAddr
            };
            bool isQuitMessage = onMessage(req);
            if (isQuitMessage)
                break;
        }
        if (pollItems[1].revents & ZMQ_POLLIN) {
            // forward a response to from inprocSocket to tcpSocket
            std::cout << "forwarding msg from inprocSocket to tcpSocket\n";
            auto msg = readMultiFrameMessage(inprocSocket);
            writeMultiFrameMessage(tcpSocket, msg);
        }
    }
}
