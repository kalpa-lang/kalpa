#include <string_view>
#include <unordered_set>

#include "print.h"

#include "test.h"


namespace klp {


int main(int argc, char* argv[]) {
    std::unordered_set<std::string_view> tests;
    tests.reserve(argc - 1);

    for (int i = 1; i < argc; ++i) {
        tests.emplace(argv[i]);
    }

    for (const auto test : *shared_tests()) {
        if (tests.count(test->name)) {
            test->run();
        }
    }

    return 0;
}


}


int main(int argc, char* argv[]) {
    return klp::main(argc, argv);
}
