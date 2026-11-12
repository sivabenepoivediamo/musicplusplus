#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

using musicpp::vector_set;
using musicpp::position_vector;
using musicpp::interval_vector;
using musicpp::onset_vector;

namespace {

std::vector<int> normalize_positions(const position_vector& positions) {
    std::vector<int> normalized = positions.data();
    if (normalized.empty()) {
        return normalized;
    }
    int anchor = normalized.front();
    for (int& value : normalized) {
        value -= anchor;
    }
    return normalized;
}

TEST_CASE("vectors_construction_and_transforms", "[vectors]") {
    vector_set majorTriad = vector_set::from_positions({0, 4, 7});
    ASSERT_POSITION_VECTOR_EQ(majorTriad.positions(), musicpp_test::ints({0, 4, 7}), 12);
    ASSERT_INTERVAL_VECTOR_EQ(majorTriad.intervals(), musicpp_test::ints({4, 3, 5}), 0, 12);
    ASSERT_ONSET_VECTOR_EQ(
        majorTriad.onset(),
        musicpp_test::ints({1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}),
        0,
        12);

    vector_set fromIntervals = vector_set::from_intervals({4, 3, 5});
    ASSERT_POSITION_VECTOR_EQ(fromIntervals.positions(), musicpp_test::ints({0, 4, 7}), 12);

    vector_set fromBinary = vector_set::from_onset({1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0});
    ASSERT_POSITION_VECTOR_EQ(fromBinary.positions(), musicpp_test::ints({0, 4, 7}), 12);

    vector_set euclidean = vector_set::euclidean(5, 8);
    ASSERT_POSITION_VECTOR_EQ(euclidean.positions(), musicpp_test::ints({0, 2, 3, 5, 7}), -1);
    ASSERT_INTERVAL_VECTOR_EQ(euclidean.intervals(), musicpp_test::ints({2, 1, 2, 2, 1}), 0, -1);
    ASSERT_ONSET_VECTOR_EQ(euclidean.onset(), musicpp_test::ints({1, 0, 1, 1, 0, 1, 0, 1}), 0, -1);

    TEST_CASE_LOG("vectors_construction");
    TEST_INPUT("major_triad_source", musicpp_test::ints({0, 4, 7}));
    TEST_OUTPUT("major_triad.positions", majorTriad.positions());
    TEST_OUTPUT("major_triad.intervals", majorTriad.intervals());
    TEST_OUTPUT("major_triad.onset", majorTriad.onset());
    TEST_OUTPUT("from_intervals.positions", fromIntervals.positions());
    TEST_OUTPUT("from_onset.positions", fromBinary.positions());
    TEST_CASE_LOG("vectors_euclidean");
    TEST_INPUT("args", musicpp_test::ints({5, 8}));
    TEST_OUTPUT("positions", euclidean.positions());
    TEST_OUTPUT("intervals", euclidean.intervals());
    TEST_OUTPUT("onset", euclidean.onset());
}

TEST_CASE("vectors_operator_examples", "[vectors]") {
    vector_set majorTriad = vector_set::from_positions({0, 4, 7});

    ASSERT_POSITION_VECTOR_EQ(majorTriad.transpose(5).positions(), musicpp_test::ints({5, 9, 12}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.multiply_positions(5).positions(), musicpp_test::ints({0, 20, 35}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.rotate(1).positions(), musicpp_test::ints({7, 0, 4}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.relative_mode(1).positions(), musicpp_test::ints({4, 7, 12}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.invert_positions(0).positions(), musicpp_test::ints({-7, -4, 0}), 12);
    ASSERT_POSITION_VECTOR_EQ(
        majorTriad.complement_positions().positions(),
        musicpp_test::ints({1, 2, 3, 5, 6, 8, 9, 10, 11}),
        12);

    ASSERT_POSITION_VECTOR_EQ(majorTriad.add_to_intervals(2).positions(), musicpp_test::ints({0, 6, 11}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.multiply_intervals(2).positions(), musicpp_test::ints({0, 8, 14}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.parallel_mode(1).positions(), musicpp_test::ints({0, 3, 8}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.reverse_intervals().positions(), musicpp_test::ints({0, 5, 8}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.invert_intervals(0).positions(), musicpp_test::ints({0, 5, 8}), 12);

    ASSERT_ONSET_VECTOR_EQ(
        majorTriad.rotate_onset(3).onset(),
        musicpp_test::ints({0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}),
        0,
        12);
    ASSERT_POSITION_VECTOR_EQ(
        majorTriad.complement_onset().positions(),
        musicpp_test::ints({1, 2, 3, 5, 6, 8, 9, 10, 11}),
        12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.multiply_onset(2).positions(), musicpp_test::ints({0, 8, 14}), 24);

    vector_set pentatonic = vector_set::from_positions({0, 2, 4, 7, 9});
    ASSERT_POSITION_VECTOR_EQ(pentatonic.divide_onset(2).positions(), musicpp_test::ints({0, 1, 2}), 6);

    vector_set setA = vector_set::from_positions({0, 2, 4, 6});
    vector_set setB = vector_set::from_positions({0, 3, 6, 9});
    ASSERT_POSITION_VECTOR_EQ((setA | setB).positions(), musicpp_test::ints({0, 2, 3, 4, 6, 9}), 12);
    ASSERT_POSITION_VECTOR_EQ((setA & setB).positions(), musicpp_test::ints({0, 6}), 12);
    ASSERT_POSITION_VECTOR_EQ((setA ^ setB).positions(), musicpp_test::ints({2, 3, 4, 9}), 12);

    vector_set motif = vector_set::from_positions({0, 4, 7});
    ASSERT_POSITION_VECTOR_EQ(motif.transpose(7).positions(), musicpp_test::ints({7, 11, 14}), 12);
    ASSERT_POSITION_VECTOR_EQ(motif.negative().positions(), musicpp_test::ints({7, 12, 15}), 12);
    ASSERT_POSITION_VECTOR_EQ(motif.invert_positions(0).positions(), musicpp_test::ints({-7, -4, 0}), 12);
    ASSERT_POSITION_VECTOR_EQ(motif.relative_mode(3, 4).positions(), musicpp_test::ints({12, 16, 19, 24}), 12);

    ASSERT_ONSET_VECTOR_EQ(vector_set::euclidean(3, 8).onset(), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 0}), 0, -1);
    ASSERT_ONSET_VECTOR_EQ(vector_set::euclidean(5, 8).onset(), musicpp_test::ints({1, 0, 1, 1, 0, 1, 0, 1}), 0, -1);
    ASSERT_ONSET_VECTOR_EQ(vector_set::euclidean(5, 12).onset(), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0}), 0, -1);
    ASSERT_ONSET_VECTOR_EQ(
        vector_set::euclidean(7, 16).onset(),
        musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0}),
        0,
        -1);

    TEST_CASE_LOG("vectors_position_operators");
    TEST_INPUT("major_triad", majorTriad.positions());
    TEST_OUTPUT("transpose_5", majorTriad.transpose(5).positions());
    TEST_OUTPUT("multiply_positions_5", majorTriad.multiply_positions(5).positions());
    TEST_OUTPUT("rotate_1", majorTriad.rotate(1).positions());
    TEST_OUTPUT("relative_mode_positions_1", majorTriad.relative_mode(1).positions());
    TEST_OUTPUT("invert_positions_0", majorTriad.invert_positions(0).positions());
    TEST_OUTPUT("complement_positions", majorTriad.complement_positions().positions());
    TEST_CASE_LOG("vectors_interval_and_onset_operators");
    TEST_INPUT("pentatonic", pentatonic.positions());
    TEST_OUTPUT("add_to_intervals_2", majorTriad.add_to_intervals(2).positions());
    TEST_OUTPUT("multiply_intervals_2", majorTriad.multiply_intervals(2).positions());
    TEST_OUTPUT("parallel_mode_1", majorTriad.parallel_mode(1).positions());
    TEST_OUTPUT("reverse_intervals", majorTriad.reverse_intervals().positions());
    TEST_OUTPUT("invert_intervals_0", majorTriad.invert_intervals(0).positions());
    TEST_OUTPUT("rotate_onset_3", majorTriad.rotate_onset(3).onset());
    TEST_OUTPUT("complement_onset", majorTriad.complement_onset().positions());
    TEST_OUTPUT("multiply_onset_2", majorTriad.multiply_onset(2).positions());
    TEST_OUTPUT("divide_onset_2", pentatonic.divide_onset(2).positions());
    TEST_CASE_LOG("vectors_set_operations");
    TEST_INPUT("setA", setA.positions());
    TEST_INPUT("setB", setB.positions());
    TEST_OUTPUT("union", (setA | setB).positions());
    TEST_OUTPUT("intersection", (setA & setB).positions());
    TEST_OUTPUT("xor", (setA ^ setB).positions());
    TEST_CASE_LOG("vectors_motif_and_euclidean");
    TEST_INPUT("motif", motif.positions());
    TEST_OUTPUT("motif_transpose_7", motif.transpose(7).positions());
    TEST_OUTPUT("motif_negative", motif.negative().positions());
    TEST_OUTPUT("motif_invert_0", motif.invert_positions(0).positions());
    TEST_OUTPUT("motif_relative_mode_3_4", motif.relative_mode(3, 4).positions());
    TEST_OUTPUT("euclidean_3_8", vector_set::euclidean(3, 8).onset());
    TEST_OUTPUT("euclidean_5_8", vector_set::euclidean(5, 8).onset());
    TEST_OUTPUT("euclidean_5_12", vector_set::euclidean(5, 12).onset());
    TEST_OUTPUT("euclidean_7_16", vector_set::euclidean(7, 16).onset());
}

TEST_CASE("vectors_roundtrip_and_invariants", "[vectors]") {
    vector_set source = vector_set::from_positions({0, 3, 7, 10});

    ASSERT_POSITION_VECTOR_EQ(
        vector_set::from_intervals(source.intervals().data()).positions(),
        source.positions().data(),
        12);
    ASSERT_POSITION_VECTOR_EQ(
        vector_set::from_onset(source.onset().data()).positions(),
        source.positions().data(),
        12);

    vector_set transposed = source.transpose(1);
    ASSERT_POSITION_VECTOR_EQ(transposed.positions(), musicpp_test::ints({1, 4, 8, 11}), 12);
    ASSERT_INTERVAL_VECTOR_EQ(transposed.intervals(), source.intervals().data(), 1, 12);
    ASSERT_VECTOR_EQ(
        vector_set::from_onset(transposed.onset().data()).positions().data(),
        normalize_positions(transposed.positions()));
    ASSERT_VECTOR_EQ(
        normalize_positions(transposed.positions()),
        vector_set::from_intervals(transposed.intervals().data()).positions().data());

    vector_set rotated = source.rotate(1);
    ASSERT_ONSET_VECTOR_EQ(
        vector_set::from_onset(rotated.onset().data()).onset(),
        rotated.onset().data(),
        0,
        12);
    ASSERT_POSITION_VECTOR_EQ(rotated.rotate(3).positions(), source.positions().data(), 12);

    vector_set inverted = source.invert_positions(0);
    ASSERT_VECTOR_EQ(
        normalize_positions(inverted.invert_positions(0).positions()),
        source.positions().data());
    ASSERT_VECTOR_EQ(
        normalize_positions(inverted.positions()),
        vector_set::from_intervals(inverted.intervals().data()).positions().data());

    TEST_CASE_LOG("vectors_roundtrip_and_invariants");
    TEST_INPUT("source", source.positions());
    TEST_OUTPUT("from_intervals", vector_set::from_intervals(source.intervals().data()).positions());
    TEST_OUTPUT("from_onset", vector_set::from_onset(source.onset().data()).positions());
    TEST_OUTPUT("transposed", transposed.positions());
    TEST_OUTPUT("transposed_intervals", transposed.intervals());
    TEST_OUTPUT("rotated", rotated.positions());
    TEST_OUTPUT("rotated_onset", rotated.onset());
    TEST_OUTPUT("inverted", inverted.positions());
}

TEST_CASE("interval_vector_examples", "[vectors]") {
    interval_vector iv1;
    ASSERT_INTERVAL_VECTOR_EQ(iv1, musicpp_test::ints({0}), 0, 12);

    interval_vector iv2({2, 3, 5, 7}, 1, 12);
    ASSERT_EQ(iv2.size(), static_cast<size_t>(4));
    ASSERT_EQ(iv2.offset(), 1);
    ASSERT_EQ(iv2.mod(), 12);
    ASSERT_TRUE(!iv2.empty());

    iv2.set_offset(3);
    iv2.set_mod(24);
    iv2.set_data({1, 4, 7});
    ASSERT_INTERVAL_VECTOR_EQ(iv2, musicpp_test::ints({1, 4, 7}), 3, 24);

    interval_vector iv3({1, 2, 3, 4});
    ASSERT_INTERVAL_VECTOR_EQ(iv3 + 5, musicpp_test::ints({6, 7, 8, 9}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 - 2, musicpp_test::ints({-1, 0, 1, 2}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 * 3, musicpp_test::ints({3, 6, 9, 12}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 / 2, musicpp_test::ints({0, 1, 1, 2}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 % 3, musicpp_test::ints({1, 2, 0, 1}), 0, 12);

    interval_vector iv4({1, 2, 3});
    interval_vector iv5({4, 5, 6});
    ASSERT_INTERVAL_VECTOR_EQ(iv4 + iv5, musicpp_test::ints({5, 7, 9}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 - iv5, musicpp_test::ints({-3, -3, -3}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 * iv5, musicpp_test::ints({4, 10, 18}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 / iv5, musicpp_test::ints({0, 0, 0}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 % iv5, musicpp_test::ints({1, 2, 3}), 0, 12);

    interval_vector iv8({10, 20, 30, 40});
    ASSERT_EQ(iv8[0], 10);
    ASSERT_EQ(iv8[-1], 40);

    interval_vector iv12({1, 2, 3, 4, 5});
    ASSERT_INTERVAL_VECTOR_EQ(iv12.parallel_mode(2), musicpp_test::ints({3, 4, 5, 1, 2}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv12.reverse(), musicpp_test::ints({5, 4, 3, 2, 1}), 0, 12);

    interval_vector iv13({1, 2, 3, 4, 5, 6});
    ASSERT_INTERVAL_VECTOR_EQ(iv13.inversion(1), musicpp_test::ints({1, 6, 5, 4, 3, 2}), 0, 12);

    interval_vector iv14({13, 25, -3, 14});
    ASSERT_INTERVAL_VECTOR_EQ(iv14.normalize(12), musicpp_test::ints({1, 1, 9, 2}), 0, 12);

    interval_vector iv15({1, 2, 3});
    ASSERT_INTERVAL_VECTOR_EQ(iv15.componentwiseSum({4, 5}, true), musicpp_test::ints({5, 7, 7}), 0, 12);

    interval_vector iv16({1, 2, 3});
    interval_vector iv17({4, 5});
    ASSERT_INTERVAL_VECTOR_EQ(iv16.concatenate(iv17), musicpp_test::ints({1, 2, 3, 4, 5}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv16.repeat(3), musicpp_test::ints({1, 2, 3, 1, 2, 3, 1, 2, 3}), 0, 12);

    interval_vector iv18({1, 2, 3, 4, 5, 6});
    ASSERT_INTERVAL_VECTOR_EQ(iv18.singleMirror(3, true), musicpp_test::ints({3, 2, 1, 4, 5, 6}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv18.doubleMirror(3), musicpp_test::ints({3, 2, 1, 6, 5, 4}), 0, 12);

    TEST_CASE_LOG("interval_vector_operations");
    TEST_INPUT("iv12", iv12);
    TEST_INPUT("iv13", iv13);
    TEST_OUTPUT("parallel_mode_2", iv12.parallel_mode(2));
    TEST_OUTPUT("reverse", iv12.reverse());
    TEST_OUTPUT("inversion_1", iv13.inversion(1));
    TEST_OUTPUT("normalize_12", iv14.normalize(12));
    TEST_OUTPUT("componentwise_sum", iv15.componentwiseSum({4, 5}, true));
    TEST_OUTPUT("concatenate", iv16.concatenate(iv17));
    TEST_OUTPUT("repeat_3", iv16.repeat(3));
    TEST_OUTPUT("single_mirror", iv18.singleMirror(3, true));
    TEST_OUTPUT("double_mirror", iv18.doubleMirror(3));
}

TEST_CASE("onset_vector_examples", "[vectors]") {
    onset_vector bv1;
    ASSERT_ONSET_VECTOR_EQ(bv1, musicpp_test::ints({1, 0, 0, 0}), 0, 4);

    onset_vector bv2({1, 0, 1, 1, 0}, 0, 5);
    ASSERT_EQ(bv2.countPulses(), 3);
    ASSERT_NEAR(bv2.density(), 0.6, 1e-6);

    onset_vector bv7({1, 0, 1, 0, 1});
    ASSERT_ONSET_VECTOR_EQ(~bv7, musicpp_test::ints({0, 1, 0, 1, 0}), 0, 4);
    ASSERT_ONSET_VECTOR_EQ(bv7.complement(), musicpp_test::ints({0, 1, 0, 1, 0}), 0, 4);

    onset_vector bv16({1, 0, 0, 1, 0});
    ASSERT_ONSET_VECTOR_EQ(bv16.rotate(1), musicpp_test::ints({0, 0, 1, 0, 1}), 0, 4);
    ASSERT_ONSET_VECTOR_EQ(bv16.rotate(2), musicpp_test::ints({0, 1, 0, 1, 0}), 0, 4);

    onset_vector bv17({1, 0, 1, 0, 1});
    ASSERT_ONSET_VECTOR_EQ(bv17.inversion(0), musicpp_test::ints({1, 1, 0, 1, 0}), 0, 4);

    onset_vector bv19({1, 0, 1});
    onset_vector bv20({0, 1});
    ASSERT_ONSET_VECTOR_EQ(bv19.concatenate(bv20), musicpp_test::ints({1, 0, 1, 0, 1}), 0, 4);
    ASSERT_ONSET_VECTOR_EQ(bv19.repeat(3), musicpp_test::ints({1, 0, 1, 1, 0, 1, 1, 0, 1}), 0, 4);

    onset_vector bv21({1, 0, 0, 1, 0, 1, 0, 0});
    ASSERT_EQ(bv21.countPulses(), 3);
    ASSERT_NEAR(bv21.density(), 0.375, 1e-6);
    ASSERT_VECTOR_EQ(bv21.getPulseIndices(), musicpp_test::ints({0, 3, 5}));
    ASSERT_VECTOR_EQ(bv21.getInterOnsetIntervals(), musicpp_test::ints({3, 2, 3}));

    ASSERT_ONSET_VECTOR_EQ(onset_vector::euclidean(3, 8), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 0}), 0, -1);
    ASSERT_ONSET_VECTOR_EQ(onset_vector::euclidean(5, 12), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0}), 0, -1);

    TEST_CASE_LOG("onset_vector_operations");
    TEST_INPUT("bv16", bv16);
    TEST_INPUT("bv17", bv17);
    TEST_INPUT("bv21", bv21);
    TEST_OUTPUT("rotate1", bv16.rotate(1));
    TEST_OUTPUT("rotate2", bv16.rotate(2));
    TEST_OUTPUT("inversion0", bv17.inversion(0));
    TEST_OUTPUT("concatenate", bv19.concatenate(bv20));
    TEST_OUTPUT("repeat3", bv19.repeat(3));
    TEST_OUTPUT("pulse_indices", bv21.getPulseIndices());
    TEST_OUTPUT("inter_onset_intervals", bv21.getInterOnsetIntervals());
    TEST_OUTPUT("euclidean_3_8", onset_vector::euclidean(3, 8));
    TEST_OUTPUT("euclidean_5_12", onset_vector::euclidean(5, 12));
}

TEST_CASE("position_vector_examples", "[vectors]") {
    position_vector pv1;
    ASSERT_POSITION_VECTOR_EQ(pv1, musicpp_test::ints({0}), 12);

    position_vector pv2({0, 2, 4, 7, 9}, 12, 0, true, false);
    ASSERT_EQ(pv2.size(), static_cast<size_t>(5));
    ASSERT_EQ(pv2.mod(), 12);
    ASSERT_EQ(pv2.user_range(), 12);
    ASSERT_EQ(pv2.effective_range(), 12);
    ASSERT_TRUE(pv2.auto_range_updates());
    ASSERT_TRUE(!pv2.use_user_range());

    position_vector pv3({0, 3, 7, 10});
    ASSERT_POSITION_VECTOR_EQ(pv3 + 5, musicpp_test::ints({5, 8, 12, 15}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 - 2, musicpp_test::ints({-2, 1, 5, 8}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 * 2, musicpp_test::ints({0, 6, 14, 20}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 / 2, musicpp_test::ints({0, 1, 3, 5}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 % 3, musicpp_test::ints({0, 0, 1, 1}), 12);

    position_vector pv12({0, 3, 7, 10});
    ASSERT_POSITION_VECTOR_EQ(pv12.rotate(1), musicpp_test::ints({10, 0, 3, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv12.rotate(2), musicpp_test::ints({7, 10, 0, 3}), 12);

    position_vector pv14({0, 3, 7}, 12);
    ASSERT_POSITION_VECTOR_EQ(pv14.complement(), musicpp_test::ints({1, 2, 4, 5, 6, 8, 9, 10, 11}), 12);

    position_vector pv15({0, 4, 7, 11});
    ASSERT_POSITION_VECTOR_EQ(pv15.inversion(0), musicpp_test::ints({-11, -7, -4, 0}), 12);

    position_vector pv18({0, 3, 7});
    position_vector pv19({10, 14});
    ASSERT_POSITION_VECTOR_EQ(pv18.concatenate(pv19), musicpp_test::ints({0, 3, 7, 10, 14}), 12);

    position_vector pv22({0, 5, 12, 18}, 12, 0, true, false);
    ASSERT_EQ(pv22.effective_range(), 24);

    TEST_CASE_LOG("position_vector_operations");
    TEST_INPUT("pv12", pv12);
    TEST_INPUT("pv14", pv14);
    TEST_INPUT("pv15", pv15);
    TEST_OUTPUT("rotate1", pv12.rotate(1));
    TEST_OUTPUT("rotate2", pv12.rotate(2));
    TEST_OUTPUT("complement", pv14.complement());
    TEST_OUTPUT("inversion0", pv15.inversion(0));
    TEST_OUTPUT("concatenate", pv18.concatenate(pv19));
    TEST_OUTPUT("range", pv22.effective_range());
}

} // namespace
