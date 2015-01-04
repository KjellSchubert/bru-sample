#ifndef FOO_ZMQ_UTIL_H
#define FOO_ZMQ_UTIL_H

#include "zmq.hpp" // from module cppzmq
#include <vector>
#include <memory>

// zmq often deals with multi-frame messages (e.g. ZMQ_ROUTER), even for
// client applications using only use single-frame (string or protobuf) payloads.
class ZmqMultiFrameMsg {
    public:
        // using a ptr because message_t is not copyable 
        std::vector<std::shared_ptr<zmq::message_t>> frames;
};

// reads all parts of a multipart message
ZmqMultiFrameMsg readMultiFrameMessage(zmq::socket_t& socket);
void writeMultiFrameMessage(zmq::socket_t& socket, const ZmqMultiFrameMsg& msg);

// it took me the longest time to figure out that when reading from a ZMQ_ROUTER
// socket each message you get from a ZMQ_REQ client will consist of >= 3 parts,
// even though the ZMQ_REQ client code only sent a single message part: the
// ZMQ_ROUTER will get these message parts:
//   1. a msg part representing the ephemeral port of the client who sent the 
//      message.
//   2. an empty 0-len msg part
//   3. the >=1 message parts sent by the ZMQ_REQ code
// To have the router send a message back to this particular client just prefix
// your response by copies of msg parts 1+2. This means the router may respond
// to clients in any order it wants (or not at all).
class ZmqRouterMsg {
    public:
        ZmqRouterMsg(const ZmqMultiFrameMsg& msg);
        
        // get the 3rd part of the message, containing the frame sent by
        // the ZMQ_REQ client.
        std::shared_ptr<zmq::message_t> getPayload() const {
            return payload;
        }
        
        // sends a response to the original sender of the message
        void sendResponse(zmq::socket_t& socket, const zmq::message_t& response) const;
        
    private:
        std::shared_ptr<zmq::message_t> sender;
        std::shared_ptr<zmq::message_t> empty;
        std::shared_ptr<zmq::message_t> payload;
};

#endif