#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

namespace {

std::vector<int> normalize_positions(const PositionVector& positions) {
    std::vector<int> normalized = positions.getData();
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
    Vectors majorTriad = Vectors::fromPositions({0, 4, 7});
    ASSERT_POSITION_VECTOR_EQ(majorTriad.getPositions(), musicpp_test::ints({0, 4, 7}), 12);
    ASSERT_INTERVAL_VECTOR_EQ(majorTriad.getIntervals(), musicpp_test::ints({4, 3, 5}), 0, 12);
    ASSERT_BINARY_VECTOR_EQ(
        majorTriad.getBinary(),
        musicpp_test::ints({1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}),
        0,
        12);

    Vectors fromIntervals = Vectors::fromIntervals({4, 3, 5});
    ASSERT_POSITION_VECTOR_EQ(fromIntervals.getPositions(), musicpp_test::ints({0, 4, 7}), 12);

    Vectors fromBinary = Vectors::fromBinary({1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0});
    ASSERT_POSITION_VECTOR_EQ(fromBinary.getPositions(), musicpp_test::ints({0, 4, 7}), 12);

    Vectors euclidean = Vectors::euclidean(5, 8);
    ASSERT_POSITION_VECTOR_EQ(euclidean.getPositions(), musicpp_test::ints({0, 2, 3, 5, 7}), -1);
    ASSERT_INTERVAL_VECTOR_EQ(euclidean.getIntervals(), musicpp_test::ints({2, 1, 2, 2, 1}), 0, -1);
    ASSERT_BINARY_VECTOR_EQ(euclidean.getBinary(), musicpp_test::ints({1, 0, 1, 1, 0, 1, 0, 1}), 0, -1);

    TEST_CASE_LOG("vectors_construction");
    TEST_INPUT("major_triad_source", musicpp_test::ints({0, 4, 7}));
    TEST_OUTPUT("major_triad.positions", majorTriad.getPositions());
    TEST_OUTPUT("major_triad.intervals", majorTriad.getIntervals());
    TEST_OUTPUT("major_triad.binary", majorTriad.getBinary());
    TEST_OUTPUT("from_intervals.positions", fromIntervals.getPositions());
    TEST_OUTPUT("from_binary.positions", fromBinary.getPositions());
    TEST_CASE_LOG("vectors_euclidean");
    TEST_INPUT("args", musicpp_test::ints({5, 8}));
    TEST_OUTPUT("positions", euclidean.getPositions());
    TEST_OUTPUT("intervals", euclidean.getIntervals());
    TEST_OUTPUT("binary", euclidean.getBinary());
}

TEST_CASE("vectors_operator_examples", "[vectors]") {
    Vectors majorTriad = Vectors::fromPositions({0, 4, 7});

    ASSERT_POSITION_VECTOR_EQ(majorTriad.transpose(5).getPositions(), musicpp_test::ints({5, 9, 12}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.multiplyPositions(5).getPositions(), musicpp_test::ints({0, 20, 35}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.rotatePositions(1).getPositions(), musicpp_test::ints({7, 0, 4}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.rototranslatePositions(1).getPositions(), musicpp_test::ints({4, 7, 12}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.invertPositions(0).getPositions(), musicpp_test::ints({-7, -4, 0}), 12);
    ASSERT_POSITION_VECTOR_EQ(
        majorTriad.complementPositions().getPositions(),
        musicpp_test::ints({1, 2, 3, 5, 6, 8, 9, 10, 11}),
        12);

    ASSERT_POSITION_VECTOR_EQ(majorTriad.addToIntervals(2).getPositions(), musicpp_test::ints({0, 6, 11}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.multiplyIntervals(2).getPositions(), musicpp_test::ints({0, 8, 14}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.rotateIntervals(1).getPositions(), musicpp_test::ints({0, 3, 8}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.reverseIntervals().getPositions(), musicpp_test::ints({0, 5, 8}), 12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.invertIntervals(0).getPositions(), musicpp_test::ints({0, 5, 8}), 12);

    ASSERT_BINARY_VECTOR_EQ(
        majorTriad.rotateBinary(3).getBinary(),
        musicpp_test::ints({0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}),
        0,
        12);
    ASSERT_POSITION_VECTOR_EQ(
        majorTriad.complementBinary().getPositions(),
        musicpp_test::ints({1, 2, 3, 5, 6, 8, 9, 10, 11}),
        12);
    ASSERT_POSITION_VECTOR_EQ(majorTriad.multiplyBinary(2).getPositions(), musicpp_test::ints({0, 8, 14}), 12);

    Vectors pentatonic = Vectors::fromPositions({0, 2, 4, 7, 9});
    ASSERT_POSITION_VECTOR_EQ(pentatonic.divideBinary(2).getPositions(), musicpp_test::ints({0, 1, 2}), 12);

    Vectors setA = Vectors::fromPositions({0, 2, 4, 6});
    Vectors setB = Vectors::fromPositions({0, 3, 6, 9});
    ASSERT_POSITION_VECTOR_EQ((setA | setB).getPositions(), musicpp_test::ints({0, 2, 3, 4, 6, 9}), 12);
    ASSERT_POSITION_VECTOR_EQ((setA & setB).getPositions(), musicpp_test::ints({0, 6}), 12);
    ASSERT_POSITION_VECTOR_EQ((setA ^ setB).getPositions(), musicpp_test::ints({2, 3, 4, 9}), 12);

    Vectors motif = Vectors::fromPositions({0, 4, 7});
    ASSERT_POSITION_VECTOR_EQ(motif.transpose(7).getPositions(), musicpp_test::ints({7, 11, 14}), 12);
    ASSERT_POSITION_VECTOR_EQ(motif.negative().getPositions(), musicpp_test::ints({7, 12, 15}), 12);
    ASSERT_POSITION_VECTOR_EQ(motif.invertPositions(0).getPositions(), musicpp_test::ints({-7, -4, 0}), 12);
    ASSERT_POSITION_VECTOR_EQ(motif.rototranslatePositions(3, 4).getPositions(), musicpp_test::ints({12, 16, 19, 24}), 12);

    ASSERT_BINARY_VECTOR_EQ(Vectors::euclidean(3, 8).getBinary(), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 0}), 0, -1);
    ASSERT_BINARY_VECTOR_EQ(Vectors::euclidean(5, 8).getBinary(), musicpp_test::ints({1, 0, 1, 1, 0, 1, 0, 1}), 0, -1);
    ASSERT_BINARY_VECTOR_EQ(Vectors::euclidean(5, 12).getBinary(), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0}), 0, -1);
    ASSERT_BINARY_VECTOR_EQ(
        Vectors::euclidean(7, 16).getBinary(),
        musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0}),
        0,
        -1);

    TEST_CASE_LOG("vectors_position_operators");
    TEST_INPUT("major_triad", majorTriad.getPositions());
    TEST_OUTPUT("transpose_5", majorTriad.transpose(5).getPositions());
    TEST_OUTPUT("multiply_positions_5", majorTriad.multiplyPositions(5).getPositions());
    TEST_OUTPUT("rotate_positions_1", majorTriad.rotatePositions(1).getPositions());
    TEST_OUTPUT("rototranslate_positions_1", majorTriad.rototranslatePositions(1).getPositions());
    TEST_OUTPUT("invert_positions_0", majorTriad.invertPositions(0).getPositions());
    TEST_OUTPUT("complement_positions", majorTriad.complementPositions().getPositions());
    TEST_CASE_LOG("vectors_interval_and_binary_operators");
    TEST_INPUT("pentatonic", pentatonic.getPositions());
    TEST_OUTPUT("add_to_intervals_2", majorTriad.addToIntervals(2).getPositions());
    TEST_OUTPUT("multiply_intervals_2", majorTriad.multiplyIntervals(2).getPositions());
    TEST_OUTPUT("rotate_intervals_1", majorTriad.rotateIntervals(1).getPositions());
    TEST_OUTPUT("reverse_intervals", majorTriad.reverseIntervals().getPositions());
    TEST_OUTPUT("invert_intervals_0", majorTriad.invertIntervals(0).getPositions());
    TEST_OUTPUT("rotate_binary_3", majorTriad.rotateBinary(3).getBinary());
    TEST_OUTPUT("complement_binary", majorTriad.complementBinary().getPositions());
    TEST_OUTPUT("multiply_binary_2", majorTriad.multiplyBinary(2).getPositions());
    TEST_OUTPUT("divide_binary_2", pentatonic.divideBinary(2).getPositions());
    TEST_CASE_LOG("vectors_set_operations");
    TEST_INPUT("setA", setA.getPositions());
    TEST_INPUT("setB", setB.getPositions());
    TEST_OUTPUT("union", (setA | setB).getPositions());
    TEST_OUTPUT("intersection", (setA & setB).getPositions());
    TEST_OUTPUT("xor", (setA ^ setB).getPositions());
    TEST_CASE_LOG("vectors_motif_and_euclidean");
    TEST_INPUT("motif", motif.getPositions());
    TEST_OUTPUT("motif_transpose_7", motif.transpose(7).getPositions());
    TEST_OUTPUT("motif_negative", motif.negative().getPositions());
    TEST_OUTPUT("motif_invert_0", motif.invertPositions(0).getPositions());
    TEST_OUTPUT("motif_rototranslate_3_4", motif.rototranslatePositions(3, 4).getPositions());
    TEST_OUTPUT("euclidean_3_8", Vectors::euclidean(3, 8).getBinary());
    TEST_OUTPUT("euclidean_5_8", Vectors::euclidean(5, 8).getBinary());
    TEST_OUTPUT("euclidean_5_12", Vectors::euclidean(5, 12).getBinary());
    TEST_OUTPUT("euclidean_7_16", Vectors::euclidean(7, 16).getBinary());
}

TEST_CASE("vectors_roundtrip_and_invariants", "[vectors]") {
    Vectors source = Vectors::fromPositions({0, 3, 7, 10});

    ASSERT_POSITION_VECTOR_EQ(
        Vectors::fromIntervals(source.getIntervals().getData()).getPositions(),
        source.getPositions().getData(),
        12);
    ASSERT_POSITION_VECTOR_EQ(
        Vectors::fromBinary(source.getBinary().getData()).getPositions(),
        source.getPositions().getData(),
        12);

    Vectors transposed = source.transpose(1);
    ASSERT_POSITION_VECTOR_EQ(transposed.getPositions(), musicpp_test::ints({1, 4, 8, 11}), 12);
    ASSERT_INTERVAL_VECTOR_EQ(transposed.getIntervals(), source.getIntervals().getData(), 1, 12);
    ASSERT_VECTOR_EQ(
        Vectors::fromBinary(transposed.getBinary().getData()).getPositions().getData(),
        normalize_positions(transposed.getPositions()));
    ASSERT_VECTOR_EQ(
        normalize_positions(transposed.getPositions()),
        Vectors::fromIntervals(transposed.getIntervals().getData()).getPositions().getData());

    Vectors rotated = source.rotatePositions(1);
    ASSERT_BINARY_VECTOR_EQ(
        Vectors::fromBinary(rotated.getBinary().getData()).getBinary(),
        rotated.getBinary().getData(),
        0,
        12);
    ASSERT_POSITION_VECTOR_EQ(rotated.rotatePositions(3).getPositions(), source.getPositions().getData(), 12);

    Vectors inverted = source.invertPositions(0);
    ASSERT_VECTOR_EQ(
        normalize_positions(inverted.invertPositions(0).getPositions()),
        source.getPositions().getData());
    ASSERT_VECTOR_EQ(
        normalize_positions(inverted.getPositions()),
        Vectors::fromIntervals(inverted.getIntervals().getData()).getPositions().getData());

    TEST_CASE_LOG("vectors_roundtrip_and_invariants");
    TEST_INPUT("source", source.getPositions());
    TEST_OUTPUT("from_intervals", Vectors::fromIntervals(source.getIntervals().getData()).getPositions());
    TEST_OUTPUT("from_binary", Vectors::fromBinary(source.getBinary().getData()).getPositions());
    TEST_OUTPUT("transposed", transposed.getPositions());
    TEST_OUTPUT("transposed_intervals", transposed.getIntervals());
    TEST_OUTPUT("rotated", rotated.getPositions());
    TEST_OUTPUT("rotated_binary", rotated.getBinary());
    TEST_OUTPUT("inverted", inverted.getPositions());
}

TEST_CASE("interval_vector_examples", "[vectors]") {
    IntervalVector iv1;
    ASSERT_INTERVAL_VECTOR_EQ(iv1, musicpp_test::ints({0}), 0, 12);

    IntervalVector iv2({2, 3, 5, 7}, 1, 12);
    ASSERT_EQ(iv2.size(), static_cast<size_t>(4));
    ASSERT_EQ(iv2.getOffset(), 1);
    ASSERT_EQ(iv2.getMod(), 12);
    ASSERT_TRUE(!iv2.empty());

    iv2.setOffset(3);
    iv2.setMod(24);
    iv2.setData({1, 4, 7});
    ASSERT_INTERVAL_VECTOR_EQ(iv2, musicpp_test::ints({1, 4, 7}), 3, 24);

    IntervalVector iv3({1, 2, 3, 4});
    ASSERT_INTERVAL_VECTOR_EQ(iv3 + 5, musicpp_test::ints({6, 7, 8, 9}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 - 2, musicpp_test::ints({-1, 0, 1, 2}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 * 3, musicpp_test::ints({3, 6, 9, 12}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 / 2, musicpp_test::ints({0, 1, 1, 2}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv3 % 3, musicpp_test::ints({1, 2, 0, 1}), 0, 12);

    IntervalVector iv4({1, 2, 3});
    IntervalVector iv5({4, 5, 6});
    ASSERT_INTERVAL_VECTOR_EQ(iv4 + iv5, musicpp_test::ints({5, 7, 9}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 - iv5, musicpp_test::ints({-3, -3, -3}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 * iv5, musicpp_test::ints({4, 10, 18}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 / iv5, musicpp_test::ints({0, 0, 0}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv4 % iv5, musicpp_test::ints({1, 2, 3}), 0, 12);

    IntervalVector iv8({10, 20, 30, 40});
    ASSERT_EQ(iv8[0], 10);
    ASSERT_EQ(iv8[-1], 40);

    IntervalVector iv12({1, 2, 3, 4, 5});
    ASSERT_INTERVAL_VECTOR_EQ(iv12.rotate(2), musicpp_test::ints({3, 4, 5, 1, 2}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv12.reverse(), musicpp_test::ints({5, 4, 3, 2, 1}), 0, 12);

    IntervalVector iv13({1, 2, 3, 4, 5, 6});
    ASSERT_INTERVAL_VECTOR_EQ(iv13.inversion(1), musicpp_test::ints({1, 6, 5, 4, 3, 2}), 0, 12);

    IntervalVector iv14({13, 25, -3, 14});
    ASSERT_INTERVAL_VECTOR_EQ(iv14.normalize(12), musicpp_test::ints({1, 1, 9, 2}), 0, 12);

    IntervalVector iv15({1, 2, 3});
    ASSERT_INTERVAL_VECTOR_EQ(iv15.componentwiseSum({4, 5}, true), musicpp_test::ints({5, 7, 7}), 0, 12);

    IntervalVector iv16({1, 2, 3});
    IntervalVector iv17({4, 5});
    ASSERT_INTERVAL_VECTOR_EQ(iv16.concatenate(iv17), musicpp_test::ints({1, 2, 3, 4, 5}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv16.repeat(3), musicpp_test::ints({1, 2, 3, 1, 2, 3, 1, 2, 3}), 0, 12);

    IntervalVector iv18({1, 2, 3, 4, 5, 6});
    ASSERT_INTERVAL_VECTOR_EQ(iv18.singleMirror(3, true), musicpp_test::ints({3, 2, 1, 4, 5, 6}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(iv18.doubleMirror(3), musicpp_test::ints({3, 2, 1, 6, 5, 4}), 0, 12);

    TEST_CASE_LOG("interval_vector_operations");
    TEST_INPUT("iv12", iv12);
    TEST_INPUT("iv13", iv13);
    TEST_OUTPUT("rotate_2", iv12.rotate(2));
    TEST_OUTPUT("reverse", iv12.reverse());
    TEST_OUTPUT("inversion_1", iv13.inversion(1));
    TEST_OUTPUT("normalize_12", iv14.normalize(12));
    TEST_OUTPUT("componentwise_sum", iv15.componentwiseSum({4, 5}, true));
    TEST_OUTPUT("concatenate", iv16.concatenate(iv17));
    TEST_OUTPUT("repeat_3", iv16.repeat(3));
    TEST_OUTPUT("single_mirror", iv18.singleMirror(3, true));
    TEST_OUTPUT("double_mirror", iv18.doubleMirror(3));
}

TEST_CASE("binary_vector_examples", "[vectors]") {
    BinaryVector bv1;
    ASSERT_BINARY_VECTOR_EQ(bv1, musicpp_test::ints({1, 0, 0, 0}), 0, 4);

    BinaryVector bv2({1, 0, 1, 1, 0}, 0, 5);
    ASSERT_EQ(bv2.countPulses(), 3);
    ASSERT_NEAR(bv2.density(), 0.6, 1e-6);

    BinaryVector bv7({1, 0, 1, 0, 1});
    ASSERT_BINARY_VECTOR_EQ(~bv7, musicpp_test::ints({0, 1, 0, 1, 0}), 0, 4);
    ASSERT_BINARY_VECTOR_EQ(bv7.complement(), musicpp_test::ints({0, 1, 0, 1, 0}), 0, 4);

    BinaryVector bv16({1, 0, 0, 1, 0});
    ASSERT_BINARY_VECTOR_EQ(bv16.rotate(1), musicpp_test::ints({0, 0, 1, 0, 1}), 0, 4);
    ASSERT_BINARY_VECTOR_EQ(bv16.rotate(2), musicpp_test::ints({0, 1, 0, 1, 0}), 0, 4);

    BinaryVector bv17({1, 0, 1, 0, 1});
    ASSERT_BINARY_VECTOR_EQ(bv17.inversion(0), musicpp_test::ints({1, 1, 0, 1, 0}), 0, 4);

    BinaryVector bv19({1, 0, 1});
    BinaryVector bv20({0, 1});
    ASSERT_BINARY_VECTOR_EQ(bv19.concatenate(bv20), musicpp_test::ints({1, 0, 1, 0, 1}), 0, 4);
    ASSERT_BINARY_VECTOR_EQ(bv19.repeat(3), musicpp_test::ints({1, 0, 1, 1, 0, 1, 1, 0, 1}), 0, 4);

    BinaryVector bv21({1, 0, 0, 1, 0, 1, 0, 0});
    ASSERT_EQ(bv21.countPulses(), 3);
    ASSERT_NEAR(bv21.density(), 0.375, 1e-6);
    ASSERT_VECTOR_EQ(bv21.getPulseIndices(), musicpp_test::ints({0, 3, 5}));
    ASSERT_VECTOR_EQ(bv21.getInterOnsetIntervals(), musicpp_test::ints({3, 2, 3}));

    ASSERT_BINARY_VECTOR_EQ(BinaryVector::euclidean(3, 8), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 0}), 0, -1);
    ASSERT_BINARY_VECTOR_EQ(BinaryVector::euclidean(5, 12), musicpp_test::ints({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0}), 0, -1);

    TEST_CASE_LOG("binary_vector_operations");
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
    TEST_OUTPUT("euclidean_3_8", BinaryVector::euclidean(3, 8));
    TEST_OUTPUT("euclidean_5_12", BinaryVector::euclidean(5, 12));
}

TEST_CASE("position_vector_examples", "[vectors]") {
    PositionVector pv1;
    ASSERT_POSITION_VECTOR_EQ(pv1, musicpp_test::ints({0}), 12);

    PositionVector pv2({0, 2, 4, 7, 9}, 12, 0, true, false);
    ASSERT_EQ(pv2.size(), static_cast<size_t>(5));
    ASSERT_EQ(pv2.getMod(), 12);
    ASSERT_EQ(pv2.getUserRange(), 12);
    ASSERT_EQ(pv2.getRange(), 12);
    ASSERT_TRUE(pv2.getRangeUpdate());
    ASSERT_TRUE(!pv2.getUser());

    PositionVector pv3({0, 3, 7, 10});
    ASSERT_POSITION_VECTOR_EQ(pv3 + 5, musicpp_test::ints({5, 8, 12, 15}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 - 2, musicpp_test::ints({-2, 1, 5, 8}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 * 2, musicpp_test::ints({0, 6, 14, 20}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 / 2, musicpp_test::ints({0, 1, 3, 5}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv3 % 3, musicpp_test::ints({0, 0, 1, 1}), 12);

    PositionVector pv12({0, 3, 7, 10});
    ASSERT_POSITION_VECTOR_EQ(pv12.rotate(1), musicpp_test::ints({10, 0, 3, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(pv12.rotate(2), musicpp_test::ints({7, 10, 0, 3}), 12);

    PositionVector pv14({0, 3, 7}, 12);
    ASSERT_POSITION_VECTOR_EQ(pv14.complement(), musicpp_test::ints({1, 2, 4, 5, 6, 8, 9, 10, 11}), 12);

    PositionVector pv15({0, 4, 7, 11});
    ASSERT_POSITION_VECTOR_EQ(pv15.inversion(0), musicpp_test::ints({-11, -7, -4, 0}), 12);

    PositionVector pv18({0, 3, 7});
    PositionVector pv19({10, 14});
    ASSERT_POSITION_VECTOR_EQ(pv18.concatenate(pv19), musicpp_test::ints({0, 3, 7, 10, 14}), 12);

    PositionVector pv22({0, 5, 12, 18}, 12, 0, true, false);
    ASSERT_EQ(pv22.getRange(), 24);

    TEST_CASE_LOG("position_vector_operations");
    TEST_INPUT("pv12", pv12);
    TEST_INPUT("pv14", pv14);
    TEST_INPUT("pv15", pv15);
    TEST_OUTPUT("rotate1", pv12.rotate(1));
    TEST_OUTPUT("rotate2", pv12.rotate(2));
    TEST_OUTPUT("complement", pv14.complement());
    TEST_OUTPUT("inversion0", pv15.inversion(0));
    TEST_OUTPUT("concatenate", pv18.concatenate(pv19));
    TEST_OUTPUT("range", pv22.getRange());
}

} // namespace
