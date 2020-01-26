#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <string_view>
#include <variant>

#include "defs.h"

namespace klp {
using Value = std::variant<std::string, std::string_view, i64, double>;

struct Token {
    enum class Type {  // TODO add bit operations and lambdas
        Identifier,

        Indent,
        Dedent,

        LeftParen,
        RightParen,

        Colon,

        Comma,

        Dot,

        Def,
        Class,
        Let,

        For,
        While,

        If,
        Else,
        Elif,

        In,

        Not,
        Or,
        And,

        Return,

        Int,
        Float,
        String,

        Assign,

        Equal,
        NotEqual,
        Less,
        LessEq,
        Greater,
        GreaterEq,

        Add,
        Sub,
        Mul,
        Pow,
        Div,
        IntDiv,

        AddEq,
        SubEq,
        MulEq,
        PowEq,
        DivEq,
        IntDivEq,

        Xor,

        XorEq,

        LeftBracket,
        RightBracket,

        LeftBrace,
        RightBrace,

        Eof
    };

    Token(Type type, u32 offset, Value value = Value()) : type(type),
                                                          offset(offset),
                                                          value(value) {}

    Type type;
    u32 offset;
    Value value;
};


class Tokenizer {
public:
    Tokenizer(std::string_view source) : source(source) {}

    Token next();

private:
    std::string_view source;
    u32 offset = 0;
    u32 indent_level = 0;
    u32 dedent_counder = 0;
    u32 dedent_offset = 0;

    void trim(u32 trim_size);
    void trim_comment();
    Token handle_eof();
    Token::Type get_string_token_type(const std::string_view token);
};
}

#endif
