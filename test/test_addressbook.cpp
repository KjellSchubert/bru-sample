#include "src/addressbook.pb.h"
#include "gtest/gtest.h" // from module googletest
#include <string>
#include <type_traits> // std::is_same, just to verify we compile under >= C++11

using string = std::string;
using namespace tutorial; // Person from addressbook.proto

// silly test, whatever, just playing with generated protobuf API
TEST(AddressbookTest, set_name) {
    Person person;
    person.set_name("bob");
    person.set_id(678);
    auto phone = person.add_phone(); // no RAII? life time?
    phone->set_number("4125556666");
    phone->set_type(Person::WORK);
    EXPECT_EQ(person.name(), "bob");
    
    // this just verifies we have basic C++11 features available
    static_assert(std::is_same<
        decltype(phone), 
        tutorial::Person_PhoneNumber*>::value, "");
}
