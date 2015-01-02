#include "zmq.hpp" // from module cppzmq
#include <string>

// some helpers for sending strings as zmq messages
zmq::message_t string2msg(const std::string& str);
std::string msg2string(const zmq::message_t& msg);

std::string s_recv(zmq::socket_t& socket);
void s_send(zmq::socket_t& socket, const std::string& str);
