#include "defs.h"

#include <cstdio>
#include <cstdlib>

#include "print.h"


namespace klp {


void todo(const char* msg) {
    if (msg) {
        eprint("Error: not implemented: {}\n", msg);
    } else {
        eprint("Error: not implemented!\n");
    }

    std::fflush(stdout);
    std::fflush(stderr);

    std::abort();
}


}
