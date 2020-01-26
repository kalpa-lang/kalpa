#include "tokenizer.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "defs.h"
#include "hashmap.h"

namespace klp {
void Tokenizer::trim(u32 trim_size) {
    source.remove_prefix(trim_size);
    offset += trim_size;
}

Token Tokenizer::handle_eof() {  // Returns an Eof token or a Dedent token if indentation level != 0
    if (indent_level) {
        --indent_level;
        return Token(Token::Type::Dedent, offset);
    } else {
        return Token(Token::Type::Eof, offset);
    }
}

Token::Type Tokenizer::get_string_token_type(std::string_view token) {
    const HashMap<std::string, Token::Type> keywords = {
        {"def", Token::Type::Def},
        {"class", Token::Type::Class},
        {"let", Token::Type::Let},
        {"for", Token::Type::For},
        {"while", Token::Type::While},
        {"if", Token::Type::If},
        {"else", Token::Type::Else},
        {"elif", Token::Type::Elif},
        {"return", Token::Type::Return},
        {"in", Token::Type::In},
        {"not", Token::Type::Not},
        {"or", Token::Type::Or},
        {"and", Token::Type::And}
    };
    
    auto it = keywords.find(std::string(token.data(), token.size()));
    if (it != keywords.end()) {  // token in keywords
        return it->second;
    } else {
        return Token::Type::Identifier;
    }
}

// TODO Write tests
// TODO add tokenization error handling
Token Tokenizer::next() {
    if (dedent_counder) {  // If any unhandled dedents are left
        --dedent_counder;
        --indent_level;
        return Token(Token::Type::Dedent, dedent_offset);
    }

    // Removing leading spaces
    trim(std::min(source.find_first_not_of(' '), source.size()));
    if (source.empty()) {
        return handle_eof();
    }

    u32 token_offset = offset;
    u32 token_size = 0;  // May not be a correct value
    char last_char = source[0];

    if (last_char == 0) {
        todo();
    }

    while (last_char == '\n' || last_char == '#') {  // Getting new indentation level
        if (last_char == '#') {  // Removing comments
            while (token_size < source.size() && source[token_size] != '\n') {
                ++token_size;
            }
            trim(token_size);
            if (source.empty()) {
                return handle_eof();
            }
        }

        trim(1);  // Removing the "\n"
        token_offset = offset;

        u32 num_zeros = std::min(source.find_first_not_of(' '), source.size());  // Removing leading spaces
        trim(num_zeros);
        last_char = source[0];

        if (source.empty()) {
            return handle_eof();
        } else if (last_char != '\n' && last_char != '#') {  // Line is not empty
            if (num_zeros % 4 != 0) {  // One level is 4 spaces
                todo();
            }

            u32 current_indent_level = num_zeros / 4;
            if (current_indent_level > indent_level) {
                if (current_indent_level - indent_level == 1) {  // Level increased
                    indent_level = current_indent_level;
                    return Token(Token::Type::Indent, token_offset);
                } else {  // Wrong indentation
                    todo();
                }
            } else if (current_indent_level < indent_level) {  // Level decreased
                dedent_counder = (--indent_level) - current_indent_level;  // Storing counter for future next() calls
                dedent_offset = token_offset;  // Storing offset for future next() calls
                return Token(Token::Type::Dedent, token_offset);
            }
        }
    }

    if (std::isalpha(last_char) || last_char == '_') {  // Identifier or keyword
        while (token_size < source.size() &&
              (std::isalnum(source[token_size]) || last_char == '_')) {  // Expanding the token
            last_char = source[token_size++];
        }
        std::string_view token = source.substr(0, token_size);
        trim(token_size);

        Token::Type token_type = get_string_token_type(token);
        if (token_type == Token::Type::Identifier) {
            return Token(token_type, token_offset, token);
        } else {
            return Token(token_type, token_offset);
        }
    }

    if (std::isdigit(last_char)) {
        i64 int_value = 0;  // Integer part
        while (token_size < source.size() && std::isdigit(source[token_size])) {  // Expanding integer part
            last_char = source[token_size++];
            int_value = int_value * 10 + (last_char - '0');
        }

        if (source[token_size] != '.') {  // Result is an integer
            trim(token_size);
            return Token(Token::Type::Int, token_offset, int_value);
        } else {  // Result is a float
            ++token_size;
            double float_value = 0;  // Float part
            double pos_multiplicator = 1;
            while (token_size < source.size() && std::isdigit(source[token_size])) {  // Expanding float part
                last_char = source[token_size++];
                float_value += (pos_multiplicator /= 10) * (last_char - '0');
            }
            trim(token_size);
            return Token(Token::Type::Float, token_offset, static_cast<double>(int_value) + float_value);
        }
    }

    if (last_char == '(') {
        trim(1);
        return Token(Token::Type::LeftParen, token_offset);
    }

    if (last_char == ')') {
        trim(1);
        return Token(Token::Type::RightParen, token_offset);
    }

    if (last_char == ':') {
        trim(1);
        return Token(Token::Type::Colon, token_offset);
    }

    if (last_char == ',') {
        trim(1);
        return Token(Token::Type::Comma, token_offset);
    }

    if (last_char == '.') {
        if (source.size() > 1 && std::isdigit(source[++token_size])) {  // Float starting with a dot
            double float_value = 0;
            double pos_multiplicator = 1;
            while (token_size < source.size() && std::isdigit(source[token_size])) {
                last_char = source[token_size++];
                float_value += (pos_multiplicator /= 10) * (last_char - '0');
            }
            trim(token_size);
            return Token(Token::Type::Float, token_offset, float_value);
        } else {
            trim(1);
            return Token(Token::Type::Dot, token_offset);
        }
    }

    if (last_char == '"') {  // String
        ++token_size;  // Skipping the quotation mark
        std::string value;

        while (token_size < source.size() && source[token_size] != '"') {  // Expanding the string
            last_char = source[token_size++];
            if (last_char == '\\') {  // Special symbol
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

        if (source[token_size++] != '"') {  // The string doesn't close
            todo();
        }

        trim(token_size);
        return Token(Token::Type::String, token_offset, value);
    }

    if (last_char == '=') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token(Token::Type::Equal, token_offset);
        } else {
            trim(1);
            return Token(Token::Type::Assign, token_offset);
        }
    }

    if (last_char == '!') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token(Token::Type::NotEqual, token_offset);
        } else {
            todo();
        }
    }

    if (last_char == '<') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token(Token::Type::LessEq, token_offset);
        } else {
            trim(1);
            return Token(Token::Type::Less, token_offset);
        }
    }

    if (last_char == '>') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token(Token::Type::GreaterEq, token_offset);
        } else {
            trim(1);
            return Token(Token::Type::Greater, token_offset);
        }
    }

    if (last_char == '+') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token(Token::Type::AddEq, token_offset);
        } else {
            trim(1);
            return Token(Token::Type::Add, token_offset);
        }
    }

    if (last_char == '-') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token(Token::Type::SubEq, token_offset);
        } else {
            trim(1);
            return Token(Token::Type::Sub, token_offset);
        }
    }

    if (last_char == '*') {
        if (source.size() > 1 && source[1] == '*') {
            if (source.size() > 2 && source[2] == '=') {
                trim(3);
                return Token(Token::Type::PowEq, token_offset);
            } else {
                trim(2);
                return Token(Token::Type::Pow, token_offset);
            }
        } else {
            if (source.size() > 1 && source[1] == '=') {
                trim(2);
                return Token(Token::Type::MulEq, token_offset);
            } else {
                trim(1);
                return Token(Token::Type::Mul, token_offset);
            }
        }
    }

    if (last_char == '/') {
        if (source.size() > 1 && source[1] == '/') {
            if (source.size() > 2 && source[2] == '=') {
                trim(3);
                return Token(Token::Type::IntDivEq, token_offset);
            } else {
                trim(2);
                return Token(Token::Type::IntDiv, token_offset);
            }
        } else {
            if (source.size() > 1 && source[1] == '=') {
                trim(2);
                return Token(Token::Type::DivEq, token_offset);
            } else {
                trim(1);
                return Token(Token::Type::Div, token_offset);
            }
        }
    }

    if (last_char == '^') {
        if (source.size() > 1 && source[1] == '=') {
            trim(2);
            return Token(Token::Type::XorEq, token_offset);
        } else {
            trim(1);
            return Token(Token::Type::Xor, token_offset);
        }
    }

    if (last_char == '[') {
        trim(1);
        return Token(Token::Type::LeftBracket, token_offset);
    }

    if (last_char == ']') {
        trim(1);
        return Token(Token::Type::RightBracket, token_offset);
    }

    if (last_char == '{') {
        trim(1);
        return Token(Token::Type::LeftBrace, token_offset);
    }

    if (last_char == '}') {
        trim(1);
        return Token(Token::Type::RightBrace, token_offset);
    }

    todo();
}
}
