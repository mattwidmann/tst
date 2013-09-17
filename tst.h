#ifndef TST_H
#define TST_H

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

#define TST_OUTPUT stdout
// called to report on status of last test and inform what new test is about to take place
#define TST_REPORTER tst_default_reporter

enum tst_status {
    tst_pass = 0,
    tst_fail,
    tst_time,
    tst_crash
};

struct tst_progress {
    char * suite_name;
    char * test_name;
    char * test_message;

    char * file;
    unsigned int line;

    unsigned int suite;
    unsigned int test;
    unsigned int test_in_suite;

    unsigned int setup_ms;
    unsigned int test_ms;
    unsigned int teardown_ms;

    unsigned int status:3;
    unsigned int done:1;
};

// globals

static clock_t tst_all_start_ms;
static clock_t tst_start_clock;
static void (*tst_setup)(void * data);
static void * tst_setup_data;
static void (*tst_teardown)(void * data);
static void * tst_teardown_data;
// TODO rework progress struct
static struct tst_progress tst_progress = {
    /* suite_name */   NULL,
    /* test_name */    NULL,
    /* test_message */ NULL,
    /* file */         NULL,
    /* line */         0,
    /* suite */        0,
    /* test */         0,
    /* tests_in_suite */         0,
    /* setup_ms */      0,
    /* test_ms */      0,
    /* teardown_ms */      0,
    /* status */       0,
    /* done */         0
};

static void tst_default_reporter(struct tst_progress * progress)
{
    if (progress == NULL)
        return;

    if (progress->test_in_suite != 0) {
        fprintf(TST_OUTPUT, "%s\n", progress->status == tst_pass ? "pass" : "fail");
    }

    if (progress->done)
        return;

    fprintf(TST_OUTPUT, "(%i / ?) %s -- %s ", progress->test, progress->test_name, progress->test_message);
    fflush(TST_OUTPUT);
}

// test reporting

// defining tests and suites

#define TST_TEST(NAME, ...)                             \
    static enum tst_status tst_test_##NAME(__VA_ARGS__)

#define TST_SUITE(NAME)                \
    static void tst_suite_##NAME(void)

// test results

#define TST_ASSERT(CONDITION) do {        \
        if (!(CONDITION)) {               \
            tst_progress.file = __FILE__; \
            tst_progress.line = __LINE__; \
            return tst_fail;              \
        }                                 \
    } while (0)

#define TST_PASS return tst_pass
#define TST_FAIL return tst_fail

// setup and teardown for tests in a suite

#define TST_SET_SETUP(NAME, DATA) do { \
        tst_setup = NAME;              \
        tst_setup_data = DATA;         \
    } while (0)

#define TST_SET_TEARDOWN(NAME, DATA) do { \
        tst_teardown = NAME;              \
        tst_teardown_data = DATA;         \
    } while (0)

// handle command line arguments
#define TST_MAIN(ARGC, ARGV) do {  \
        tst_set_sigsegv_handler(); \
        tst_set_sigalrm_handler(); \
        TST_REPORTER(NULL);        \
    } while (0)

// call the last progress update
#define TST_MAIN_END do {            \
        tst_progress.done = 1;       \
        TST_REPORTER(&tst_progress); \
    } while (0)

// run suites and tests

#define TST_RUN_SUITE(NAME) do {         \
        tst_setup = NULL;                \
        tst_teardown = NULL;             \
        tst_setup_data = NULL;           \
        tst_teardown_data = NULL;        \
        tst_progress.suite += 1;         \
        tst_progress.suite_name = #NAME; \
        tst_progress.test_in_suite = 0;  \
        tst_suite_##NAME();              \
    } while (0)

// bookkeeping information for crash and timeout conditions

static jmp_buf tst_jmp_buf;

static void tst_recover(int sig)
{
    siglongjmp(tst_jmp_buf, 1);
}

static void tst_set_sigsegv_handler(void)
{
    // TODO make this a bit more robust
    if (signal(SIGSEGV, tst_recover) == SIG_ERR)
        fprintf(stderr, "can't set SIGSEGV handler");
}

static jmp_buf tst_alrm_jmp_buf;

static void tst_timeout(int sig)
{
    longjmp(tst_alrm_jmp_buf, 1);
}

static void tst_set_sigalrm_handler(void)
{
    // TODO make this a bit more robust
    if (signal(SIGALRM, tst_timeout) == SIG_ERR)
        fprintf(stderr, "can't set SIGALRM handler");
}

#define TST_MS_FROM(CLOCK) (unsigned long)(clock() - CLOCK) / CLOCKS_PER_SEC / 1000

// TODO time estimation using clock() is not very accurate
#define TST_RUN_TEST(NAME, MESSAGE, ...) do {                        \
        tst_progress.test += 1;                                      \
        tst_progress.test_name = #NAME;                              \
        tst_progress.test_message = MESSAGE;                         \
        TST_REPORTER(&tst_progress);                                 \
        if (tst_setup) {                                             \
            tst_start_clock = clock();                               \
            tst_setup(tst_setup_data);                               \
            tst_progress.setup_ms = TST_MS_FROM(tst_start_clock);    \
        } else {                                                     \
            tst_progress.setup_ms = 0;                               \
        }                                                            \
        tst_start_clock = clock();                                   \
        if (sigsetjmp(tst_jmp_buf, 0) == 0) {                        \
            alarm(4);                                                \
            if (!sigsetjmp(tst_alrm_jmp_buf, 0)) {                   \
                tst_progress.status = tst_test_##NAME(__VA_ARGS__);  \
            } else {                                                 \
                tst_progress.status = tst_time;                      \
            }                                                        \
        } else {                                                     \
            tst_progress.status = tst_crash;                         \
        }                                                            \
        tst_progress.test_ms = TST_MS_FROM(tst_start_clock);         \
        if (tst_teardown) {                                          \
            tst_start_clock = clock();                               \
            tst_teardown(tst_teardown_data);                         \
            tst_progress.teardown_ms = TST_MS_FROM(tst_start_clock); \
        } else {                                                     \
            tst_progress.teardown_ms = 0;                            \
        }                                                            \
        tst_progress.test_in_suite += 1;                             \
    } while (0)

#endif
