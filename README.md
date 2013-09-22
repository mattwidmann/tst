`tst`
=====

`tst` is a general purpose test framework for C.  It borrows heavily from existing testing frameworks like `greatest`, `Mocha`, and `Check`, but is a bit more flexible in how it reports tests than most C testing frameworks.  Because `tst` is written in and for C, but also must present a simple language for specifying tests, it makes extensive use of macros, which are all prefixed by `TST_`.

Using `tst` starts with tests that make up a suite.  These tests are declared using the `TST_TEST` macro, which takes a name to refer to and a message that describes what the test does.  If `tst` is being used in an environment with C99 or later, `TST_TEST` is a variadic macro that also takes a list of arguments that are passed to the test from the suite.  The body of the test comes directly after the macro, since `TST_TEST` also expands to a function prototype for the test.

The next step in setting up a test environment is to define what tests belong to each suite, as well as the suites themselves. Test suites are created by `TST_SUITE`, which takes only the name used to refer to the suite. Inside the body of the suite, any setup or teardown needed by the entire suite can be added, since it's simply run as a function. Setup and teardown routines that will run for each test can also be specified, using `TST_SET_SETUP` and `TST_SET_TEARDOWN`, which both take the name of the function to be called and any user data needed by them. Setup and teardown functions should have the following function signature:

    void name(void * data);

To run tests in a test suite, use `TST_RUN_TEST`, and specify the name of the test to run (as well as the arguments the test takes, if making use of this feature).

Status
------

Argument parsing is not implemented, nor are any complex test reporters.

Usage
-----

```
$ ./test_runner -h
usage: test_runner [-bcChl] [-s <name>] [-t <name>]

 -b, --bail         bail after first failure
 -c, --color        show test results in color
 -C, --no-color     show test results without color
 -h, --help         print this message
 -l, --list         list suites and their tests
 -s, --suite <name> only run suite named <name>
 -t, --test <name>  only run test named <name>
```

Example
-------

```c
#include <stdlib.h>

#include "tst.h"

TST_TEST(zero_memory, "should set memory to 0", int * fixture)
{
    TST_ASSERT(fixture[0] == 0);
}

static void setup(void * data)
{
    *((int * *)data) = calloc(sizeof(int), 10);
}

static void teardown(void * data)
{
    free(*(int * *)data);
}

TST_SUITE(calloc)
{
    int * fixture;
    TST_SET_SETUP(setup, &fixture);
    TST_SET_TEARDOWN(teardown, &fixture);

    TST_RUN_TEST(zero_memory, fixture);
}

int main(int argc, char * argv[])
{
    TST_MAIN(argc, argv);

    TST_RUN_SUITE(calloc);
}
```

```sh
$ gcc example.c -o example && ./example

calloc
  ✔ should set memory to 0

✔ 1 test completed (2ms)

```

Test Cases
----------

Tests can only pass if they `return 0` or use the `PASS` macro. The `FAIL` and `SKIP` macros will register the test as a failure or skipped, respecively. A function that causes a `SIGSEGV` or other abnormal failure will register as a crash, but execution of tests will continue. Tests that take longer than `CONTEST_TIMEOUT` (default is 5 seconds) will cause the test to register as timed out.

License
-------

The MIT License (MIT)

Copyright (c) 2013 Matt Widmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
