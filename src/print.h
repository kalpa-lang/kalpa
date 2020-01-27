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


template <typename S, typename... Args>
void println(const S& format, const Args&... args) {
    print(format, args...);
    print("\n");
}


template <typename S, typename... Args>
void eprint(const S& format, const Args&... args) {
    print(stderr, format, args...);
}


template <typename S, typename... Args>
void eprintln(const S& format, const Args&... args) {
    eprint(format, args...);
    eprint("\n");
}


}


#endif
