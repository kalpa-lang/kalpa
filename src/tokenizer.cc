#include "tokenizer.h"

#include <algorithm>
#include <cctype>

#include "defs.h"

namespace klp {
void Tokenizer::trim(u32 trim_size) {
    source.remove_prefix(trim_size);
    offset += trim_size;
}

Token Tokenizer::handle_eof() {  // Returns an Eof token or a Dedent token if indentation level != 0
    if (indent_level) {
        --indent_level;
        return Token{Token::Type::Dedent, offset};
    } else {
        return Token{Token::Type::Eof, offset};
    }
}

// TODO Token construction gives warnings
// TODO dedent/indent offset is not calculated correctly
// TODO test for possible eof issues
// TODO add tokenization error handling
Token Tokenizer::next() {
    if (dedent_counder) {
        --dedent_counder;
        --indent_level;
        return handle_eof();
    }

    trim(std::min(source.find_first_not_of(' '), source.size()));
    if (source.empty()) {
        return handle_eof();
    }

    u32 token_offset = offset;
    u32 token_size = 0;  // may not be a correct value
    char last_char = source[0];

    if (last_char == '#') {
        while (token_size < source.size() && source[token_size] != '\n') {
            ++token_size;
        }
        trim(token_size);
        if (source.empty()) {
            return handle_eof();
        }

        last_char = source[0];
        token_size = 0;
    }

    if (last_char == 0) {
        todo();
    }

    while (last_char == '\n') {
        trim(1);
        u32 num_zeros = std::min(source.find_first_not_of(' '), source.size());
        trim(num_zeros);
        if (source.empty()) {
            return handle_eof();
        } else if ((last_char = source[0]) != '\n') {
            if (num_zeros % 4 != 0) {
                todo();
            }

            u32 current_indent_level = num_zeros / 4;
            if (current_indent_level > indent_level) {
                if (current_indent_level - indent_level == 1) {
                    indent_level = current_indent_level;
                    return Token{Token::Type::Indent, offset};
                } else {
                    todo();
                }
            } else if (current_indent_level < indent_level) {
                dedent_counder = (--indent_level) - current_indent_level;
                return Token{Token::Type::Dedent, offset};
            }
        }
    }

    if (std::isalpha(last_char)) {
        while (token_size < source.size() && std::isalnum(source[token_size])) {
            last_char = source[token_size++];
        }
        std::string_view token = source.substr(0, token_size);
        trim(token_size);

        if (token == "def") {
            return Token{ Token::Type::Def, token_offset };
        } else if (token == "class") {
            return Token{ Token::Type::Class, token_offset };
        } else if (token == "let") {
            return Token{ Token::Type::Let, token_offset };
        } else if (token == "for") {
            return Token{ Token::Type::For, token_offset };
        } else if (token == "while") {
            return Token{ Token::Type::While, token_offset };
        } else if (token == "if") {
            return Token{ Token::Type::If, token_offset };
        } else if (token == "else") {
            return Token{ Token::Type::Else, token_offset };
        } else if (token == "elif") {
            return Token{ Token::Type::Let, token_offset };
        } else if (token == "return") {
            return Token{ Token::Type::Return, token_offset };
        } else if (token == "in") {
            return Token{ Token::Type::In, token_offset };
        } else if (token == "not") {
            return Token{ Token::Type::Not, token_offset };
        } else if (token == "or") {
            return Token{ Token::Type::Or, token_offset };
        } else if (token == "and") {
            return Token{ Token::Type::And, token_offset };
        } else {
            return Token{ Token::Type::Identifier, token_offset, token };
        }
    }

    if (std::isdigit(last_char)) {  // TODO make the solution prettier
        i64 int_value = 0;
        while (token_size < source.size() && std::isdigit(source[token_size])) {
            last_char = source[token_size++];
            int_value = int_value * 10 + (last_char - '0');
        }

        if (source[token_size] != '.') {  // result is an integer
            trim(token_size);
            return Token{ Token::Type::Int, token_offset, int_value };
        } else {  // result is a float
            ++token_size;
            double float_value = 0;
            double pos_multiplicator = 1;
            while (token_size < source.size() && std::isdigit(source[token_size])) {
                last_char = source[token_size++];
                float_value += (pos_multiplicator /= 10) * (last_char - '0');
            }
            trim(token_size);
            return Token{ Token::Type::Float, token_offset, static_cast<double>(int_value) + float_value };
        }
    }

    if (last_char == '(') {
        trim(1);
        return Token{ Token::Type::LeftParen, token_offset };
    }

    if (last_char == ')') {
        trim(1);
        return Token{ Token::Type::RightParen, token_offset };
    }

    if (last_char == ':') {
        trim(1);
        return Token{ Token::Type::Colon, token_offset };
    }

    if (last_char == ',') {
        trim(1);
        return Token{ Token::Type::Comma, token_offset };
    }

    if (last_char == '.') {
        if (source.size() > 1 && std::isdigit(source[++token_size])) {
            double float_value = 0;
            double pos_multiplicator = 1;
            while (token_size < source.size() && std::isdigit(source[token_size])) {
                last_char = source[token_size++];
                float_value += (pos_multiplicator /= 10) * (last_char - '0');
            }
            trim(token_size);
            return Token{ Token::Type::Float, token_offset, float_value };
        } else {
            trim(1);
            return Token{ Token::Type::Dot, token_offset };
        }
    }

    if (last_char == '"') {
        ++token_size;
        std::string value;

        while (token_size < source.size() && source[token_size] != '"') {
            last_char = source[token_size++];
            if (last_char == '\\') {
                if (token_size < source.size()) {
                    last_char = source[token_size++];
                    if (last_char == 'n') {
                        value += '\n';
                    } else if (last_char == 'r') {
                        value += '\r';
                    } else if (last_char == '\\') {
                        value += '\\';
                    } else if (last_char == '"') {
                        value += '"';
                    } else {
                        todo();
                    }
                }
                else {
                    todo();
                }
            } else {
                value += last_char;
            }
        }

        if (source[token_size++] != '"') {
            todo();
        }

        trim(token_size);
        return Token{ Token::Type::String, token_offset, value };
    }

    if (last_char == '=') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token{ Token::Type::Equal, token_offset };
        } else {
            trim(1);
            return Token{ Token::Type::Assign, token_offset };
        }
    }

    if (last_char == '!') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token{ Token::Type::NotEqual, token_offset };
        } else {
            todo();
        }
    }

    if (last_char == '<') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token{ Token::Type::LessEq, token_offset };
        } else {
            trim(1);
            return Token{ Token::Type::Less, token_offset };
        }
    }

    if (last_char == '>') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token{ Token::Type::GreaterEq, token_offset };
        } else {
            trim(1);
            return Token{ Token::Type::Greater, token_offset };
        }
    }

    if (last_char == '+') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token{ Token::Type::AddEq, token_offset };
        } else {
            trim(1);
            return Token{ Token::Type::Add, token_offset };
        }
    }

    if (last_char == '-') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token{ Token::Type::SubEq, token_offset };
        } else {
            trim(1);
            return Token{ Token::Type::Sub, token_offset };
        }
    }

    if (last_char == '*') {
        if (source.size() > 1 && source[1] == '*') {
            if (source.size() > 2 && source[2] == '=') {
                trim(3);
                return Token{ Token::Type::PowEq, token_offset };
            } else {
                trim(2);
                return Token{ Token::Type::Pow, token_offset };
            }
        } else {
            if (source.size() > 1 && source[1] == '=') {
                trim(2);
                return Token{ Token::Type::MulEq, token_offset };
            } else {
                trim(1);
                return Token{ Token::Type::Mul, token_offset };
            }
        }
    }

    if (last_char == '/') {
        if (source.size() > 1 && source[1] == '/') {
            if (source.size() > 2 && source[2] == '=') {
                trim(3);
                return Token{ Token::Type::IntDivEq, token_offset };
            } else {
                trim(2);
                return Token{ Token::Type::IntDiv, token_offset };
            }
        } else {
            if (source.size() > 1 && source[1] == '=') {
                trim(2);
                return Token{ Token::Type::DivEq, token_offset };
            } else {
                trim(1);
                return Token{ Token::Type::Div, token_offset };
            }
        }
    }

    if (last_char == '^') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token{ Token::Type::XorEq, token_offset };
        } else {
            trim(1);
            return Token{ Token::Type::Xor, token_offset };
        }
    }

    if (last_char == '[') {
        trim(1);
        return Token{ Token::Type::LeftBracket, token_offset };
    }

    if (last_char == ']') {
        trim(1);
        return Token{ Token::Type::RightBracket, token_offset };
    }

    if (last_char == '{') {
        trim(1);
        return Token{ Token::Type::LeftBrace, token_offset };
    }

    if (last_char == '}') {
        trim(1);
        return Token{ Token::Type::RightBrace, token_offset };
    }
}
}
