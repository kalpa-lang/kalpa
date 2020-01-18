#include <cerrno>
#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>

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

    const auto source = read_file(argv[1]);
    if (!source) {
        return 1;
    }

    print("source size: {}\n", source->size());

    return 0;
}


}


int main(int argc, char* argv[]) {
    return klp::main(argc, argv);
}
