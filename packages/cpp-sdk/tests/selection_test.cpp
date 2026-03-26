#include "test_support.h"

#include "../src/selection.h"

using musicpp_test::TestCase;

namespace {

void selection_position_examples() {
    PositionVector c_major_scale({0, 2, 4, 5, 7, 9, 11}, 12);
    PositionVector triad_degrees({0, 2, 4}, 7);
    auto triad_root = select(c_major_scale, triad_degrees, 0);
    auto triad_first_inv = select(c_major_scale, triad_degrees, 1);
    auto triad_second_inv = select(c_major_scale, triad_degrees, 2);
    auto triad_third_inv = select(c_major_scale, triad_degrees, 3);
    auto triad_negative = select(c_major_scale, triad_degrees, -1);
    auto triad_two_voices = select(c_major_scale, triad_degrees, 0, 2);
    auto triad_five_voices = select(c_major_scale, triad_degrees, 0, 5);

    ASSERT_POSITION_VECTOR_EQ(triad_root, musicpp_test::ints({0, 4, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(triad_first_inv, musicpp_test::ints({4, 7, 12}), 12);
    ASSERT_POSITION_VECTOR_EQ(triad_second_inv, musicpp_test::ints({7, 12, 16}), 12);
    ASSERT_POSITION_VECTOR_EQ(triad_third_inv, musicpp_test::ints({12, 16, 19}), 12);
    ASSERT_POSITION_VECTOR_EQ(triad_negative, musicpp_test::ints({-5, 0, 4}), 12);
    ASSERT_POSITION_VECTOR_EQ(triad_two_voices, musicpp_test::ints({0, 4}), 12);
    ASSERT_POSITION_VECTOR_EQ(triad_five_voices, musicpp_test::ints({0, 4, 7, 12, 16}), 12);

    PositionVector extended_degrees({0, 3, 7}, 12);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, extended_degrees), musicpp_test::ints({0, 5, 12}), 12);

    TEST_CASE_LOG("position_selection");
    TEST_INPUT("scale", c_major_scale);
    TEST_INPUT("triad_degrees", triad_degrees);
    TEST_OUTPUT("root", triad_root);
    TEST_OUTPUT("first_inversion", triad_first_inv);
    TEST_OUTPUT("second_inversion", triad_second_inv);
    TEST_OUTPUT("third_inversion", triad_third_inv);
    TEST_OUTPUT("negative_rotation", triad_negative);
    TEST_OUTPUT("two_voices", triad_two_voices);
    TEST_OUTPUT("five_voices", triad_five_voices);
}

void selection_interval_examples() {
    PositionVector c_major_scale({0, 2, 4, 5, 7, 9, 11}, 12);
    IntervalVector major_scale_intervals({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    IntervalVector thirds({2, 2, 3}, 0, 12);
    auto thirds_root = select(c_major_scale, thirds, 0);
    auto thirds_first = select(c_major_scale, thirds, 1);
    auto thirds_second = select(c_major_scale, thirds, 2);

    ASSERT_POSITION_VECTOR_EQ(thirds_root, musicpp_test::ints({0, 4, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(thirds_first, musicpp_test::ints({0, 4, 9}), 12);
    ASSERT_POSITION_VECTOR_EQ(thirds_second, musicpp_test::ints({0, 5, 9}), 12);

    thirds.setOffset(1);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, thirds, 0), musicpp_test::ints({2, 5, 9}), 12);
    thirds.setOffset(2);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, thirds, 0), musicpp_test::ints({4, 7, 11}), 12);
    thirds.setOffset(-1);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, thirds, 0), musicpp_test::ints({-1, 2, 5}), 12);
    thirds.setOffset(0);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, thirds, 0, 7), musicpp_test::ints({0, 4, 7, 12, 16, 19, 24}), 12);

    IntervalVector grouping({2}, 1, 12);
    ASSERT_INTERVAL_VECTOR_EQ(select(major_scale_intervals, grouping, 0, 3), musicpp_test::ints({3, 4, 3}), 2, 12);
    ASSERT_INTERVAL_VECTOR_EQ(select(major_scale_intervals, grouping, 1, 3), musicpp_test::ints({3, 4, 3}), 2, 12);
    ASSERT_INTERVAL_VECTOR_EQ(select(major_scale_intervals, grouping, 2, 3), musicpp_test::ints({3, 4, 3}), 2, 12);

    IntervalVector scale_with_offset({2, 2, 1, 2, 2, 2, 1}, 7, 12);
    grouping.setOffset(0);
    ASSERT_INTERVAL_VECTOR_EQ(select(scale_with_offset, grouping, 0, 3), musicpp_test::ints({4, 3, 4}), 7, 12);
    grouping.setOffset(1);
    ASSERT_INTERVAL_VECTOR_EQ(select(scale_with_offset, grouping, 0, 3), musicpp_test::ints({3, 4, 3}), 9, 12);
    grouping.setOffset(2);
    ASSERT_INTERVAL_VECTOR_EQ(select(scale_with_offset, grouping, 0, 3), musicpp_test::ints({3, 4, 3}), 11, 12);
    grouping.setOffset(0);
    ASSERT_INTERVAL_VECTOR_EQ(select(scale_with_offset, grouping, 0, 6), musicpp_test::ints({4, 3, 4, 3, 3, 4}), 7, 12);

    TEST_CASE_LOG("interval_selection");
    TEST_INPUT("scale", c_major_scale);
    TEST_INPUT("major_scale_intervals", major_scale_intervals);
    TEST_INPUT("thirds", IntervalVector({2, 2, 3}, 0, 12));
    TEST_OUTPUT("thirds_root", thirds_root);
    TEST_OUTPUT("thirds_first", thirds_first);
    TEST_OUTPUT("thirds_second", thirds_second);
    TEST_OUTPUT("grouping_on_offset_scale", select(scale_with_offset, grouping, 0, 6));
}

