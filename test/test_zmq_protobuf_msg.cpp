#include "src/addressbook.pb.h"
#include "foo/zmq_protobuf_msg.h"
#include "gtest/gtest.h" // from module googletest

TEST(zmq_protobuf_msg_test, send_recv) {
    tutorial::MessageUnion msg;
    auto person = msg.mutable_person();
    person->set_name("bob");
    person->set_id(678);
    auto phone = person->add_phone();
    phone->set_number("4125556666");
    phone->set_type(tutorial::Person::WORK);
    tutorial::MessageUnion out = msg2protobuf(protobuf2msg(msg));
    EXPECT_TRUE(out.msg_case() == tutorial::MessageUnion::kPerson);
    EXPECT_TRUE(out.has_person());
    EXPECT_EQ(out.person().name(), msg.person().name()); 
    // note that person() creates a default instance if the oneof did
    // not contain a person, I had expected exception instead.
}
