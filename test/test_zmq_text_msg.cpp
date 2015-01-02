#include <string>
#include "foo/zmq_text_msg.h"
#include "gtest/gtest.h" // from module googletest

TEST(zmq_text_msg_test, send_recv) {
    EXPECT_EQ(msg2string(string2msg("hi")), "hi");
}

