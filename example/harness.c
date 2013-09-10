#include "tst.h"

#include "lib.h"

TST_TEST(returns_1)
{
    TST_ASSERT(1 == returns_1());

    TST_PASS;
}

TST_SUITE(lib)
{
    TST_RUN_TEST(returns_1, "should return 1");
}

int main(int argc, char * * argv)
{
    TST_MAIN(argc, argv);
    TST_RUN_SUITE(lib);
    TST_MAIN_END;

    return 0;
}
