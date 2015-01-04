// some helpers for sending strings as zmq messages

#include <iostream>
#include "google/protobuf/text_format.h" // TextFormat::PrintToString
#include "foo/zmq_protobuf_msg.h"
#include "foo/zmq_text_msg.h"

// general utility func
std::string Message2HumanReadableString(const google::protobuf::Message& msg) {
    std::string result;
    google::protobuf::TextFormat::PrintToString(msg, &result);
    return result;
}

tutorial::MessageUnion msg2protobuf(const zmq::message_t& msg) {
    // there's needless copying going on here. TODO: avoid the copies
    const auto& encoded = msg2string(msg);
    tutorial::MessageUnion result;
    result.ParseFromString(encoded);
    return result;
}

zmq::message_t protobuf2msg(const tutorial::MessageUnion& msg) {
    // there's needless copying going on here. TODO: avoid the copies
    std::string encoded;
    msg.SerializeToString(&encoded);

    /* verbose
    std::cout << "message txt=" << Message2HumanReadableString(msg)
        << " len=" << Message2HumanReadableString(msg).length() << " encoded as "
        //<< " enc=" << encoded  // hex gibberish, good, could print as hex chars
        << " len=" << encoded.length() << '\n';
    */
        
    return string2msg(encoded);
}
