#include <cerrno>
#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>

#include "defs.h"
#include "tokenizer.h"
#include "print.h"
#include "expression_parser.h"


namespace klp {


void print_file_error(const char* path) {
    eprint("Error: {}: {}\n", path, std::strerror(errno));
}


std::optional<std::vector<char>> read_file(const char* path) {
    std::optional<std::vector<char>> ret;

    const auto file = std::fopen(path, "r");
    if (!file) {
        print_file_error(path);
        return ret;
    }

    if (std::fseek(file, 0, SEEK_END) < 0) {
        print_file_error(path);
        return ret;
    }

    const auto file_size = std::ftell(file);
    if (file_size < 0) {
        print_file_error(path);
        return ret;
    }

    if (file_size == 0) {
        ret.emplace();
        return ret;
    }

    if (std::fseek(file, 0, SEEK_SET) < 0) {
        print_file_error(path);
        return ret;
    }

    std::vector<char> file_bytes(file_size);
    if (std::fread(file_bytes.data(), file_size, 1, file) != 1) {
        print_file_error(path);
        return ret;
    }

    ret = std::move(file_bytes);
    return ret;
}

void print_token(Token& token) {
    switch (token.type) {
        case Token::Type::Identifier            : eputs("Identifier"); break;
        case Token::Type::Indent                : eputs("Indent"); break;
        case Token::Type::Dedent                : eputs("Dedent"); break;
        case Token::Type::LeftParen             : eputs("LeftParen"); break;
        case Token::Type::RightParen            : eputs("RightParen"); break;
        case Token::Type::Colon         : eputs("Colon"); break;
        case Token::Type::Comma         : eputs("Comma"); break;
        case Token::Type::Dot           : eputs("Dot"); break;
        case Token::Type::Def           : eputs("Def"); break;
        case Token::Type::Class         : eputs("Class"); break;
        case Token::Type::Let           : eputs("Let"); break;
        case Token::Type::For           : eputs("For"); break;
        case Token::Type::While         : eputs("While"); break;
        case Token::Type::If            : eputs("If"); break;
        case Token::Type::Else          : eputs("Else"); break;
        case Token::Type::Elif          : eputs("Elif"); break;
        case Token::Type::In            : eputs("In"); break;
        case Token::Type::Not           : eputs("Not"); break;
        case Token::Type::Or            : eputs("Or"); break;
        case Token::Type::And           : eputs("And"); break;
        case Token::Type::Return                : eputs("Return"); break;
        case Token::Type::Int           : eputs("Int"); break;
        case Token::Type::Float         : eputs("Float"); break;
        case Token::Type::String                : eputs("String"); break;
        case Token::Type::Assign                : eputs("Assign"); break;
        case Token::Type::Equal         : eputs("Equal"); break;
        case Token::Type::NotEqual              : eputs("NotEqual"); break;
        case Token::Type::Less          : eputs("Less"); break;
        case Token::Type::LessEq                : eputs("LessEq"); break;
        case Token::Type::Greater               : eputs("Greater"); break;
        case Token::Type::GreaterEq             : eputs("GreaterEq"); break;
        case Token::Type::Add           : eputs("Add"); break;
        case Token::Type::Sub           : eputs("Sub"); break;
        case Token::Type::Mul           : eputs("Mul"); break;
        case Token::Type::Pow           : eputs("Pow"); break;
        case Token::Type::Div           : eputs("Div"); break;
        case Token::Type::IntDiv                : eputs("IntDiv"); break;
        case Token::Type::AddEq         : eputs("AddEq"); break;
        case Token::Type::SubEq         : eputs("SubEq"); break;
        case Token::Type::MulEq         : eputs("MulEq"); break;
        case Token::Type::PowEq         : eputs("PowEq"); break;
        case Token::Type::DivEq         : eputs("DivEq"); break;
        case Token::Type::IntDivEq              : eputs("IntDivEq"); break;
        case Token::Type::Xor           : eputs("Xor"); break;
        case Token::Type::XorEq         : eputs("XorEq"); break;
        case Token::Type::LeftBracket           : eputs("LeftBracket"); break;
        case Token::Type::RightBracket          : eputs("RightBracket"); break;
        case Token::Type::LeftBrace             : eputs("LeftBrace"); break;
        case Token::Type::RightBrace            : eputs("RightBrace"); break;
        case Token::Type::Eof           : eputs("Eof"); break;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        eputs("Usage: kalpa <path/to/source.kl>");
        return 1;
    }

    auto source = read_file(argv[1]);
    if (!source) {
        return 1;
    }

    source->push_back('\0');
    std::string_view s(source->data(), source->size());
    Tokenizer tokenizer(s);
    Node* tree = parse_expression(tokenizer);
    print(tree->to_string());

    return 0;
}


}


int main(int argc, char* argv[]) {
    return klp::main(argc, argv);
}
