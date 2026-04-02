#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/matrix_distance.h"

#include <sstream>
#include <tuple>

namespace {

using namespace musicpp;

TEST_CASE("modal_matrix_distance_empty_throws", "[matrix_distance]") {
    ModalMatrixDistance<position_vector> empty;
    REQUIRE_THROWS_AS(empty.getClosest(), std::runtime_error);
    REQUIRE_THROWS_AS(empty.getFurthest(), std::runtime_error);
    REQUIRE_THROWS_AS(empty.getByComplexity(0), std::runtime_error);
}

TEST_CASE("modal_matrix_distance_sort_and_extractors", "[matrix_distance]") {
    position_vector a({0, 4, 7});
    position_vector b({0, 3, 7});
    std::vector<std::tuple<position_vector, int, double>> rows = {
        {a, 0, 2.5},
        {b, 1, 0.5},
    };
    ModalMatrixDistance<position_vector> mmd(rows);
    mmd.sortByDistance();
    ASSERT_NEAR(std::get<2>(mmd[0]), 0.5, 1e-9);
    auto vecs = mmd.getVectors();
    ASSERT_EQ(vecs.size(), static_cast<size_t>(2));
    ASSERT_VECTOR_EQ(vecs[0].data(), b.data());
    ASSERT_EQ(mmd.getIndices()[0], 1);
    ASSERT_NEAR(mmd.getDistances()[1], 2.5, 1e-9);
    auto closest = mmd.getClosest();
    ASSERT_NEAR(std::get<2>(closest), 0.5, 1e-9);
}

TEST_CASE("modal_matrix_row_accessors", "[matrix_distance]") {
    position_vector v({1, 2, 3});
    ModalMatrixRow<position_vector> row(v, 4, 1.25);
    ASSERT_EQ(row.getIndex(), 4);
    ASSERT_NEAR(row.getDistance(), 1.25, 1e-9);
    ASSERT_VECTOR_EQ(row.getVector().data(), v.data());
    auto tup = row.toTuple();
    ASSERT_NEAR(std::get<2>(tup), 1.25, 1e-9);
    std::ostringstream oss;
    oss << row;
    ASSERT_TRUE(oss.str().find("Mode = 4") != std::string::npos);
}

TEST_CASE("transposition_matrix_row_accessors", "[matrix_distance]") {
    position_vector v({0, 4, 7});
    TranspositionMatrixRow row(v, 5, 0.0);
    ASSERT_EQ(row.getTransposition(), 5);
    ASSERT_POSITION_VECTOR_EQ(row.getVector(), musicpp_test::ints({0, 4, 7}), 12);
}

TEST_CASE("rototranslation_matrix_row_accessors", "[matrix_distance]") {
    position_vector v({0, 4, 7});
    RototranslationMatrixRow row(v, 3, 1.5, -1);
    ASSERT_EQ(row.getTranslation(), 3);
    ASSERT_EQ(row.getCenter(), -1);
    ASSERT_NEAR(row.getDistance(), 1.5, 1e-9);
}

TEST_CASE("modal_selection_matrix_row_accessors", "[matrix_distance]") {
    position_vector c({0, 4, 7});
    ModalSelectionMatrixRow<position_vector> row(c, 2, 4.0);
    ASSERT_EQ(row.getModeIndex(), 2);
    ASSERT_NEAR(row.getDistance(), 4.0, 1e-9);
}

TEST_CASE("transposition_matrix_distance_complexity_bounds", "[matrix_distance]") {
    TranspositionMatrixDistance tmd;
    REQUIRE_THROWS_AS(tmd.getByComplexity(-1), std::runtime_error);
    REQUIRE_THROWS_AS(tmd.getByComplexity(101), std::runtime_error);
}

TEST_CASE("modal_rototranslation_matrix_distance_empty_throws", "[matrix_distance]") {
    ModalRototranslationMatrixDistance empty;
    REQUIRE_THROWS_AS(empty.getClosest(), std::runtime_error);
    REQUIRE_THROWS_AS(empty.getFurthest(), std::runtime_error);
    REQUIRE_THROWS_AS(empty.getByComplexity(0), std::runtime_error);
}

TEST_CASE("modal_rototranslation_matrix_distance_complexity_bounds", "[matrix_distance]") {
    position_vector v({0, 4, 7}, 12, 12);
    std::vector<std::tuple<int, int, position_vector, double>> rows = {
        {0, 0, v, 1.0},
    };
    ModalRototranslationMatrixDistance mrd(rows);
    REQUIRE_THROWS_AS(mrd.getByComplexity(-1), std::runtime_error);
    REQUIRE_THROWS_AS(mrd.getByComplexity(101), std::runtime_error);
}

TEST_CASE("modal_rototranslation_matrix_distance_sort_and_closest", "[matrix_distance]") {
    position_vector a({0, 4, 7}, 12, 12);
    position_vector b({1, 5, 8}, 12, 12);
    std::vector<std::tuple<int, int, position_vector, double>> rows = {
        {0, 0, a, 3.0},
        {1, 2, b, 0.5},
    };
    ModalRototranslationMatrixDistance mrd(rows);
    mrd.sortByDistance();
    ASSERT_NEAR(std::get<3>(mrd[0]), 0.5, 1e-9);
    auto closest = mrd.getClosest();
    ASSERT_NEAR(closest.getDistance(), 0.5, 1e-9);
    ASSERT_EQ(closest.getModeIndex(), 1);
    ASSERT_EQ(closest.getTranslationIndex(), 2);
}

TEST_CASE("modal_rototranslation_matrix_row_accessors", "[matrix_distance]") {
    position_vector v({0, 4, 7}, 12, 12);
    ModalRototranslationMatrixRow row(5, -1, v, 2.25);
    ASSERT_EQ(row.getModeIndex(), 5);
    ASSERT_EQ(row.getTranslationIndex(), -1);
    ASSERT_NEAR(row.getDistance(), 2.25, 1e-9);
    ASSERT_POSITION_VECTOR_EQ(row.getVector(), musicpp_test::ints({0, 4, 7}), 12);
    std::ostringstream oss;
    oss << row;
    ASSERT_TRUE(oss.str().find("Degree = 5") != std::string::npos);
}

} // namespace
