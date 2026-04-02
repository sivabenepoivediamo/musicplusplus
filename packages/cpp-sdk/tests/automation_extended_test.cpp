#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/automations.h"

namespace {

using musicpp::manhattanDistance;

using musicpp::autoScale;
using musicpp::degreeAutomationSequentialBackward;
using musicpp::degreeAutomationVectorReference;
using musicpp::forwardDegreeAutomation;
using musicpp::interval_vector;
using musicpp::normalizeComplexityVector;
using musicpp::position_vector;
using musicpp::voiceLeadingAutomation;
using musicpp::voiceLeadingAutomationSequentialBackward;
using musicpp::voiceLeadingAutomationVectorReference;

TEST_CASE("normalize_complexity_vector_single_repeats", "[automation]") {
    auto v = normalizeComplexityVector({7}, 4);
    ASSERT_EQ(v, std::vector<int>({7, 7, 7, 7}));
}

TEST_CASE("forward_degree_automation_empty_degrees_throws", "[automation]") {
    position_vector scale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    interval_vector crit({2, 2, 3}, 0, 12);
    position_vector seed({0, 4, 7}, 12, 12);
    std::vector<int> degrees;
    REQUIRE_THROWS_AS(forwardDegreeAutomation(scale, crit, degrees, seed, {}), std::runtime_error);
}

TEST_CASE("degree_automation_sequential_backward_empty_throws", "[automation]") {
    position_vector scale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    interval_vector crit({2, 2, 3}, 0, 12);
    position_vector seed({0, 4, 7}, 12, 12);
    std::vector<int> degrees;
    REQUIRE_THROWS_AS(degreeAutomationSequentialBackward(scale, crit, degrees, seed, {}), std::runtime_error);
}

TEST_CASE("voice_leading_sequential_backward_empty_throws", "[automation]") {
    std::vector<position_vector> empty;
    REQUIRE_THROWS_AS(voiceLeadingAutomationSequentialBackward(empty, {}), std::runtime_error);
}

TEST_CASE("degree_automation_vector_reference_size_mismatch_throws", "[automation]") {
    position_vector scale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    interval_vector crit({2, 2, 3}, 0, 12);
    std::vector<int> degrees = {0, 1};
    std::vector<position_vector> refs = {position_vector({0, 4, 7})};
    REQUIRE_THROWS_AS(degreeAutomationVectorReference(scale, crit, degrees, refs, {}), std::runtime_error);
}

TEST_CASE("degree_automation_vector_reference_two_degrees", "[automation]") {
    position_vector scale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    interval_vector crit({2, 2, 3}, 0, 12);
    std::vector<int> degrees = {0, 1};
    std::vector<position_vector> refs = {
        position_vector({0, 4, 7}, 12, 12),
        position_vector({2, 5, 9}, 12, 12),
    };
    auto out = degreeAutomationVectorReference(scale, crit, degrees, refs, {0, 0});
    ASSERT_EQ(out.size(), static_cast<size_t>(2));
    ASSERT_EQ(out[0].mod(), 12);
    ASSERT_EQ(out[1].mod(), 12);
    ASSERT_EQ(out[0].size(), static_cast<size_t>(3));
    ASSERT_EQ(out[1].size(), static_cast<size_t>(3));
}

TEST_CASE("voice_leading_automation_identical_ref_target_zero_distance", "[automation]") {
    position_vector chord({0, 4, 7}, 12, 12);
    position_vector copy = chord;
    auto row = voiceLeadingAutomation(chord, copy, 0);
    ASSERT_NEAR(row.getDistance(), 0.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(row.getVector(), musicpp_test::ints({0, 4, 7}), 12);
}

TEST_CASE("voice_leading_automation_vector_reference_two_chords", "[automation]") {
    std::vector<position_vector> targets = {
        position_vector({0, 4, 7}, 12, 12),
        position_vector({2, 5, 9}, 12, 12),
    };
    std::vector<position_vector> refs = {
        position_vector({0, 4, 7}, 12, 12),
        position_vector({2, 5, 9}, 12, 12),
    };
    auto out = voiceLeadingAutomationVectorReference(targets, refs, {});
    ASSERT_EQ(out.size(), static_cast<size_t>(2));
    ASSERT_EQ(manhattanDistance(out[0], targets[0]), 0);
    ASSERT_EQ(manhattanDistance(out[1], targets[1]), 0);
}

TEST_CASE("auto_scale_preserves_length_and_mod", "[automation]") {
    position_vector scale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    std::vector<int> notes = {60, 64, 67};
    position_vector out = autoScale(scale, notes);
    ASSERT_EQ(out.size(), scale.size());
    ASSERT_EQ(out.mod(), scale.mod());
}

TEST_CASE("auto_scale_single_note_no_throw", "[automation]") {
    position_vector scale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    std::vector<int> notes = {12};
    position_vector out = autoScale(scale, notes);
    ASSERT_EQ(out.data().size(), scale.data().size());
}

} // namespace
