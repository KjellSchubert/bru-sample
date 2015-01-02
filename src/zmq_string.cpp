// some helpers for sending strings as zmq messages

#include "foo/zmq_string.h"
#include <string>

std::string msg2string(const zmq::message_t& msg) {
    auto strBegin = reinterpret_cast<const char*>(msg.data());
    auto strEnd = strBegin + msg.size();
    return std::string(strBegin, strEnd);
}

zmq::message_t string2msg(const std::string& str) {
    // sending as bytes in local encoding, kinda sloppy
    auto msgLen = str.length();
    zmq::message_t msg(msgLen);
    std::memcpy(msg.data(), &str[0], msgLen);
    return msg;
}

std::string s_recv(zmq::socket_t& socket) {
    zmq::message_t msg;
    socket.recv(&msg);
    return msg2string(msg);
}

void s_send(zmq::socket_t& socket, const std::string& str ) {
    auto msg = string2msg(str);
    socket.send(msg);
}
