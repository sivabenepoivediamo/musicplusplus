cmake_minimum_required(VERSION 3.16)
project(musicplusplus)

# Define the header-only library
add_library(musicplusplus INTERFACE)
target_include_directories(musicplusplus INTERFACE src)
target_compile_features(musicplusplus INTERFACE cxx_std_17)

# Strict warnings for consumers of the header-only SDK (does not affect FetchContent targets like Catch2).
if(MSVC)
    target_compile_options(musicplusplus INTERFACE /W4 /permissive-)
else()
    # -Wno-c2y-extensions must follow -Wpedantic (Catch2 / TEST_CASE uses __COUNTER__).
    target_compile_options(musicplusplus INTERFACE
        -Wall -Wextra -Wpedantic
        $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-c2y-extensions>)
endif()

# Enable C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(CATCH_INSTALL_HELPERS OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

function(add_musicpp_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE musicplusplus Catch2::Catch2WithMain)
    target_include_directories(${test_name} PRIVATE tests)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_musicpp_test(vectors_test tests/vectors_test.cpp)
add_musicpp_test(chord_test tests/chord_test.cpp)
add_musicpp_test(scale_test tests/scale_test.cpp)
add_musicpp_test(automation_test tests/automation_test.cpp)
add_musicpp_test(matrix_test tests/matrix_test.cpp)
add_musicpp_test(selection_test tests/selection_test.cpp)
add_musicpp_test(analysis_test tests/analysis_test.cpp)
add_musicpp_test(rhythm_test tests/rhythm_test.cpp)
add_musicpp_test(note_names_test tests/note_names_test.cpp)
add_musicpp_test(slonimsky_test tests/slonimsky_test.cpp)
add_musicpp_test(math_util_test tests/math_util_test.cpp)
add_musicpp_test(distances_util_test tests/distances_util_test.cpp)
add_musicpp_test(quantize_transpose_test tests/quantize_transpose_test.cpp)
add_musicpp_test(measures_util_test tests/measures_util_test.cpp)
add_musicpp_test(melody_test tests/melody_test.cpp)
add_musicpp_test(scale_dictionary_test tests/scale_dictionary_test.cpp)
add_musicpp_test(chord_names_util_test tests/chord_names_util_test.cpp)
add_musicpp_test(matrix_row_test tests/matrix_row_test.cpp)
add_musicpp_test(automation_helpers_test tests/automation_helpers_test.cpp)
add_musicpp_test(vectors_conversions_test tests/vectors_conversions_test.cpp)
add_musicpp_test(vector_division_safety_test tests/vector_division_safety_test.cpp)
add_musicpp_test(matrix_calculate_distances_test tests/matrix_calculate_distances_test.cpp)
add_musicpp_test(rhythm_util_test tests/rhythm_util_test.cpp)
add_musicpp_test(position_vector_extended_test tests/position_vector_extended_test.cpp)
add_musicpp_test(interval_binary_extended_test tests/interval_binary_extended_test.cpp)
add_musicpp_test(matrix_filter_rototranslation_test tests/matrix_filter_rototranslation_test.cpp)
add_musicpp_test(automation_extended_test tests/automation_extended_test.cpp)
add_musicpp_test(matrix_print_test tests/matrix_print_test.cpp)


