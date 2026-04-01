#ifndef MUSICPP_TEST_SUPPORT_H
#define MUSICPP_TEST_SUPPORT_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <utility>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../src/Vector.h"

namespace musicpp_test {

struct AssertionError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

template <typename T>
std::string debug_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

inline std::string debug_string(const std::string& value) {
    return value;
}

inline std::string debug_string(const char* value) {
    return value == nullptr ? "(null)" : std::string(value);
}

template <typename First, typename Second>
std::string debug_string(const std::pair<First, Second>& value) {
    std::ostringstream oss;
    oss << "(" << debug_string(value.first) << ", " << debug_string(value.second) << ")";
    return oss.str();
}

template <typename T>
std::string debug_string(const std::vector<T>& values) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << debug_string(values[i]);
    }
    oss << "]";
    return oss.str();
}

template <typename Key, typename Value>
std::string debug_string(const std::map<Key, Value>& values) {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& entry : values) {
        if (!first) {
            oss << ", ";
        }
        first = false;
        oss << debug_string(entry.first) << ": " << debug_string(entry.second);
    }
    oss << "}";
    return oss.str();
}

inline std::string debug_string(const PositionVector& value) {
    std::ostringstream oss;
    oss << "{data=" << debug_string(value.getData())
        << ", mod=" << value.getMod()
        << ", range=" << value.getRange() << "}";
    return oss.str();
}

inline std::string debug_string(const IntervalVector& value) {
    std::ostringstream oss;
    oss << "{data=" << debug_string(value.getData())
        << ", offset=" << value.getOffset()
        << ", mod=" << value.getMod() << "}";
    return oss.str();
}

inline std::string debug_string(const BinaryVector& value) {
    std::ostringstream oss;
    oss << "{data=" << debug_string(value.getData())
        << ", offset=" << value.getOffset()
        << ", mod=" << value.getMod() << "}";
    return oss.str();
}

inline std::vector<int> ints(std::initializer_list<int> values) {
    return std::vector<int>(values);
}

inline std::vector<std::string> strings(std::initializer_list<std::string> values) {
    return std::vector<std::string>(values);
}

inline void fail(const char* file, int line, const std::string& message) {
    std::ostringstream oss;
    oss << file << ":" << line << ": " << message;
    throw AssertionError(oss.str());
}

inline bool verbose_enabled() {
    const char* value = std::getenv("MUSICPP_TEST_VERBOSE");
    if (value == nullptr) {
        return false;
    }
    return value[0] != '\0' && std::string(value) != "0" && std::string(value) != "false";
}

template <typename T>
void log_value_impl(const char* label, const T& value) {
    if (verbose_enabled()) {
        std::cout << label << ": " << debug_string(value) << "\n";
    }
}

inline void log_line_impl(const char* label) {
    if (verbose_enabled()) {
        std::cout << label << "\n";
    }
}

inline void log_case_impl(const char* label) {
    if (verbose_enabled()) {
        std::cout << "  case: " << label << "\n";
    }
}

template <typename T>
void log_input_impl(const char* label, const T& value) {
    if (verbose_enabled()) {
        std::cout << "    in." << label << ": " << debug_string(value) << "\n";
    }
}

template <typename T>
void log_output_impl(const char* label, const T& value) {
    if (verbose_enabled()) {
        std::cout << "    out." << label << ": " << debug_string(value) << "\n";
    }
}

template <typename Actual, typename Expected>
void assert_eq_impl(
    const Actual& actual,
    const Expected& expected,
    const char* actual_expr,
    const char* expected_expr,
    const char* file,
    int line) {
    if (!(actual == expected)) {
        std::ostringstream oss;
        oss << "ASSERT_EQ failed for " << actual_expr << " and " << expected_expr
            << " | actual=" << debug_string(actual)
            << " expected=" << debug_string(expected);
        fail(file, line, oss.str());
    }
}

inline void assert_true_impl(bool condition, const char* expr, const char* file, int line) {
    if (!condition) {
        std::ostringstream oss;
        oss << "ASSERT_TRUE failed for " << expr;
        fail(file, line, oss.str());
    }
}

inline void assert_near_impl(
    double actual,
    double expected,
    double tolerance,
    const char* actual_expr,
    const char* expected_expr,
    const char* file,
    int line) {
    if (std::fabs(actual - expected) > tolerance) {
        std::ostringstream oss;
        oss << "ASSERT_NEAR failed for " << actual_expr << " and " << expected_expr
            << " | actual=" << actual
            << " expected=" << expected
            << " tolerance=" << tolerance;
        fail(file, line, oss.str());
    }
}

