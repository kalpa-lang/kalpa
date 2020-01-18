#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <string_view>
#include <variant>

namespace klp {
struct Token {
    enum class Class {
        Identifier,

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
        
        Assign,

        Equal,
        NotEqual,
        Less,
        Greater,
        
        Add,
        Sub,
        Mul,
        Div,
        IntDiv,

        AddEq,
        SubEq,
        MulEq,
        DivEq,
        IntDivEq,

        LeftBracket,
        RightBracket,

        LeftBrace,
        RightBrace,
    };

    std::variant<std::string, std::string_view> string_value;

    Token(Class);
    Token(Class, std::string);
    Token(Class, std::string_view);
};


class Tokenizer {
public:
    Tokenizer(std::string_view code);
    
    Token next();
};
}

#endif
