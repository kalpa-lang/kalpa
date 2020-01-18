#include <cerrno>
#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>

#include "defs.h"
#include "tokenizer.h"
#include "print.h"


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

    while (true) {
        auto token = tokenizer.next();
        if (token.type == Token::Type::Eof) {
            break;
        }

        std::visit([] (auto& x) {
            print("{}\n", x);
        }, token.value);
    }

    return 0;
}


}


int main(int argc, char* argv[]) {
    return klp::main(argc, argv);
}
