#ifndef KALPA_DEFS_H
#define KALPA_DEFS_H


#include <cstddef>
#include <cstdint>

#include "print.h"


namespace klp {


#define KALPA_IDENTITY(x) (x)


#define KALPA_DO_STRINGIFY(s) #s
#define KALPA_STRINGIFY(s) KALPA_DO_STRINGIFY(s)


using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::size_t;


using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using isize = std::intptr_t;


void verify(bool cond, const char* msg = nullptr);


template <typename T, typename U>
void verify_eq(const T& x, const U& y) {
    if (x != y) {
        eprint("Condition violated: {} == {}\n", x, y);
        verify(false);
    }
}


#define KALPA_VERIFY(cond) verify(cond, KALPA_STRINGIFY(cond))


[[noreturn]]
void todo(const char* msg = nullptr);


#define KALPA_VARY_CONST_MOVE(f) \
    f(const, &, KALPA_IDENTITY); \
    f(, &, KALPA_IDENTITY); \
    f(const, &&, std::move); \
    f(, &&, std::move);


template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;


}


#endif
