// some helpers for sending strings as zmq messages

#include "foo/zmq_text_msg.h"
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
