#include "zmq.hpp" // from module cppzmq
#include <string>

// some helpers for sending strings as zmq messages
zmq::message_t string2msg(const std::string& str);
std::string msg2string(const zmq::message_t& msg);

// methods for sending a text message over a zmq socket
std::string recv_text_msg(zmq::socket_t& socket);
void send_text_msg(zmq::socket_t& socket, const std::string& str);
