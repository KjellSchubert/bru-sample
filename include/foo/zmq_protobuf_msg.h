#include "zmq.hpp" // from module cppzmq
#include "src/addressbook.pb.h"

// general utility func
std::string Message2HumanReadableString(const google::protobuf::Message& msg);

// some helpers for sending protobuf objects as zmq messages,
// same behaviour as in the string case btw
tutorial::MessageUnion msg2protobuf(const zmq::message_t& msg);
zmq::message_t protobuf2msg(const tutorial::MessageUnion& str);

// methods for sending a text message over a zmq socket
tutorial::MessageUnion recv_protobuf_msg(zmq::socket_t& socket);
void send_protobuf_msg(zmq::socket_t& socket, const tutorial::MessageUnion& str );
