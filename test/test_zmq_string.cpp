#include <string>
#include "foo/zmq_string.h"
#include "gtest/gtest.h" // from module googletest

TEST(zmq_string_test, send_recv) {
    EXPECT_EQ(msg2string(string2msg("hi")), "hi");
}

