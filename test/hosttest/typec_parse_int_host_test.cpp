#include "typec_parse_int.h"
#include <climits>
#include <cstdio>
#include <cstdlib>

static void Expect(bool cond, const char *msg)
{
    if (!cond) {
        std::fprintf(stderr, "FAIL: %s\n", msg);
        std::exit(1);
    }
}

int main()
{
    int32_t v = 42;
    Expect(ParseNumericInt("0", v) && v == 0, "zero");
    Expect(ParseNumericInt("35000", v) && v == 35000, "milli");
    Expect(ParseNumericInt("-1", v) && v == -1, "neg");
    Expect(ParseNumericInt("2147483647", v) && v == INT_MAX, "max");
    Expect(!ParseNumericInt("", v), "empty");
    Expect(!ParseNumericInt("abc", v), "abc");
    Expect(!ParseNumericInt("12a", v), "12a");
    Expect(!ParseNumericInt("9999999999999999999", v), "huge");
    Expect(!ParseNumericInt("2147483648", v), "overflow");
    Expect(!ParseNumericInt(" 3", v), "space");
    std::puts("ok");
    return 0;
}
