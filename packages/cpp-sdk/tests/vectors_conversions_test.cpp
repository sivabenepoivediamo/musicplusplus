#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/vectors.h"

namespace {

using musicpp::binary_vector;
using musicpp::interval_vector;
using musicpp::intervals_to_positions;
using musicpp::position_vector;
using musicpp::positions_to_binary;
using musicpp::positions_to_intervals;
using musicpp::vector_set;

TEST_CASE("positions_to_intervals_empty_and_singleton", "[vectors][conversion]") {
    position_vector empty_pv;
    auto iv_empty = positions_to_intervals(empty_pv);
    ASSERT_EQ(iv_empty.data().size(), static_cast<size_t>(0));
    ASSERT_EQ(iv_empty.offset(), 0);
    ASSERT_EQ(iv_empty.mod(), 12);

    position_vector one({5}, 12, 12);
    auto iv_one = positions_to_intervals(one);
    ASSERT_EQ(iv_one.data().size(), static_cast<size_t>(0));
    ASSERT_EQ(iv_one.offset(), 5);
}

TEST_CASE("positions_to_intervals_closure", "[vectors][conversion]") {
    position_vector triad({0, 4, 7}, 12, 12);
    auto iv = positions_to_intervals(triad);
    ASSERT_INTERVAL_VECTOR_EQ(iv, musicpp_test::ints({4, 3, 5}), 0, 12);
}

TEST_CASE("intervals_to_positions_roundtrip", "[vectors][conversion]") {
    interval_vector iv({2, 2, 1}, 0, 12);
    position_vector pv = intervals_to_positions(iv);
    ASSERT_POSITION_VECTOR_EQ(pv, musicpp_test::ints({0, 2, 4}), 12);

    interval_vector empty_iv(std::vector<int>{}, 3, 12);
    position_vector pv0 = intervals_to_positions(empty_iv);
    ASSERT_POSITION_VECTOR_EQ(pv0, musicpp_test::ints({3}), 12);
}

TEST_CASE("positions_to_binary_and_roundtrip_positions", "[vectors][conversion]") {
    position_vector empty(std::vector<int>{}, 12, 12, true, false);
    auto bin = positions_to_binary(empty);
    ASSERT_EQ(bin.data().size(), static_cast<size_t>(0));

    position_vector triad({0, 4, 7}, 12, 12);
    auto b = positions_to_binary(triad);
    ASSERT_TRUE(b.countPulses() == 3);

    auto iv_back = positions_to_intervals(triad);
    position_vector pv_back = intervals_to_positions(iv_back);
    ASSERT_VECTOR_EQ(pv_back.data(), triad.data());
}

TEST_CASE("vector_set_constructors_and_equality", "[vectors][conversion]") {
    vector_set def;
    ASSERT_EQ(def.mod(), 12);
    ASSERT_POSITION_VECTOR_EQ(def.positions(), musicpp_test::ints({0}), 12);

    vector_set mod7(7);
    ASSERT_EQ(mod7.mod(), 7);

    vector_set a = vector_set::from_positions({0, 3, 7});
    vector_set b = vector_set::from_positions({0, 3, 7});
    vector_set c = vector_set::from_positions({0, 4, 7});
    ASSERT_TRUE(a == b);
    ASSERT_TRUE(a != c);
    ASSERT_TRUE(!(a != b));
}

TEST_CASE("vector_set_from_interval_and_binary", "[vectors][conversion]") {
    vector_set from_iv(vector_set::from_intervals({2, 2, 1}, 12));
    ASSERT_POSITION_VECTOR_EQ(from_iv.positions(), musicpp_test::ints({0, 2, 4}), 12);

    vector_set from_bv(vector_set::from_binary({1, 0, 1, 0, 1}, 0, 5));
    ASSERT_POSITION_VECTOR_EQ(from_bv.positions(), musicpp_test::ints({0, 2, 4}), 5);
}

TEST_CASE("vector_set_inversion_alias_matches_rototranslate", "[vectors][conversion]") {
    vector_set s = vector_set::from_positions({0, 4, 7});
    ASSERT_TRUE(s.inversion(2, 0).positions().data() == s.roto_translate_positions(2, 0).positions().data());
}

TEST_CASE("vector_set_transpose_multiply_complement", "[vectors][conversion]") {
    vector_set s = vector_set::from_positions({0, 4, 7});
    auto t = s.transpose(3);
    ASSERT_EQ(t.mod(), 12);
    ASSERT_POSITION_VECTOR_EQ(t.positions(), musicpp_test::ints({3, 7, 10}), 12);

    auto m = s.multiply_positions(2);
    ASSERT_POSITION_VECTOR_EQ(m.positions(), musicpp_test::ints({0, 8, 14}), 12);

    auto c = s.complement_positions();
    ASSERT_EQ(c.positions().size(), static_cast<size_t>(9));
    ASSERT_EQ(c.mod(), 12);
}

TEST_CASE("vector_set_mode_rotates_intervals", "[vectors][conversion]") {
    vector_set s = vector_set::from_intervals({2, 2, 1}, 12);
    auto r = s.mode(1);
    ASSERT_INTERVAL_VECTOR_EQ(r.intervals(), musicpp_test::ints({2, 1, 2}), 0, 12);
}

TEST_CASE("vector_set_binary_or_and_and", "[vectors][conversion]") {
    vector_set a = vector_set::from_binary({1, 0, 1, 0, 0}, 0, 5);
    vector_set b = vector_set::from_binary({0, 1, 1, 0, 0}, 0, 5);
    vector_set u = a | b;
    vector_set n = a & b;
    ASSERT_EQ(u.binary().countPulses(), 3);
    ASSERT_EQ(n.binary().countPulses(), 1);
}

TEST_CASE("vector_set_reverse_intervals", "[vectors][conversion]") {
    vector_set s = vector_set::from_intervals({2, 2, 1}, 12);
    auto rev = s.reverse_intervals();
    ASSERT_INTERVAL_VECTOR_EQ(rev.intervals(), musicpp_test::ints({1, 2, 2}), 0, 12);
}

TEST_CASE("vector_set_add_to_intervals", "[vectors][conversion]") {
    vector_set s = vector_set::from_intervals({2, 2, 1}, 12);
    auto t = s.add_to_intervals(1);
    ASSERT_INTERVAL_VECTOR_EQ(t.intervals(), musicpp_test::ints({3, 3, 2}), 0, 12);
}

TEST_CASE("vector_set_binary_xor", "[vectors][conversion]") {
    vector_set a = vector_set::from_binary({1, 0, 1, 0, 0}, 0, 5);
    vector_set b = vector_set::from_binary({1, 1, 0, 0, 0}, 0, 5);
    vector_set x = a ^ b;
    ASSERT_EQ(x.binary().countPulses(), 2);
}

TEST_CASE("vector_set_invert_positions_preserves_size", "[vectors][conversion]") {
    vector_set s = vector_set::from_positions({0, 4, 7});
    auto inv = s.invert_positions(0);
    ASSERT_EQ(inv.positions().size(), static_cast<size_t>(3));
    ASSERT_EQ(inv.mod(), 12);
}

TEST_CASE("vector_set_multiply_intervals", "[vectors][conversion]") {
    vector_set s = vector_set::from_intervals({2, 2, 1}, 12);
    auto m = s.multiply_intervals(2);
    ASSERT_INTERVAL_VECTOR_EQ(m.intervals(), musicpp_test::ints({4, 4, 2}), 0, 12);
}

TEST_CASE("vector_set_rotate_binary_preserves_pulse_count", "[vectors][conversion]") {
    vector_set s = vector_set::from_binary({1, 0, 0, 1, 0}, 0, 5);
    auto r = s.rotate_binary(2);
    ASSERT_EQ(r.binary().mod(), 5);
    ASSERT_EQ(r.binary().size(), static_cast<size_t>(5));
    ASSERT_EQ(r.binary().countPulses(), s.binary().countPulses());
}

TEST_CASE("vector_set_complement_binary", "[vectors][conversion]") {
    vector_set s = vector_set::from_binary({1, 0, 0, 1, 0}, 0, 5);
    auto c = s.complement_binary();
    ASSERT_EQ(c.binary().mod(), 5);
    ASSERT_EQ(c.binary().countPulses() + s.binary().countPulses(), 5);
}

TEST_CASE("vector_set_invert_intervals", "[vectors][conversion]") {
    vector_set s = vector_set::from_intervals({2, 2, 1}, 12);
    auto iv = s.invert_intervals(1);
    ASSERT_EQ(iv.intervals().size(), static_cast<size_t>(3));
    ASSERT_EQ(iv.intervals().mod(), 12);
}

} // namespace
