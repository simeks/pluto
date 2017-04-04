// Copyright 2008-2014 Simon Ekström

#ifndef __UNITTEST_FRAMEWORK_H__
#define __UNITTEST_FRAMEWORK_H__

#include <Core/Common.h>

namespace testing
{

    //-------------------------------------------------------------------------------
    struct TestResult
    {
        enum Result { FAILED, PASSED };

        Result result;

        const char* error_file; // File name in case the test failed.
        int error_line; // Line number in case the test failed.
        char error_msg[2048]; // Error message in case the test failed.
    };

    class TestCase
    {
    public:
        TestCase(const char* name) : _name(name), _next(NULL) {}
        virtual ~TestCase() {}
        
        virtual void Run(TestResult& _test_result) = 0;

        TestCase* Next() const;
        void SetNext(TestCase* test);

        const char* Name() const;

    private:
        const char* _name;
        TestCase* _next;
    };

    //-------------------------------------------------------------------------------
    namespace test_framework
    {
        void RegisterTest(TestCase* test_case);

        /// Runs all tests
        /// @return True if all tests succeeded, false if any test failed.
        bool RunTests();

    };

    //-------------------------------------------------------------------------------
    namespace test_assert_helper
    {
        void FormatAssertMessage(TestResult& test_result, const char* file, int line, const char* fmt, ...);


        template<typename T, typename U>
        INLINE bool AssertEqual(const char* file, int line, T value, U expected, TestResult& test_result);
        template<typename T, typename U>
        INLINE bool AssertNotEqual(const char* file, int line, T value, U expected, TestResult& test_result);

        INLINE bool AssertEqualFloat(const char* file, int line, double value, double expected,
            double tolerance, TestResult& test_result);
        INLINE bool AssertNotEqualFloat(const char* file, int line, double value, double expected,
            double tolerance, TestResult& test_result);

        INLINE bool AssertExpression(const char* file, int line, const char* expr, bool expr_result, TestResult& test_result);
    };

    //-------------------------------------------------------------------------------

} // namespace testing



//-------------------------------------------------------------------------------
template<typename T, typename U>
INLINE bool testing::test_assert_helper::AssertEqual(const char* file, int line, T value, U expected,
    TestResult& test_result)
{
    if ((U)value != expected)
    {
        std::stringstream value_str; value_str << value;
        std::stringstream expected_str; expected_str << expected;

        FormatAssertMessage(test_result, file, line, "equality failed: %s != %s", 
            value_str.str().c_str(), expected_str.str().c_str());

        return false;
    }
    return true;
}

template<typename T, typename U>
INLINE bool testing::test_assert_helper::AssertNotEqual(const char* file, int line, T value, U expected,
    TestResult& test_result)
{
    if ((U)value == expected)
    {
        std::stringstream value_str; value_str << value;
        std::stringstream expected_str; expected_str << expected;

        FormatAssertMessage(test_result, file, line, "inequality failed: %s == %s", 
            value_str.str().c_str(), expected_str.str().c_str());

        return false;
    }
    return true;
}

template<>
INLINE bool testing::test_assert_helper::AssertEqual(const char* file, int line, const char* value,
    const char* expected, TestResult& test_result)
{
    if (strcmp(value, expected) != 0)
    {
        FormatAssertMessage(test_result, file, line, "equality failed: \"%s\" != \"%s\"", value, expected);
        return false;
    }
    return true;
}

template<>
INLINE bool testing::test_assert_helper::AssertNotEqual(const char* file, int line, const char* value,
    const char* expected, TestResult& test_result)
{
    if (strcmp(value, expected) == 0)
    {
        FormatAssertMessage(test_result, file, line, "inequality failed: \"%s\" == \"%s\"", value, expected);
        return false;
    }
    return true;
}

INLINE bool testing::test_assert_helper::AssertEqualFloat(const char* file, int line, double value,
    double expected, double tolerance, TestResult& test_result)
{
    if (fabs(value - expected) > tolerance)
    {
        FormatAssertMessage(test_result, file, line, "equality failed: %f != %f", value, expected);
        return false;
    }
    return true;
}
INLINE bool testing::test_assert_helper::AssertNotEqualFloat(const char* file, int line, double value,
    double expected, double tolerance, TestResult& test_result)
{
    if (fabs(value - expected) <= tolerance)
    {
        FormatAssertMessage(test_result, file, line, "inequality failed: %f == %f", value, expected);
        return false;
    }
    return true;
}

INLINE bool testing::test_assert_helper::AssertExpression(const char* file, int line, const char* expr, bool expr_result,
    TestResult& test_result)
{
    if (!expr_result)
    {
        FormatAssertMessage(test_result, file, line, "expression failed: %s", expr);
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------

#define TEST_CLASS_NAME(test_case_name) test_case_name##_Test

#define TEST_CASE(test_case_name) \
class TEST_CLASS_NAME(test_case_name) : protected testing::TestCase \
{ \
public: \
    TEST_CLASS_NAME(test_case_name)() : TestCase(#test_case_name) { \
    testing::test_framework::RegisterTest(this); \
    } \
    virtual void Run(testing::TestResult& _test_result); \
}; \
    static TEST_CLASS_NAME(test_case_name) s_test_##test_case_name; \
    void TEST_CLASS_NAME(test_case_name)::Run(testing::TestResult& _test_result) \



//-------------------------------------------------------------------------------

#define ASSERT_EXPR(expression) \
do { if (!testing::test_assert_helper::AssertExpression(__FILE__, __LINE__, #expression, expression, _test_result)) \
{ _test_result.result = testing::TestResult::FAILED; return; } } while (0)

#define ASSERT_EQUAL(value, expected) \
do { if (!testing::test_assert_helper::AssertEqual(__FILE__, __LINE__, value, expected, _test_result)) \
{ _test_result.result = testing::TestResult::FAILED; return; } } while (0) 

#define ASSERT_NOT_EQUAL(value, expected) \
do { if (!testing::test_assert_helper::AssertNotEqual(__FILE__, __LINE__, value, expected, _test_result)) \
{ _test_result.result = testing::TestResult::FAILED; return; } } while (0)  

#define ASSERT_EQUAL_F(value, expected, tolerance) \
do { if (!testing::test_assert_helper::AssertEqualFloat(__FILE__, __LINE__, value, expected, tolerance, _test_result)) \
{ _test_result.result = testing::TestResult::FAILED; return; } } while (0) 

#define ASSERT_NOT_EQUAL_F(value, expected, tolerance) \
do { if (!testing::test_assert_helper::AssertNotEqualFloat(__FILE__, __LINE__, value, expected, tolerance, _test_result)) \
{ _test_result.result = testing::TestResult::FAILED; return; } } while (0) 

#define ASSERT_EQUAL_STR(value, expected) \
do { if (!testing::test_assert_helper::AssertEqual<const char*, const char*>(__FILE__, __LINE__, value, expected, _test_result)) \
{ _test_result.result = testing::TestResult::FAILED; return; } } while (0) 

#define ASSERT_NOT_EQUAL_STR(value, expected) \
do { if (!testing::test_assert_helper::AssertNotEqual<const char*, const char*>(__FILE__, __LINE__, value, expected, _test_result)) \
{ _test_result.result = testing::TestResult::FAILED; return; } } while (0) 

//-------------------------------------------------------------------------------


#endif // __UNITTEST_FRAMEWORK_H__
