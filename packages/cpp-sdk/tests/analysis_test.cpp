#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/distances.h"
#include "../src/measures.h"

namespace {

using namespace musicpp;

TEST_CASE("distance_examples", "[analysis]") {
    position_vector a({0, 4, 7, 11});
    position_vector b({7, 11, 14, 17});
    auto euclidean = euclideanDistance(a, b);
    auto manhattan = manhattanDistance(a, b);
    auto edit = editDistance(a, b);
    auto weighted = weightedTransformationDistance(a, b);
    auto diff = difference(a, b);
    auto hamming = hammingDistance(a, b);

    ASSERT_NEAR(euclidean, 13.5277, 1e-4);
    ASSERT_EQ(manhattan, 27);
    ASSERT_EQ(edit, 4);
    ASSERT_EQ(weighted, 27);
    ASSERT_EQ(diff, -27);
    ASSERT_EQ(hamming, 4);

    auto steps = transformationSteps(a.data(), b.data());
    ASSERT_EQ(steps.size(), static_cast<size_t>(4));
    ASSERT_EQ(steps[0].first, 0);
    ASSERT_EQ(steps[0].second.first, 0);
    ASSERT_EQ(steps[0].second.second, 7);
    ASSERT_EQ(steps[1].first, 0);
    ASSERT_EQ(steps[1].second.first, 1);
    ASSERT_EQ(steps[1].second.second, 7);
    ASSERT_EQ(steps[2].first, 0);
    ASSERT_EQ(steps[2].second.first, 2);
    ASSERT_EQ(steps[2].second.second, 7);
    ASSERT_EQ(steps[3].first, 0);
    ASSERT_EQ(steps[3].second.first, 3);
    ASSERT_EQ(steps[3].second.second, 6);

    TEST_CASE_LOG("quartal_distance_metrics");
    TEST_INPUT("a", a);
    TEST_INPUT("b", b);
    TEST_OUTPUT("euclidean", euclidean);
    TEST_OUTPUT("manhattan", manhattan);
    TEST_OUTPUT("edit", edit);
    TEST_OUTPUT("weighted", weighted);
    TEST_OUTPUT("difference", diff);
    TEST_OUTPUT("hamming", hamming);
    TEST_OUTPUT("steps", steps);
}

TEST_CASE("measure_examples", "[analysis]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    auto distances = geodesicDistances(c_major);
    auto occurrences = calculateOccurrences(distances);
    auto spectra = calculateDistributionSpectra(c_major);
    auto widths = calculateSpectrumWidths(spectra);
    auto rotational_axes = findRotationalSymmetryAxes(c_major);
    auto reflective_axes = findReflectiveSymmetryAxes(c_major);
    auto raw_positions = c_major.data();

    ASSERT_EQ(distances.size(), static_cast<size_t>(21));
    ASSERT_EQ(occurrences[1], 2);
    ASSERT_EQ(occurrences[2], 5);
    ASSERT_EQ(occurrences[3], 4);
    ASSERT_EQ(occurrences[4], 3);
    ASSERT_EQ(occurrences[5], 6);
    ASSERT_EQ(occurrences[6], 1);
    ASSERT_TRUE(isWinogradDeep(occurrences, static_cast<int>(c_major.size())));
    ASSERT_TRUE(isErdosDeep(occurrences));

    ASSERT_NEAR(calculateRegressionEvenness(raw_positions, c_major.effective_range()), 2.28571, 1e-5);
    ASSERT_EQ(calculateRhythmicOddity(c_major), 1);
    ASSERT_NEAR(computeEntropy(c_major), 0.979869, 1e-5);
    ASSERT_EQ(computeLongestSubsequence(c_major), 2);
    ASSERT_TRUE(!isEuclidean(c_major, c_major.effective_range()));

    ASSERT_EQ(widths, std::vector<int>({1, 1, 1, 1, 1, 1}));
    ASSERT_NEAR(calculateSpectrumVariation(widths, static_cast<int>(c_major.size())), 0.857143, 1e-6);
    ASSERT_EQ(rotational_axes.size(), static_cast<size_t>(0));
    ASSERT_EQ(reflective_axes.size(), static_cast<size_t>(2));
    ASSERT_NEAR(reflective_axes[0], 2.0, 1e-6);
    ASSERT_NEAR(reflective_axes[1], 8.0, 1e-6);

    ASSERT_EQ(computeTransitionComplexity(c_major, c_major.effective_range()), 10);
    ASSERT_TRUE(!isPalindrome(c_major));
    ASSERT_TRUE(!isChiral(c_major));
    ASSERT_TRUE(!isBalanced(c_major));
    ASSERT_EQ(isGenerated(raw_positions, c_major.effective_range()), std::make_pair(false, -1));

    TEST_CASE_LOG("c_major_measure_profile");
    TEST_INPUT("positions", c_major);
    TEST_OUTPUT("distances", distances);
    TEST_OUTPUT("occurrences", occurrences);
    TEST_OUTPUT("widths", widths);
    TEST_OUTPUT("reflective_axes", reflective_axes);
    TEST_OUTPUT("rotational_axes", rotational_axes);
    TEST_OUTPUT("regression_evenness", calculateRegressionEvenness(raw_positions, c_major.effective_range()));
    TEST_OUTPUT("rhythmic_oddity", calculateRhythmicOddity(c_major));
    TEST_OUTPUT("entropy", computeEntropy(c_major));
    TEST_OUTPUT("longest_subsequence", computeLongestSubsequence(c_major));
    TEST_OUTPUT("transition_complexity", computeTransitionComplexity(c_major, c_major.effective_range()));
    TEST_OUTPUT("balanced", isBalanced(c_major));
    TEST_OUTPUT("generated", isGenerated(raw_positions, c_major.effective_range()));
}

TEST_CASE("measure_false_cases", "[analysis]") {
    position_vector asymmetrical({0, 1, 4, 6, 9}, 12, 12);
    auto distances = geodesicDistances(asymmetrical);
    auto occurrences = calculateOccurrences(distances);
    auto rotational_axes = findRotationalSymmetryAxes(asymmetrical);
    auto reflective_axes = findReflectiveSymmetryAxes(asymmetrical);
    auto raw_positions = asymmetrical.data();

    ASSERT_TRUE(!isWinogradDeep(occurrences, static_cast<int>(asymmetrical.size())));
    ASSERT_TRUE(!isErdosDeep(occurrences));
    ASSERT_EQ(rotational_axes.size(), static_cast<size_t>(0));
    ASSERT_EQ(reflective_axes.size(), static_cast<size_t>(0));
    ASSERT_TRUE(!isBalanced(asymmetrical));
    ASSERT_EQ(isGenerated(raw_positions, asymmetrical.effective_range()), std::make_pair(false, -1));
    ASSERT_TRUE(!isEuclidean(asymmetrical, asymmetrical.effective_range()));
    ASSERT_TRUE(!isPalindrome(asymmetrical));
    ASSERT_TRUE(isChiral(asymmetrical));
    ASSERT_EQ(computeTransitionComplexity(asymmetrical, asymmetrical.effective_range()), 8);

    TEST_CASE_LOG("asymmetrical_negative_profile");
    TEST_INPUT("positions", asymmetrical);
    TEST_OUTPUT("distances", distances);
    TEST_OUTPUT("occurrences", occurrences);
    TEST_OUTPUT("reflective_axes", reflective_axes);
    TEST_OUTPUT("rotational_axes", rotational_axes);
    TEST_OUTPUT("balanced", isBalanced(asymmetrical));
    TEST_OUTPUT("generated", isGenerated(raw_positions, asymmetrical.effective_range()));
    TEST_OUTPUT("euclidean", isEuclidean(asymmetrical, asymmetrical.effective_range()));
    TEST_OUTPUT("palindrome", isPalindrome(asymmetrical));
    TEST_OUTPUT("chiral", isChiral(asymmetrical));
    TEST_OUTPUT("transition_complexity", computeTransitionComplexity(asymmetrical, asymmetrical.effective_range()));
}

} // namespace
