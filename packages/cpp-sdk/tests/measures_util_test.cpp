#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/measures.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <vector>

namespace {

using namespace musicpp;

TEST_CASE("differences_empty_and_short", "[measures]") {
    std::vector<int> a = {5};
    ASSERT_TRUE(differences(a).empty());
    std::vector<int> b;
    ASSERT_TRUE(differences(b).empty());
    std::vector<int> c = {1, 4, 9};
    ASSERT_VECTOR_EQ(differences(c), musicpp_test::ints({3, 5}));
}

TEST_CASE("geodesic_distance_cycle", "[measures]") {
    ASSERT_EQ(geodesicDistance(0, 6, 12), 6);
    ASSERT_EQ(geodesicDistance(11, 1, 12), 2);
    ASSERT_EQ(geodesicDistance(5, 5, 12), 0);
}

TEST_CASE("geodesic_distances_pairing_order", "[measures]") {
    position_vector pv({0, 3, 7}, 12, 12);
    auto d = geodesicDistances(pv);
    ASSERT_EQ(d.size(), static_cast<size_t>(3));
    ASSERT_EQ(d[0], geodesicDistance(0, 3, 12));
    ASSERT_EQ(d[1], geodesicDistance(0, 7, 12));
    ASSERT_EQ(d[2], geodesicDistance(3, 7, 12));
}

TEST_CASE("calculate_occurrences", "[measures]") {
    std::vector<int> v = {1, 1, 2};
    auto m = calculateOccurrences(v);
    ASSERT_EQ(m[1], 2);
    ASSERT_EQ(m[2], 1);
    std::vector<int> empty;
    auto m0 = calculateOccurrences(empty);
    ASSERT_TRUE(m0.empty());
}

TEST_CASE("is_winograd_deep_and_erdos_deep", "[measures]") {
    std::map<int, int> good;
    good[1] = 3;
    good[2] = 1;
    good[3] = 2;
    ASSERT_TRUE(isWinogradDeep(good, 4));
    ASSERT_TRUE(isErdosDeep(good));

    std::map<int, int> bad_dup_freq;
    bad_dup_freq[1] = 2;
    bad_dup_freq[2] = 2;
    ASSERT_TRUE(!isErdosDeep(bad_dup_freq));
}

TEST_CASE("calculate_regression_evenness", "[measures]") {
    std::vector<int> rhythm = {0, 3, 6};
    double e = calculateRegressionEvenness(rhythm, 9);
    ASSERT_NEAR(e, 0.0, 1e-9);
}

TEST_CASE("calculate_regression_evenness_empty_rhythm", "[measures][safety]") {
    std::vector<int> empty;
    ASSERT_NEAR(calculateRegressionEvenness(empty, 12), 0.0, 1e-12);
}

TEST_CASE("is_balanced_two_antipodal_notes", "[measures]") {
    position_vector pv({0, 6}, 12, 12);
    ASSERT_TRUE(isBalanced(pv));
}

TEST_CASE("is_prime", "[measures]") {
    ASSERT_TRUE(!isPrime(0));
    ASSERT_TRUE(!isPrime(1));
    ASSERT_TRUE(isPrime(2));
    ASSERT_TRUE(isPrime(17));
    ASSERT_TRUE(!isPrime(18));
}

TEST_CASE("generate_sequence_mod", "[measures]") {
    auto seq = generate(2, 5, 7, false);
    ASSERT_VECTOR_EQ(seq, musicpp_test::ints({0, 2, 4, 6, 1}));
}

TEST_CASE("is_generated_finds_multiplier", "[measures]") {
    std::vector<int> g = generate(1, 5, 5, false);
    auto p = isGenerated(g, 5);
    ASSERT_TRUE(p.first);
    ASSERT_EQ(p.second, 1);
}

TEST_CASE("is_euclidean_empty_intervals_is_false", "[measures]") {
    position_vector one({0}, 12, 12);
    ASSERT_TRUE(!isEuclidean(one, 12));
}

TEST_CASE("compute_entropy_counts_slot_indices", "[measures]") {
    position_vector sparse({0}, 12, 12);
    position_vector dense({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, 12, 12);
    double h1 = computeEntropy(sparse);
    double h2 = computeEntropy(dense);
    ASSERT_NEAR(h1, 0.413817, 1e-5);
    ASSERT_NEAR(h2, 0.0, 1e-9);
}

TEST_CASE("compute_longest_subsequence_and_transition_complexity", "[measures]") {
    position_vector pv({0, 2, 4}, 12, 12);
    ASSERT_EQ(computeLongestSubsequence(pv), 7);
    int tc = computeTransitionComplexity(pv, 12);
    ASSERT_TRUE(tc > 0);
}

TEST_CASE("calculate_distribution_spectra_major_third", "[measures]") {
    position_vector major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    auto spectra = calculateDistributionSpectra(major);
    ASSERT_EQ(spectra.size(), static_cast<size_t>(6));
    std::set<int> third = spectra[2];
    ASSERT_TRUE(third.count(5));
    ASSERT_TRUE(third.count(6));
}

TEST_CASE("calculate_spectrum_widths_and_variation", "[measures]") {
    std::vector<std::set<int>> spectra = {{1, 3}, {}};
    auto w = calculateSpectrumWidths(spectra);
    ASSERT_EQ(w[0], 2);
    ASSERT_EQ(w[1], 0);
    ASSERT_NEAR(calculateSpectrumVariation(w, 7), 2.0 / 7.0, 1e-9);
}

TEST_CASE("augmented_triad_is_balanced_mod_twelve", "[measures]") {
    position_vector aug({0, 4, 8}, 12, 12);
    ASSERT_TRUE(isBalanced(aug));
}

TEST_CASE("whole_tone_has_rotational_symmetry_two", "[measures]") {
    position_vector wt({0, 2, 4, 6, 8, 10}, 12, 12);
    auto axes = findRotationalSymmetryAxes(wt);
    ASSERT_TRUE(std::find(axes.begin(), axes.end(), 2) != axes.end());
}

TEST_CASE("tritone_pair_palindrome_axis_zero", "[measures]") {
    position_vector tritone({0, 6}, 12, 12);
    ASSERT_TRUE(isPalindrome(tritone));
}

TEST_CASE("diminished_seventh_not_chiral", "[measures]") {
    position_vector dim({0, 3, 6, 9}, 12, 12);
    ASSERT_TRUE(!isChiral(dim));
}

TEST_CASE("calculate_rhythmic_oddity_singleton", "[measures]") {
    position_vector one({0}, 12, 12);
    ASSERT_EQ(calculateRhythmicOddity(one), 0);
}

} // namespace
