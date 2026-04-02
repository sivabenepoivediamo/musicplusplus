#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/matrix_distance.h"

namespace {

using namespace musicpp;

TEST_CASE("matrix_generation_examples", "[matrix]") {
    interval_vector major_scale_intervals({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    auto interval_modes = modalMatrix(major_scale_intervals);
    ASSERT_EQ(interval_modes.size(), static_cast<size_t>(7));
    ASSERT_INTERVAL_VECTOR_EQ(interval_modes[0].first, musicpp_test::ints({2, 2, 1, 2, 2, 2, 1}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(interval_modes[3].first, musicpp_test::ints({2, 2, 2, 1, 2, 2, 1}), 0, 12);
    ASSERT_EQ(interval_modes.getIndices(), std::vector<int>({0, 1, 2, 3, 4, 5, 6}));

    position_vector c_major({0, 2, 4, 5, 7, 9, 11});
    auto position_modes = modalMatrix(c_major);
    ASSERT_EQ(position_modes.size(), static_cast<size_t>(7));
    ASSERT_POSITION_VECTOR_EQ(position_modes[0].first, musicpp_test::ints({0, 2, 4, 5, 7, 9, 11}), 12);
    ASSERT_POSITION_VECTOR_EQ(position_modes[3].first, musicpp_test::ints({0, 2, 4, 6, 7, 9, 11}), 12);

    auto transpositions = transpositionMatrix(c_major);
    ASSERT_EQ(transpositions.size(), static_cast<size_t>(12));
    ASSERT_POSITION_VECTOR_EQ(transpositions[0].first, musicpp_test::ints({0, 2, 4, 5, 7, 9, 11}), 12);
    ASSERT_POSITION_VECTOR_EQ(transpositions[7].first, musicpp_test::ints({0, 2, 4, 6, 7, 9, 11}), 12);

    position_vector triad({0, 4, 7});
    auto rel_mode_matrix = relative_mode_matrix(triad, 0);
    ASSERT_EQ(rel_mode_matrix.getCenter(), 0);
    ASSERT_EQ(rel_mode_matrix.size(), static_cast<size_t>(7));
    ASSERT_POSITION_VECTOR_EQ(rel_mode_matrix[0].first, musicpp_test::ints({-12, -8, -5}), 12);
    ASSERT_POSITION_VECTOR_EQ(rel_mode_matrix[3].first, musicpp_test::ints({0, 4, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(rel_mode_matrix[6].first, musicpp_test::ints({12, 16, 19}), 12);

    TEST_CASE_LOG("matrix_generation");
    TEST_INPUT("intervals", major_scale_intervals);
    TEST_INPUT("positions", c_major);
    TEST_INPUT("triad", triad);
    TEST_OUTPUT("interval_mode_0", interval_modes[0].first);
    TEST_OUTPUT("interval_mode_3", interval_modes[3].first);
    TEST_OUTPUT("position_mode_0", position_modes[0].first);
    TEST_OUTPUT("position_mode_3", position_modes[3].first);
    TEST_OUTPUT("transposition_0", transpositions[0].first);
    TEST_OUTPUT("transposition_7", transpositions[7].first);
    TEST_OUTPUT("relative_mode_row_0", rel_mode_matrix[0].first);
    TEST_OUTPUT("relative_mode_row_3", rel_mode_matrix[3].first);
    TEST_OUTPUT("relative_mode_row_6", rel_mode_matrix[6].first);
}

TEST_CASE("matrix_selection_and_filter_examples", "[matrix]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11});
    interval_vector criterion({2, 2, 3}, 0, 12);
    auto interval_selection = modalSelection(positions_to_intervals(c_major), criterion, 0);
    ASSERT_EQ(interval_selection.size(), static_cast<size_t>(3));
    ASSERT_INTERVAL_VECTOR_EQ(interval_selection[0].first, musicpp_test::ints({4, 3, 5}), 0, 12);
    ASSERT_EQ(interval_selection.getModeIndices(), std::vector<int>({0, 5, 3}));

    auto position_selection = modalSelection(c_major, criterion, 0);
    ASSERT_EQ(position_selection.size(), static_cast<size_t>(3));
    ASSERT_POSITION_VECTOR_EQ(position_selection[0].first, musicpp_test::ints({0, 4, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(position_selection[1].first, musicpp_test::ints({0, 4, 9}), 12);
    ASSERT_POSITION_VECTOR_EQ(position_selection[2].first, musicpp_test::ints({0, 5, 9}), 12);

    auto modal_rel_mode = modal_relative_mode(position_selection);
    ASSERT_EQ(modal_rel_mode.size(), static_cast<size_t>(3));
    ASSERT_EQ(modal_rel_mode.getModeIndices(), std::vector<int>({0, 5, 3}));
    ASSERT_EQ(modal_rel_mode.getTotalVectorCount(), static_cast<size_t>(21));

    std::vector<int> notes = {63};
    auto filtered_modes = filterModalMatrix(modalMatrix(c_major), notes);
    auto filtered_transpositions = filterTranspositionMatrix(transpositionMatrix(c_major), notes);
    ASSERT_EQ(filtered_modes.size(), static_cast<size_t>(4));
    ASSERT_EQ(filtered_transpositions.size(), static_cast<size_t>(7));

    auto modes_copy = modalMatrix(c_major);
    auto transpositions_copy = transpositionMatrix(c_major);
    filterModalMatrixInPlace(modes_copy, notes);
    filterTranspositionMatrixInPlace(transpositions_copy, notes);
    ASSERT_EQ(modes_copy.size(), static_cast<size_t>(4));
    ASSERT_EQ(transpositions_copy.size(), static_cast<size_t>(7));

    TEST_CASE_LOG("matrix_selection_and_filtering");
    TEST_INPUT("scale", c_major);
    TEST_INPUT("criterion", criterion);
    TEST_INPUT("notes", notes);
    TEST_OUTPUT("interval_selection_indices", interval_selection.getModeIndices());
    TEST_OUTPUT("interval_selection_first", interval_selection[0].first);
    TEST_OUTPUT("position_selection_first", position_selection[0].first);
    TEST_OUTPUT("position_selection_second", position_selection[1].first);
    TEST_OUTPUT("position_selection_third", position_selection[2].first);
    TEST_OUTPUT("filtered_modes_size", filtered_modes.size());
    TEST_OUTPUT("filtered_transpositions_size", filtered_transpositions.size());
}

TEST_CASE("matrix_distance_examples", "[matrix]") {
    position_vector c_major({0, 2, 4, 5, 7, 9, 11});
    position_vector c_lydian({0, 2, 4, 6, 7, 9, 11});
    position_vector c_major_chord({0, 4, 7});
    position_vector g_major_chord({7, 11, 14});
    int complexity = 0;
    auto transposition_distances = calculateDistances(c_lydian, transpositionMatrix(c_major));
    auto best_transposition = transposition_distances.getByComplexity(complexity);
    ASSERT_EQ(best_transposition.getTransposition(), 7);
    ASSERT_NEAR(best_transposition.getDistance(), 0.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(best_transposition.getVector(), musicpp_test::ints({0, 2, 4, 6, 7, 9, 11}), 12);

    auto modal_distances = calculateDistances(c_lydian, modalMatrix(c_major));
    auto best_mode = modal_distances.getByComplexity(complexity);
    ASSERT_EQ(best_mode.getIndex(), 3);
    ASSERT_NEAR(best_mode.getDistance(), 0.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(best_mode.getVector(), musicpp_test::ints({0, 2, 4, 6, 7, 9, 11}), 12);

    int center = align(c_major_chord, g_major_chord);
    ASSERT_EQ(center, -2);
    auto rel_mode_distances = calculateDistances(c_major_chord, relative_mode_matrix(g_major_chord, center));
    auto best_rel_mode = rel_mode_distances.getByComplexity(complexity);
    ASSERT_EQ(best_rel_mode.get_relative_mode_offset(), -2);
    ASSERT_EQ(best_rel_mode.getCenter(), -2);
    ASSERT_NEAR(best_rel_mode.getDistance(), 3.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(best_rel_mode.getVector(), musicpp_test::ints({-1, 2, 7}), 12);

    interval_vector crit({2, 2, 3});
    auto modal_selection_distances = calculateDistances(g_major_chord, modalSelection(c_major, crit, 0));
    auto best_degree = modal_selection_distances.getByComplexity(complexity);
    ASSERT_EQ(best_degree.getModeIndex(), 3);
    ASSERT_NEAR(best_degree.getDistance(), 18.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(best_degree.getChord(), musicpp_test::ints({0, 5, 9}), 12);

    auto modal_rel_mode_distances = calculateDistances(g_major_chord, modal_relative_mode(modalSelection(c_major, crit, 0)));
    auto best_modal_rel_mode = modal_rel_mode_distances.getByComplexity(complexity);
    ASSERT_EQ(best_modal_rel_mode.getModeIndex(), 0);
    ASSERT_EQ(best_modal_rel_mode.get_relative_mode_index(), 2);
    ASSERT_NEAR(best_modal_rel_mode.getDistance(), 3.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(best_modal_rel_mode.getVector(), musicpp_test::ints({7, 12, 16}), 12);

    TEST_CASE_LOG("matrix_distance_search");
    TEST_INPUT("c_major", c_major);
    TEST_INPUT("c_lydian", c_lydian);
    TEST_INPUT("c_major_chord", c_major_chord);
    TEST_INPUT("g_major_chord", g_major_chord);
    TEST_INPUT("criterion", crit);
    TEST_OUTPUT("best_transposition", best_transposition.getVector());
    TEST_OUTPUT("best_transposition_index", best_transposition.getTransposition());
    TEST_OUTPUT("best_mode", best_mode.getVector());
    TEST_OUTPUT("best_mode_index", best_mode.getIndex());
    TEST_OUTPUT("best_relative_mode", best_rel_mode.getVector());
    TEST_OUTPUT("best_relative_mode_offset", best_rel_mode.get_relative_mode_offset());
    TEST_OUTPUT("best_degree", best_degree.getChord());
    TEST_OUTPUT("best_degree_mode_index", best_degree.getModeIndex());
    TEST_OUTPUT("best_modal_relative_mode", best_modal_rel_mode.getVector());
    TEST_OUTPUT("best_modal_relative_mode_mode_index", best_modal_rel_mode.getModeIndex());
}

} // namespace
