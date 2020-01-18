#ifndef KALPA_DEFS_H
#define KALPA_DEFS_H


#include <cstddef>
#include <cstdint>


namespace klp {


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


[[noreturn]]
void todo(const char* msg = nullptr);


}


#endif
