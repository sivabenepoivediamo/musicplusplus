#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/distances.h"
#include "../src/matrix.h"
#include "../src/matrix_distance.h"

namespace {

using namespace musicpp;

TEST_CASE("calculate_distances_modal_interval_euclidean", "[matrix][distance]") {
    interval_vector ref({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    interval_vector other({2, 2, 2, 1, 2, 2, 1}, 0, 12);
    auto modes = modalMatrix(ref);
    auto mmd = calculateDistances(other, modes, manhattanDistance, true);
    ASSERT_TRUE(!mmd.empty());
    auto closest = mmd.getClosest();
    ASSERT_NEAR(std::get<2>(closest), 0.0, 1e-6);
    ASSERT_EQ(std::get<1>(closest), 3);
}

TEST_CASE("calculate_distances_modal_position_sort_flag", "[matrix][distance]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11});
    position_vector c_lydian({0, 2, 4, 6, 7, 9, 11});
    auto modes = modalMatrix(c_major);
    auto sorted = calculateDistances(c_lydian, modes, manhattanDistance, true);
    auto unsorted = calculateDistances(c_lydian, modes, manhattanDistance, false);
    ASSERT_EQ(sorted.size(), unsorted.size());
    ASSERT_NEAR(std::get<2>(sorted.getClosest()), 0.0, 1e-6);
    double first_row_dist = std::get<2>(unsorted[0]);
    ASSERT_TRUE(first_row_dist > 1e-6);
    ASSERT_NEAR(std::get<2>(sorted[0]), std::get<2>(sorted.getClosest()), 1e-9);
}

TEST_CASE("calculate_distances_transposition_matrix_closest", "[matrix][distance]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11});
    position_vector c_lydian({0, 2, 4, 6, 7, 9, 11});
    auto tmd = calculateDistances(c_lydian, transpositionMatrix(c_major), manhattanDistance, true);
    ASSERT_TRUE(!tmd.empty());
    auto best = tmd.getClosest();
    ASSERT_NEAR(best.getDistance(), 0.0, 1e-6);
    ASSERT_EQ(best.getTransposition(), 7);
}

TEST_CASE("align_chord_pair", "[matrix][distance]") {
    position_vector c_major_chord({0, 4, 7});
    position_vector g_major_chord({7, 11, 14});
    ASSERT_EQ(align(c_major_chord, g_major_chord), -2);
}

TEST_CASE("calculate_distances_modal_relative_mode_closest", "[matrix][distance]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11});
    interval_vector crit({2, 2, 3});
    position_vector g_major_chord({7, 11, 14});
    auto sel = modalSelection(c_major, crit, 0);
    auto mrmd = calculateDistances(g_major_chord, modal_relative_mode(sel), manhattanDistance, true);
    auto best = mrmd.getClosest();
    ASSERT_EQ(best.getModeIndex(), 0);
    ASSERT_EQ(best.get_relative_mode_index(), 2);
    ASSERT_NEAR(best.getDistance(), 3.0, 1e-6);
}

TEST_CASE("calculate_distances_modal_selection_closest", "[matrix][distance]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11});
    interval_vector crit({2, 2, 3});
    position_vector g_major_chord({7, 11, 14});
    auto sel = modalSelection(c_major, crit, 0);
    auto msd = calculateDistances(g_major_chord, sel, manhattanDistance, true);
    auto best = msd.getClosest();
    ASSERT_EQ(best.getModeIndex(), 3);
    ASSERT_NEAR(best.getDistance(), 18.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(best.getChord(), musicpp_test::ints({0, 5, 9}), 12);
}

} // namespace
