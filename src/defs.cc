#include "defs.h"

#include <cstdio>
#include <cstdlib>

#include "print.h"


namespace klp {


[[noreturn]]
static void flush_and_die() {
    std::fflush(stdout);
    std::fflush(stderr);
    std::abort();
}


void verify(bool cond, const char* msg) {
    if (!cond) {
        if (msg) {
            eprint("Condition violated: {}\n", msg);
        }

        flush_and_die();
    }
}


void todo(const char* msg) {
    if (msg) {
        eprint("Error: not implemented: {}\n", msg);
    } else {
        eprint("Error: not implemented!\n");
    }

    flush_and_die();
}


}
