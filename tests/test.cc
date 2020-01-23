#include "test.h"


namespace klp {


static std::vector<Test*>* _shared_tests = nullptr;

std::vector<Test*>* shared_tests() {
    if (!_shared_tests) {
        _shared_tests = new std::vector<Test*>;
    }

    return _shared_tests;
}


}
