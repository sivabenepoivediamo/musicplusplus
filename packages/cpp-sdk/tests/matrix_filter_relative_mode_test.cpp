#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/matrix.h"
#include "../src/matrix_distance.h"

namespace {

using namespace musicpp;

TEST_CASE("relative_mode_matrix_size_and_center", "[matrix][relative_mode]") {
    position_vector triad({0, 4, 7}, 12, 12);
    position_vector copy = triad;
    auto rtm = relative_mode_matrix(copy, 2);
    ASSERT_EQ(rtm.getCenter(), 2);
    ASSERT_EQ(rtm.size(), static_cast<size_t>(2 * static_cast<int>(triad.size()) + 1));
}

TEST_CASE("calculate_distances_relative_mode_closest", "[matrix][relative_mode]") {
    position_vector target({0, 4, 7}, 12, 12);
    position_vector mutable_target = target;
    auto rtm = relative_mode_matrix(mutable_target, 0);
    ASSERT_TRUE(rtm.size() > static_cast<size_t>(0));
    position_vector reference = rtm[rtm.size() / 2].first;
    auto rmd = calculateDistances(reference, rtm, manhattanDistance, true);
    ASSERT_TRUE(!rmd.empty());
    auto best = rmd.getClosest();
    ASSERT_NEAR(best.getDistance(), 0.0, 1e-6);
    ASSERT_EQ(rtm.getCenter(), best.getCenter());
}

TEST_CASE("relative_mode_matrix_distance_empty_throws", "[matrix][distance]") {
    RelativeModeMatrixDistance empty;
    REQUIRE_THROWS_AS(empty.getClosest(), std::runtime_error);
    REQUIRE_THROWS_AS(empty.getFurthest(), std::runtime_error);
}

TEST_CASE("filter_modal_matrix_empty_notes_noop", "[matrix][filter]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    auto modes = modalMatrix(c_major);
    ASSERT_EQ(modes.size(), static_cast<size_t>(7));
    auto same = filterModalMatrix(modes, std::vector<int>{});
    ASSERT_EQ(same.size(), modes.size());
}

TEST_CASE("filter_modal_matrix_impossible_pitch_set", "[matrix][filter]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    auto modes = modalMatrix(c_major);
    auto filtered = filterModalMatrix(modes, std::vector<int>{0, 1, 2});
    ASSERT_TRUE(filtered.empty());
}

TEST_CASE("filter_modal_matrix_single_required_note", "[matrix][filter]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    auto modes = modalMatrix(c_major);
    auto filtered = filterModalMatrix(modes, std::vector<int>{0});
    ASSERT_EQ(filtered.size(), modes.size());
}

TEST_CASE("filter_modal_matrix_major_triad_subset", "[matrix][filter]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    auto modes = modalMatrix(c_major);
    auto filtered = filterModalMatrix(modes, std::vector<int>{0, 4, 7});
    ASSERT_TRUE(filtered.size() < modes.size());
    ASSERT_TRUE(filtered.size() >= static_cast<size_t>(1));
}

TEST_CASE("filter_transposition_matrix_impossible_cluster", "[matrix][filter]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    auto tm = transpositionMatrix(c_major);
    ASSERT_TRUE(tm.size() >= static_cast<size_t>(1));
    auto filtered = filterTranspositionMatrix(tm, std::vector<int>{0, 1, 2});
    ASSERT_TRUE(filtered.empty());
}

TEST_CASE("filter_modal_matrix_in_place", "[matrix][filter]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    ModalMatrix<position_vector> modes = modalMatrix(c_major);
    const size_t full = modes.size();
    filterModalMatrixInPlace(modes, std::vector<int>{0, 1, 2});
    ASSERT_TRUE(modes.empty());
    ASSERT_TRUE(full > static_cast<size_t>(0));
}

TEST_CASE("filter_transposition_matrix_in_place", "[matrix][filter]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    TranspositionMatrix tm = transpositionMatrix(c_major);
    const size_t full = tm.size();
    filterTranspositionMatrixInPlace(tm, std::vector<int>{0, 1, 2});
    ASSERT_TRUE(tm.empty());
    ASSERT_TRUE(full > static_cast<size_t>(0));
}

TEST_CASE("calculate_distances_modal_selection_interval_reference", "[matrix][distance]") {
    interval_vector source({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    interval_vector crit({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    auto sel = modalSelection(source, crit, 0);
    ASSERT_TRUE(!sel.empty());
    interval_vector ref = sel[sel.size() / 2].first;
    auto msd = calculateDistances(ref, sel, manhattanDistance, true);
    ASSERT_TRUE(!msd.empty());
    ASSERT_NEAR(msd.getClosest().getDistance(), 0.0, 1e-6);
}

} // namespace
