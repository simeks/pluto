// Copyright 2008-2014 Simon Ekström

#include "Framework.h"

namespace testing
{

    //-------------------------------------------------------------------------------
    TestCase* TestCase::Next() const
    {
        return _next;
    }
    void TestCase::SetNext(TestCase* test)
    {
        _next = test;
    }
    const char* TestCase::Name() const
    {
        return _name;
    }
    //-------------------------------------------------------------------------------


    //-------------------------------------------------------------------------------
    namespace
    {
        TestCase* g_head_test_case = nullptr;
    };

    void test_framework::RegisterTest(TestCase* test_case)
    {
        if (g_head_test_case)
        {
            // Put the new test case last in the chain.

            TestCase* next = g_head_test_case;
            while (next->Next())
            {
                next = next->Next();
            }
            next->SetNext(test_case);
        }
        else
            g_head_test_case = test_case;

    }

    bool test_framework::RunTests()
    {
        uint32_t test_count = 0;
        uint32_t fail_count = 0;

        TestCase* test = g_head_test_case;
        while (test)
        {
            TestResult result;
            result.result = TestResult::PASSED;

            test->Run(result);

            if (result.result == TestResult::FAILED)
            {
                printf("[FAILED] %s\n[------] %s(%d): %s\n", test->Name(), result.error_file, result.error_line, result.error_msg);
                ++fail_count;
            }
            else
            {
                printf("[PASSED] %s\n", test->Name());
            }

            ++test_count;
            test = test->Next();
        }

        printf("\nSummary: %d tests run, %d tests failed.\n", test_count, fail_count);

        return (fail_count == 0);
    }
    //-------------------------------------------------------------------------------
    void test_assert_helper::FormatAssertMessage(TestResult& test_result, const char* file, int line, const char* fmt, ...)
    {
        test_result.error_file = file;
        test_result.error_line = line;

        va_list args;
        va_start(args, fmt);
        vsnprintf(test_result.error_msg, sizeof(test_result.error_msg), fmt, args);
        va_end(args);

        test_result.error_msg[sizeof(test_result.error_msg) - 1] = '\0';
    }

    //-------------------------------------------------------------------------------


} // namespace sb

int main()
{
    memory::initialize();

    // Let the program return 1 if any of the tests fails.
    int ret = testing::test_framework::RunTests() ? 0 : 1;

    memory::shutdown();

    return ret;
}
