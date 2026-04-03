#include "test_support.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <string>

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

TEST_CASE("scale_database_try_get_scale_by_string_keys", "[scale_dictionary]") {
    ScaleDatabase& db = ScaleDatabase::shared();
    musicpp::ScaleMatch m;
    const std::string sheet = "Major and minor scales";
    ASSERT_TRUE(db.tryGetScale(sheet, "Ionian (Major)", m));
    ASSERT_EQ(m.sheetName, sheet);
    std::vector<int> expected = {0, 2, 4, 5, 7, 9, 11};
    ASSERT_EQ(m.toPitchClasses(), expected);
}

TEST_CASE("scale_database_get_scale_names_in_major_sheet", "[scale_dictionary]") {
    ScaleDatabase db;
    auto names = db.getScaleNames("Major and minor scales");
    ASSERT_TRUE(std::find(names.begin(), names.end(), "Dorian") != names.end());
    auto sorted = names;
    std::sort(sorted.begin(), sorted.end());
    ASSERT_EQ(names, sorted);
}

TEST_CASE("scale_database_get_sheets_contains_expected_categories", "[scale_dictionary]") {
    ScaleDatabase db;
    const auto& sheets = db.getSheetNames();
    ASSERT_TRUE(std::find(sheets.begin(), sheets.end(), "Jazz Scales") != sheets.end());
    ASSERT_TRUE(std::find(sheets.begin(), sheets.end(), "Indian Scales") != sheets.end());
}

TEST_CASE("scale_database_shared_same_count_as_fresh_instance", "[scale_dictionary]") {
    ASSERT_EQ(ScaleDatabase().count(), ScaleDatabase::shared().count());
}

TEST_CASE("scale_database_map_indexer_matches_try_get", "[scale_dictionary]") {
    ScaleDatabase db;
    const auto key = std::make_pair(std::string("Jazz Scales"), std::string("Bebop"));
    auto it = db.scalesBySheetAndName().find(key);
    ASSERT_TRUE(it != db.scalesBySheetAndName().end());
    musicpp::ScaleMatch from_try;
    ASSERT_TRUE(db.tryGetScale(key.first, key.second, from_try));
    ASSERT_EQ(it->second.sheetName, from_try.sheetName);
    ASSERT_EQ(it->second.scaleName, from_try.scaleName);
    ASSERT_EQ(it->second.pitchClasses, from_try.pitchClasses);
}

} // namespace
