#ifndef KALPA_RESULT_H
#define KALPA_RESULT_H


#include <utility>
#include <variant>

#include "defs.h"


namespace klp {


template<typename V, typename E>
class Result;


enum ResultIndex {
    ResultValueIndex,
    ResultErrorIndex
};


template <typename E, typename V>
Result<V, E> make_value(V value) {
    return Result<V, E>(
        std::in_place_index<ResultValueIndex>,
        std::move(value)
    );
}


template <typename V, typename E>
Result<V, E> make_error(E error) {
    return Result<V, E>(
        std::in_place_index<ResultErrorIndex>,
        std::move(error)
    );
}


template <typename V, typename E>
class Result : public std::variant<V, E> {
private:
    using Base = std::variant<V, E>;

public:
    using Value = V;
    using Error = E;

    enum Index {
        ValueIndex = ResultValueIndex,
        ErrorIndex = ResultErrorIndex
    };

public:
    using Base::Base;

    Result(Value value) :
        Base(std::in_place_index<ValueIndex>, std::move(value))
    {}

    template <
        typename H = void,
        typename = std::enable_if_t<!std::is_same_v<V, E>, H>
    > Result(Error error) :
        Base(std::in_place_index<ErrorIndex>, std::move(error))
    {}

    explicit operator bool() const {
        return Base::index() == ValueIndex;
    }

    Value* value_ptr() {
        return std::get_if<ValueIndex>(this);
    }

    const Value* value_ptr() const {
        return std::get_if<ValueIndex>(this);
    }

#define OPERATOR_STAR(maybe_const, ref, maybe_move) \
    maybe_const Value ref operator*() maybe_const ref { \
        return maybe_move(*value_ptr()); \
    }

    KALPA_VARY_CONST_MOVE(OPERATOR_STAR)
#undef OPERATOR_STAR

    Value* operator->() {
        return value_ptr();
    }

    const Value* operator->() const {
        return value_ptr();
    }

    Error* error_ptr() {
        return std::get_if<ErrorIndex>(this);
    }

    const Error* error_ptr() const {
        return std::get_if<ErrorIndex>(this);
    }

#define ERROR(maybe_const, ref, maybe_move) \
    maybe_const Error ref error() maybe_const ref { \
        return maybe_move(*error_ptr()); \
    }

    KALPA_VARY_CONST_MOVE(ERROR)
#undef ERROR

#define MAP(maybe_const, ref, maybe_move) \
    template <typename F> \
    auto map(F f) maybe_const ref { \
        return *this ? \
            make_value<E>(f(maybe_move(**this))) : \
            make_error<decltype(f(maybe_move(**this)))>(maybe_move(error())); \
    }

    KALPA_VARY_CONST_MOVE(MAP)
#undef MAP

#define MAP_ERROR(maybe_const, ref, maybe_move) \
    template <typename F> \
    auto map_error(F f) maybe_const ref { \
        return *this ? \
            make_value<decltype(f(maybe_move(error())))>(maybe_move(**this)) : \
            make_error<V>(f(maybe_move(error()))); \
    }

    KALPA_VARY_CONST_MOVE(MAP_ERROR)
#undef MAP_ERROR

#define AND_THEN(maybe_const, ref, maybe_move) \
    template <typename F> \
    auto and_then(F f) maybe_const ref { \
        return *this ? \
            f(maybe_move(**this)) : \
            decltype(f(maybe_move(**this)))(maybe_move(error())); \
    }

    KALPA_VARY_CONST_MOVE(AND_THEN)
#undef AND_THEN
};


template <typename E>
Result<E, E> as_error(E error) {
    return make_error<E>(std::move(error));
}


}


#endif
