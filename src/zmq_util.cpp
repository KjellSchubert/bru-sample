#include "foo/zmq_util.h"
#include <algorithm>
#include <stdexcept>
#include <string>

// reads all parts of a multipart message
ZmqMultiFrameMsg readMultiFrameMessage(zmq::socket_t& socket) {
    ZmqMultiFrameMsg result;
    while (true) {
        auto frame = std::make_shared<zmq::message_t>();
        socket.recv(frame.get());
        result.frames.push_back(frame);
        if (!frame->more())
            break;
    }
    return result;
}

void writeMultiFrameMessage(zmq::socket_t& socket, const ZmqMultiFrameMsg& msg) {
    const auto& frames = msg.frames;
    assert(frames.size() > 0);
    std::for_each(frames.begin(), frames.end()-1, 
        [&socket](const std::shared_ptr<zmq::message_t>& frame) {
            socket.send(*frame, ZMQ_SNDMORE);
        });
    socket.send(*frames.back());
}

ZmqRouterRequest::ZmqRouterRequest(const ZmqMultiFrameMsg& msg) {
    if (msg.frames.size() < 3)
        throw std::runtime_error("ZMQ_ROUTER unexpected multipart msg length: " 
            + std::to_string(msg.frames.size()));
    if (msg.frames[1]->size() > 0)
        throw std::runtime_error("ZMQ_ROUTER should have empty 2nd frame");
    sender = msg.frames[0];
    empty = msg.frames[1];
    payload = msg.frames[2];
}

void ZmqRouterRequest::sendResponse(
    const zmq::message_t& response, 
    zmq::socket_t& socket) const 
{
    socket.send(*sender.get(), ZMQ_SNDMORE);
    socket.send(*empty.get(), ZMQ_SNDMORE);
    socket.send(const_cast<zmq::message_t&>(response));
}
