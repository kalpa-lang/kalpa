#include "defs.h"
#include "tokenizer.h"

#include "test.h"


namespace klp {


KALPA_TEST(tokenizer) {
    int x = 2 * 2;
    int y = 4;
    verify_eq(x, y);
}


}
