#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/matrix_distance.h"

#include <sstream>
#include <tuple>

namespace {

using namespace musicpp;

TEST_CASE("print_matrix_distance_modal_position_smoke", "[matrix][print]") {
    position_vector a({0, 4, 7}, 12, 12);
    ModalMatrixDistance<position_vector> mmd({{a, 0, 1.5}});
    std::ostringstream oss;
    printMatrixDistance(mmd, oss);
    const std::string s = oss.str();
    ASSERT_TRUE(s.find("Row") != std::string::npos);
    ASSERT_TRUE(s.find("Distance") != std::string::npos);
    ASSERT_TRUE(s.find("1.5") != std::string::npos);
}

TEST_CASE("print_matrix_distance_transposition_smoke", "[matrix][print]") {
    position_vector v({0, 4, 7}, 12, 12);
    TranspositionMatrixDistance tmd({{v, 3, 0.0}});
    std::ostringstream oss;
    printMatrixDistance(tmd, oss);
    ASSERT_TRUE(oss.str().find("Transposition") != std::string::npos);
}

TEST_CASE("print_matrix_distance_relative_mode_smoke", "[matrix][print]") {
    position_vector v({0, 4, 7}, 12, 12);
    RelativeModeMatrixDistance rmd({{v, 0, 2.0}}, 5);
    std::ostringstream oss;
    printMatrixDistance(rmd, oss);
    const std::string s = oss.str();
    ASSERT_TRUE(s.find("Center") != std::string::npos);
    ASSERT_TRUE(s.find('5') != std::string::npos);
}

TEST_CASE("print_matrix_distance_modal_relative_mode_smoke", "[matrix][print]") {
    position_vector v({0, 4, 7}, 12, 12);
    ModalRelativeModeMatrixDistance mrmd({{1, -2, v, 4.5}});
    std::ostringstream oss;
    printMatrixDistance(mrmd, oss);
    ASSERT_TRUE(oss.str().find("Degree") != std::string::npos);
}

TEST_CASE("print_matrix_distance_modal_selection_interval_smoke", "[matrix][print]") {
    interval_vector iv({2, 2, 3}, 0, 12);
    ModalSelectionMatrixDistance<interval_vector> msd({{iv, 2, 1.0}});
    std::ostringstream oss;
    printMatrixDistance(msd, oss);
    ASSERT_TRUE(oss.str().find("Chord") != std::string::npos);
}

TEST_CASE("print_matrix_row_modal_in_and_out_of_range", "[matrix][print]") {
    position_vector a({0, 4, 7}, 12, 12);
    ModalMatrixDistance<position_vector> mmd({{a, 1, 2.0}});
    std::ostringstream ok;
    printMatrixRow(mmd, 0, ok);
    ASSERT_TRUE(ok.str().find("idx=") != std::string::npos);
    std::ostringstream bad;
    printMatrixRow(mmd, 99, bad);
    ASSERT_TRUE(bad.str().find("out of range") != std::string::npos);
}

TEST_CASE("print_transposition_row_out_of_range", "[matrix][print]") {
    position_vector v({0, 4, 7}, 12, 12);
    TranspositionMatrixDistance tmd({{v, 0, 0.0}});
    std::ostringstream oss;
    printTranspositionRow(tmd, 10, oss);
    ASSERT_TRUE(oss.str().find("out of range") != std::string::npos);
}

TEST_CASE("print_tuple_T_int_double_interval", "[matrix][print]") {
    interval_vector iv({2, 2, 3}, 0, 12);
    std::tuple<interval_vector, int, double> t = {iv, 2, 3.5};
    std::ostringstream oss;
    print_tuple_T_int_double(t, oss);
    ASSERT_TRUE(oss.str().find("dist=3.5") != std::string::npos);
}

TEST_CASE("tuple_T_int_double_to_string_position", "[matrix][print]") {
    position_vector v({1, 2}, 12, 12);
    auto s = tuple_T_int_double_to_string(std::make_tuple(v, 7, 0.25));
    ASSERT_TRUE(s.find("idx=7") != std::string::npos);
    ASSERT_TRUE(s.find("dist=0.25") != std::string::npos);
}

TEST_CASE("print_relative_mode_row_ok_and_out_of_range", "[matrix][print]") {
    position_vector v({0, 4, 7}, 12, 12);
    RelativeModeMatrixDistance rmd({{v, 3, 1.0}}, 0);
    std::ostringstream ok;
    print_relative_mode_row(rmd, 0, ok);
    ASSERT_TRUE(ok.str().find("idx=3") != std::string::npos);
    std::ostringstream bad;
    print_relative_mode_row(rmd, 3, bad);
    ASSERT_TRUE(bad.str().find("out of range") != std::string::npos);
}

TEST_CASE("print_selection_row_position_ok_and_out_of_range", "[matrix][print]") {
    position_vector c({0, 4, 7}, 12, 12);
    ModalSelectionMatrixDistance<position_vector> msd({{c, 4, 0.5}});
    std::ostringstream ok;
    printSelectionRow(msd, 0, ok);
    ASSERT_TRUE(ok.str().find("idx=4") != std::string::npos);
    std::ostringstream bad;
    printSelectionRow(msd, 5, bad);
    ASSERT_TRUE(bad.str().find("out of range") != std::string::npos);
}

TEST_CASE("print_modal_relative_mode_row_ok_and_out_of_range", "[matrix][print]") {
    position_vector v({0, 4, 7}, 12, 12);
    ModalRelativeModeMatrixDistance mrmd({{2, 1, v, 8.0}});
    std::ostringstream ok;
    print_modal_relative_mode_row(mrmd, 0, ok);
    ASSERT_TRUE(ok.str().find("mode=2") != std::string::npos);
    ASSERT_TRUE(ok.str().find("rel_mode=1") != std::string::npos);
    std::ostringstream bad;
    print_modal_relative_mode_row(mrmd, 9, bad);
    ASSERT_TRUE(bad.str().find("out of range") != std::string::npos);
}

TEST_CASE("tuple_int_int_PV_double_string_and_print", "[matrix][print]") {
    position_vector v({0, 4, 7}, 12, 12);
    auto t = std::make_tuple(1, -3, v, 0.125);
    auto s = tuple_int_int_PV_double_to_string(t);
    ASSERT_TRUE(s.find("mode=1") != std::string::npos);
    ASSERT_TRUE(s.find("rel_mode=-3") != std::string::npos);
    ASSERT_TRUE(s.find("dist=0.125") != std::string::npos);
    std::ostringstream oss;
    print_tuple_int_int_PV_double(t, oss);
    ASSERT_TRUE(oss.str().find("dist=0.125") != std::string::npos);
}

} // namespace