void selection_position_from_interval_examples() {
    IntervalVector major_scale_intervals({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    PositionVector positions({0, 2, 5}, 12);
    auto positions_0 = select(major_scale_intervals, positions, 0);
    auto positions_1 = select(major_scale_intervals, positions, 1);
    auto positions_2 = select(major_scale_intervals, positions, 2);

    ASSERT_INTERVAL_VECTOR_EQ(positions_0, musicpp_test::ints({4, 5, 3}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(positions_1, musicpp_test::ints({5, 3, 4}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(positions_2, musicpp_test::ints({3, 4, 5}), 0, 12);

    IntervalVector offset_source({2, 2, 1, 2, 2, 2, 1}, 5, 12);
    ASSERT_INTERVAL_VECTOR_EQ(select(offset_source, positions, 0), musicpp_test::ints({4, 5, 3}), 5, 12);
    ASSERT_INTERVAL_VECTOR_EQ(select(offset_source, positions, 0, 5), musicpp_test::ints({4, 5, 3, 4, 5}), 5, 12);

    PositionVector single_pos({3}, 12);
    ASSERT_INTERVAL_VECTOR_EQ(select(major_scale_intervals, single_pos, 0), musicpp_test::ints({12}), 5, 12);

    TEST_CASE_LOG("position_selection_from_interval_source");
    TEST_INPUT("intervals", major_scale_intervals);
    TEST_INPUT("positions", positions);
    TEST_OUTPUT("rotation_0", positions_0);
    TEST_OUTPUT("rotation_1", positions_1);
    TEST_OUTPUT("rotation_2", positions_2);
    TEST_OUTPUT("offset_source_rotation_0", select(offset_source, positions, 0));
}

void selection_musical_and_edge_examples() {
    PositionVector c_major_scale({0, 2, 4, 5, 7, 9, 11}, 12);
    IntervalVector harmony_by_thirds({2}, 0, 12);
    harmony_by_thirds.setOffset(1);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, harmony_by_thirds, 0, 4), musicpp_test::ints({2, 5, 9, 12}), 12);
    harmony_by_thirds.setOffset(4);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, harmony_by_thirds, 0, 4), musicpp_test::ints({7, 11, 14, 17}), 12);
    harmony_by_thirds.setOffset(0);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, harmony_by_thirds, 0, 4), musicpp_test::ints({0, 4, 7, 11}), 12);

    PositionVector triad_pattern({0, 2, 4}, 12);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, triad_pattern, 6), musicpp_test::ints({24, 28, 31}), 12);

    PositionVector empty_pos({}, 12);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, empty_pos), musicpp_test::ints({}), 12);

    PositionVector single_elem({5}, 12);
    PositionVector single_crit({0}, 12);
    ASSERT_POSITION_VECTOR_EQ(select(single_elem, single_crit), musicpp_test::ints({5}), 12);

    IntervalVector major_scale_intervals({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    IntervalVector small_crit({1, 1}, 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(
        select(major_scale_intervals, small_crit, 0, 15),
        musicpp_test::ints({2, 2, 1, 2, 2, 2, 1, 2, 2, 1, 2, 2, 2, 1, 2}),
        0,
        12);

    PositionVector triad_degrees({0, 2, 4}, 7);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, triad_degrees, -3), musicpp_test::ints({-12, -8, -5}), 12);

    TEST_CASE_LOG("selection_edge_cases");
    TEST_INPUT("scale", c_major_scale);
    TEST_INPUT("harmony_by_thirds", IntervalVector({2}, 0, 12));
    TEST_INPUT("triad_pattern", triad_pattern);
    TEST_OUTPUT("harmony_offset_1", select(c_major_scale, IntervalVector({2}, 1, 12), 0, 4));
    TEST_OUTPUT("harmony_offset_4", select(c_major_scale, IntervalVector({2}, 4, 12), 0, 4));
    TEST_OUTPUT("harmony_offset_0", select(c_major_scale, IntervalVector({2}, 0, 12), 0, 4));
    TEST_OUTPUT("triad_pattern_mode_6", select(c_major_scale, triad_pattern, 6));
    TEST_OUTPUT("triad_degrees_minus_3", select(c_major_scale, triad_degrees, -3));
}

