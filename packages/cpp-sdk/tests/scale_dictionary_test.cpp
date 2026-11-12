#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/scale_dictionary.h"

namespace {

using musicpp::ScaleDatabase;

TEST_CASE("scale_database_major_pitch_classes_find_scale", "[scale_dictionary]") {
    ScaleDatabase db;
    std::vector<int> ionian_pc = {60, 62, 64, 65, 67, 69, 71};
    auto matches = db.findScale(ionian_pc);
    ASSERT_TRUE(!matches.empty());
}

TEST_CASE("scale_database_empty_input_no_matches", "[scale_dictionary]") {
    ScaleDatabase db;
    auto matches = db.findScale({});
    ASSERT_TRUE(matches.empty());
}

TEST_CASE("scale_database_get_all_interval_sets_has_ionian", "[scale_dictionary]") {
    ScaleDatabase db;
    auto sets = db.getAllIntervalSets();
    std::vector<int> ionian_sorted = {0, 2, 4, 5, 7, 9, 11};
    ASSERT_TRUE(sets.count(ionian_sorted) > 0);
}

} // namespace
