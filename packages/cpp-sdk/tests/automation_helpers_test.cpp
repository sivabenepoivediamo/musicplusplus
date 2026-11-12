#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/automations.h"

#include <stdexcept>
#include <vector>

namespace {

using musicpp::forwardVoiceLeading;
using musicpp::getMaxInterval;
using musicpp::normalizeComplexityVector;
using musicpp::voiceLeadingAutomationVectorReference;

using musicpp::position_vector;

TEST_CASE("normalize_complexity_vector_empty_fills_zero", "[automation]") {
    auto v = normalizeComplexityVector({}, 4);
    ASSERT_EQ(v, std::vector<int>({0, 0, 0, 0}));
}

TEST_CASE("normalize_complexity_vector_same_size_unchanged", "[automation]") {
    auto v = normalizeComplexityVector({1, 2, 3}, 3);
    ASSERT_EQ(v, std::vector<int>({1, 2, 3}));
}

TEST_CASE("normalize_complexity_vector_cycle_and_truncate", "[automation]") {
    auto c = normalizeComplexityVector({10, 20}, 5);
    ASSERT_EQ(c, std::vector<int>({10, 20, 10, 20, 10}));
    auto t = normalizeComplexityVector({1, 2, 3, 4, 5}, 2);
    ASSERT_EQ(t, std::vector<int>({1, 2}));
}

TEST_CASE("get_max_interval", "[automation]") {
    std::vector<int> one = {42};
    ASSERT_EQ(getMaxInterval(one), 0);
    std::vector<int> xs = {0, 5, 12, 13};
    ASSERT_EQ(getMaxInterval(xs), 7);
}

TEST_CASE("forward_voice_leading_empty_throws", "[automation]") {
    std::vector<position_vector> empty;
    REQUIRE_THROWS_AS(forwardVoiceLeading(empty), std::runtime_error);
}

TEST_CASE("forward_voice_leading_single_returns_copy", "[automation]") {
    std::vector<position_vector> one = {position_vector({0, 4, 7})};
    auto out = forwardVoiceLeading(one);
    ASSERT_EQ(out.size(), static_cast<size_t>(1));
    ASSERT_POSITION_VECTOR_EQ(out[0], musicpp_test::ints({0, 4, 7}), 12);
}

TEST_CASE("voice_leading_automation_vector_reference_size_mismatch", "[automation]") {
    std::vector<position_vector> t = {position_vector({0, 4, 7})};
    std::vector<position_vector> r;
    REQUIRE_THROWS_AS(voiceLeadingAutomationVectorReference(t, r, {}), std::runtime_error);
}

} // namespace