inline void assert_vector_eq_impl(
    const std::vector<int>& actual,
    const std::vector<int>& expected,
    const char* actual_expr,
    const char* expected_expr,
    const char* file,
    int line) {
    assert_eq_impl(actual, expected, actual_expr, expected_expr, file, line);
}

inline void assert_string_vector_eq_impl(
    const std::vector<std::string>& actual,
    const std::vector<std::string>& expected,
    const char* actual_expr,
    const char* expected_expr,
    const char* file,
    int line) {
    assert_eq_impl(actual, expected, actual_expr, expected_expr, file, line);
}

inline void assert_position_vector_eq_impl(
    const PositionVector& actual,
    const std::vector<int>& expected,
    int expected_mod,
    const char* actual_expr,
    const char* expected_expr,
    const char* file,
    int line) {
    assert_vector_eq_impl(actual.getData(), expected, actual_expr, expected_expr, file, line);
    if (expected_mod >= 0) {
        assert_eq_impl(actual.getMod(), expected_mod, "actual.getMod()", "expected_mod", file, line);
    }
}

inline void assert_interval_vector_eq_impl(
    const IntervalVector& actual,
    const std::vector<int>& expected,
    int expected_offset,
    int expected_mod,
    const char* actual_expr,
    const char* expected_expr,
    const char* file,
    int line) {
    assert_vector_eq_impl(actual.getData(), expected, actual_expr, expected_expr, file, line);
    assert_eq_impl(actual.getOffset(), expected_offset, "actual.getOffset()", "expected_offset", file, line);
    if (expected_mod >= 0) {
        assert_eq_impl(actual.getMod(), expected_mod, "actual.getMod()", "expected_mod", file, line);
    }
}

inline void assert_binary_vector_eq_impl(
    const BinaryVector& actual,
    const std::vector<int>& expected,
    int expected_offset,
    int expected_mod,
    const char* actual_expr,
    const char* expected_expr,
    const char* file,
    int line) {
    assert_vector_eq_impl(actual.getData(), expected, actual_expr, expected_expr, file, line);
    assert_eq_impl(actual.getOffset(), expected_offset, "actual.getOffset()", "expected_offset", file, line);
    if (expected_mod >= 0) {
        assert_eq_impl(actual.getMod(), expected_mod, "actual.getMod()", "expected_mod", file, line);
    }
}

} // namespace musicpp_test

#define ASSERT_TRUE(expr) \
    ::musicpp_test::assert_true_impl((expr), #expr, __FILE__, __LINE__)

#define ASSERT_EQ(actual, expected) \
    ::musicpp_test::assert_eq_impl((actual), (expected), #actual, #expected, __FILE__, __LINE__)

#define ASSERT_NEAR(actual, expected, tolerance) \
    ::musicpp_test::assert_near_impl((actual), (expected), (tolerance), #actual, #expected, __FILE__, __LINE__)

#define ASSERT_VECTOR_EQ(actual, expected) \
    ::musicpp_test::assert_vector_eq_impl((actual), (expected), #actual, #expected, __FILE__, __LINE__)

#define ASSERT_STRING_VECTOR_EQ(actual, expected) \
    ::musicpp_test::assert_string_vector_eq_impl((actual), (expected), #actual, #expected, __FILE__, __LINE__)

#define ASSERT_POSITION_VECTOR_EQ(actual, expected, expected_mod) \
    ::musicpp_test::assert_position_vector_eq_impl((actual), (expected), (expected_mod), #actual, #expected, __FILE__, __LINE__)

#define ASSERT_INTERVAL_VECTOR_EQ(actual, expected, expected_offset, expected_mod) \
    ::musicpp_test::assert_interval_vector_eq_impl((actual), (expected), (expected_offset), (expected_mod), #actual, #expected, __FILE__, __LINE__)

#define ASSERT_BINARY_VECTOR_EQ(actual, expected, expected_offset, expected_mod) \
    ::musicpp_test::assert_binary_vector_eq_impl((actual), (expected), (expected_offset), (expected_mod), #actual, #expected, __FILE__, __LINE__)

#define TEST_LOG(label, value) \
    ::musicpp_test::log_value_impl((label), (value))

#define TEST_LOG_LINE(label) \
    ::musicpp_test::log_line_impl((label))

#define TEST_CASE_LOG(label) \
    ::musicpp_test::log_case_impl((label))

#define TEST_INPUT(label, value) \
    ::musicpp_test::log_input_impl((label), (value))

#define TEST_OUTPUT(label, value) \
    ::musicpp_test::log_output_impl((label), (value))

#endif