void selection_degenerate_examples() {
    PositionVector c_major_scale({0, 2, 4, 5, 7, 9, 11}, 12);
    PositionVector repeated_degrees({0, 0, 2}, 7);
    PositionVector oversized_degrees({0, 2, 9}, 7);
    PositionVector triad_degrees({0, 2, 4}, 7);
    PositionVector empty_positions({}, 12);
    PositionVector single_elem({5}, 12);
    PositionVector repeated_single({0, 0, 0}, 1);
    IntervalVector empty_interval({}, 0, 12);
    IntervalVector major_scale_intervals({2, 2, 1, 2, 2, 2, 1}, 0, 12);

    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, repeated_degrees, 0), musicpp_test::ints({0, 0, 4}), 12);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, oversized_degrees, 0), musicpp_test::ints({0, 4, 16}), 12);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, triad_degrees, 9), musicpp_test::ints({36, 40, 43}), 12);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, empty_positions, 0), musicpp_test::ints({}), 12);
    ASSERT_POSITION_VECTOR_EQ(select(c_major_scale, empty_interval, 0), musicpp_test::ints({}), 12);
    ASSERT_INTERVAL_VECTOR_EQ(select(major_scale_intervals, empty_positions, 0), musicpp_test::ints({}), 0, 12);
    ASSERT_POSITION_VECTOR_EQ(select(single_elem, repeated_single), musicpp_test::ints({5, 5, 5}), 12);

    TEST_CASE_LOG("selection_degenerate_cases");
    TEST_INPUT("scale", c_major_scale);
    TEST_INPUT("repeated_degrees", repeated_degrees);
    TEST_INPUT("oversized_degrees", oversized_degrees);
    TEST_INPUT("triad_degrees", triad_degrees);
    TEST_INPUT("empty_positions", empty_positions);
    TEST_INPUT("empty_interval", empty_interval);
    TEST_INPUT("single_elem", single_elem);
    TEST_INPUT("repeated_single", repeated_single);
    TEST_OUTPUT("repeated_degrees", select(c_major_scale, repeated_degrees, 0));
    TEST_OUTPUT("oversized_degrees", select(c_major_scale, oversized_degrees, 0));
    TEST_OUTPUT("triad_mode_9", select(c_major_scale, triad_degrees, 9));
    TEST_OUTPUT("empty_positions", select(c_major_scale, empty_positions, 0));
    TEST_OUTPUT("empty_interval", select(c_major_scale, empty_interval, 0));
    TEST_OUTPUT("single_elem", select(single_elem, repeated_single));
}

} // namespace

int main() {
    return musicpp_test::run_tests({
        {"selection_position_examples", selection_position_examples},
        {"selection_interval_examples", selection_interval_examples},
        {"selection_position_from_interval_examples", selection_position_from_interval_examples},
        {"selection_musical_and_edge_examples", selection_musical_and_edge_examples},
        {"selection_degenerate_examples", selection_degenerate_examples},
    });
}
