#ifndef KALPA_TEST_H
#define KALPA_TEST_H


#include <string_view>
#include <vector>

#include "defs.h"


namespace klp {


class Test;

std::vector<Test*>* shared_tests();


class Test {
public:
    const std::string_view name;

public:
    Test(std::string_view name, void (*func)()) : name(name), func(func) {
        shared_tests()->push_back(this);
    }

    void run() {
        func();
    }

private:
    void (*func)();
};


#define KALPA_TEST(name) \
    void test_ ## name(); \
    Test test_ctor_ ## name(KALPA_STRINGIFY(name), test_ ## name); \
    void test_ ## name()


}


#endif
