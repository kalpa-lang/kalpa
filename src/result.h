#ifndef KALPA_RESULT_H
#define KALPA_RESULT_H


#include <utility>
#include <variant>


namespace klp {


template <typename V, typename E>
class Result : public std::variant<V, E> {
private:
    using Base = std::variant<V, E>;

public:
    using Value = V;
    using Error = E;

    enum VariantIndex {
        ValueIndex,
        ErrorIndex
    };

public:
    using Base::Base;

    Result(Value&& value) :
        Base(std::in_place_index<ValueIndex>, std::move(value))
    {}

    template <
        typename H = void,
        typename = std::enable_if_t<!std::is_same_v<V, E>, H>
    > Result(Error&& error) :
        Base(std::in_place_index<ErrorIndex>, std::move(error))
    {}

    operator bool() const {
        return Base::index() == ValueIndex;
    }

    Value& operator*() {
        return *std::get_if<ValueIndex>(this);
    }

    const Value& operator*() const {
        return *std::get_if<ValueIndex>(this);
    }

    Value* operator->() {
        return &**this;
    }

    const Value* operator->() const {
        return &**this;
    }

    template <typename F>
    auto map(F f) & {
        return *this ? f(**this) : *this;
    }

    template <typename F>
    auto map(F f) const & {
        return *this ? f(**this) : *this;
    }

    template <typename F>
    auto map(F f) && {
        return *this ? f(std::move(**this)) : *this;
    }

    // TODO: map_err, and_then, etc..
};


template <typename E, typename V = E>
Result<V, E> make_error(E&& error) {
    return Result<V, E>(
        std::in_place_index<Result<V, E>::ErrorIndex>,
        std::move(error)
    );
}


}


#endif
