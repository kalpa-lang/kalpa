#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <string_view>
#include <variant>

#include "defs.h"

namespace klp {
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

    Type type;
    u32 offset;
    std::variant<std::string, std::string_view, i64, double> value;
};


class Tokenizer {
public:
    Tokenizer(std::string_view source) : source(source) {}

    Token next();

private:
    std::string_view source;
    u32 offset = 0;
    u32 indent_level = 0;
    u32 dedent_counter = 0;

    void trim(u32 trim_size);
    Token handle_eof();
};
}

#endif
