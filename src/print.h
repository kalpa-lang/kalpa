#ifndef KALPA_PRINT_H
#define KALPA_PRINT_H


#include <string_view>

#include <fmt/format.h>


namespace klp {


using std::puts;

using fmt::format;
using fmt::print;


inline void eputs(const char* msg) {
    fmt::print(stderr, "{}\n", msg);
}


template <typename... Args>
void eprint(std::string_view format, const Args&... args) {
    fmt::print(stderr, format, args...);
}


}


#endif
