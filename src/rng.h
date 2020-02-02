#ifndef KALPA_RNG_H
#define KALPA_RNG_H


#include <random>

#include "defs.h"


namespace klp {


template <usize NumBits, typename T>
class BitRng;


//
//  This is the xorshift128+ PRNG.
//
template <typename T>
class BitRng<64, T> {
private:
    using Self = BitRng<64, T>;

public:
    struct State {
        u64 a;
        u64 b;

        static State from_system_rng() {
            State state;

            std::random_device rd;
            std::uniform_int_distribution<u64> dist;
            state.a = dist(rd);
            state.b = dist(rd);

            return state;
        }
    };

public:
    BitRng(State state) : state(state) {}

    static Self from_system_rng() {
        return Self(State::from_system_rng());
    }

    T next() {
        u64 t = state.a;
        const u64 s = state.b;
        state.a = s;
        t ^= t << 23;
        t ^= t >> 17;
        t ^= s ^ (s >> 26);
        state.b = t;
        return t + s;
    }

private:
    State state;
};


//
//  This is the xorwow PRNG.
//
template <typename T>
class BitRng<32, T> {
private:
    using Self = BitRng<32, T>;

public:
    struct State {
        u32 a, b, c, d;
        u32 counter;

        static State from_system_rng() {
            State state;

            std::random_device rd;
            std::uniform_int_distribution<u32> dist;
            state.a = dist(rd);
            state.b = dist(rd);
            state.c = dist(rd);
            state.d = dist(rd);
            state.counter = 6615241;

            return state;
        }
    };

public:
    BitRng(State state) : state(state) {}

    static Self from_system_rng() {
        return Self(State::from_system_rng());
    }

    T next() {
        u32 t = state.d;

        const u32 s = state.a;
        state.d = state.c;
        state.c = state.b;
        state.b = s;

        t ^= t >> 2;
        t ^= t << 1;
        t ^= s ^ (s << 4);
        state.a = t;

        state.counter += 362437;
        return t + state.counter;
    }

private:
    State state;
};


template <typename T>
using Rng = BitRng<sizeof(T) * 8, T>;


}


#endif
