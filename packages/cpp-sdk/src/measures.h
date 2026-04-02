#ifndef MUSICPP_MEASURES_H
#define MUSICPP_MEASURES_H

#include "vectors.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

/**
 * @file measures.h
 * @brief Collection of analysis and measure utilities for position_vector and related types
 *
 * This header provides a variety of music-theoretic and rhythmic measures such as
 * geodesic distances, distribution spectra, symmetry checks, entropy and generators, useful for analysis. Functions are intended to be
 * small, self-contained utilities that operate on `position_vector`, `interval_vector`,
 * `onset_vector` and plain integer vectors.
 */

/**
 * @brief Compute successive differences of an integer vector
 *
 * @param in Input integer vector
 * @return Vector of differences where out[i] = in[i+1] - in[i]
 * @note Returns an empty vector if input has less than two elements
 */

namespace musicpp {

inline std::vector<int> differences(std::vector<int>& in) {
    std::vector<int> out;

    if (in.size() < 2) {
        return out;
    }

    for (size_t i = 1; i < in.size(); ++i) {
        out.push_back(in[i] - in[i - 1]);
    }

    return out;
}

/**
 * @brief Compute the shortest distance between two points on a cyclic space
 *
 * Computes the geodesic distance between `a` and `b` under modulus `mod`.
 * The distance returned is the minimal step distance around the cycle.
 *
 * @param a First position
 * @param b Second position
 * @param mod Modulus (cycle length)
 * @return Minimal distance between a and b on the cycle
 */
inline int geodesicDistance(int a, int b, int mod) {
    int distance = (b - a + mod) % mod;
    if (distance > mod / 2) 
        distance = mod - distance;
    return distance;
}

/**
 * @brief Compute all pairwise geodesic distances for a position_vector
 *
 * @param in Input position_vector
 * @return Flattened vector of pairwise geodesic distances (i<j order)
 */
inline std::vector<int> geodesicDistances(position_vector& in) {
    std::vector<int> distances;
    for (size_t i = 0; i < in.size(); ++i) {
        for (size_t j = i + 1; j < in.size(); ++j) {
            int distance = geodesicDistance(in[i], in[j], in.mod());
            distances.push_back(distance);
        }
    }
    return distances;
}

/**
 * @brief Test whether an interval sequence is an Euclidean rhythm
 *
 * The function checks whether the provided `position_vector` (converted to
 * intervals) corresponds to an Euclidean rhythm by applying a simple rotation
 * and equality test.
 *
 * @param in position_vector to test (positions will be converted to intervals)
 * @param mod Modulus used for normalization (not always required)
 * @return true if the interval vector is Euclidean, false otherwise
 */
inline bool isEuclidean(position_vector in, int mod) {
    
    interval_vector j = positions_to_intervals(in);
    std::vector<int> temp = j.data();
    std::vector<int> temp2 = temp;
    int n = temp.size();
    if (n == 0) return false;
    
    temp[0]++;
    temp[n-1]--;

    for (int i = 0; i < n; ++i) {
        if (temp == temp2) return true;
        std::rotate(temp.begin(), temp.begin() + 1, temp.end());
    }

    return false;
}

/**
 * @brief Count occurrences of integer values in a vector
 *
 * @param in Input integer vector
 * @return map where key = value from `in` and value = frequency
 */
inline std::map<int, int> calculateOccurrences(std::vector<int>& in) {
    std::map<int, int> occurrences;
    for (int occurrence : in) {
        occurrences[occurrence]++;
    }
    return occurrences;
}

/**
 * @brief Test Winograd-deep property for an occurrence map
 *
 * A vector is Winograd-deep if for each k in 1..(size-1) there is at least one
 * distance that occurs exactly k times, and all such occurrence counts are unique.
 *
 * @param in Map of occurrences (distance -> frequency)
 * @param size Number of tones (n)
 * @return true if Winograd-deep, false otherwise
 */
inline bool isWinogradDeep(std::map<int, int>& in, int size) {
    std::set<int> counts;
    for (auto& entry : in) {
        int count = entry.second;
        if (count < 1 || count >= size) continue;
        if (!counts.insert(count).second) return false;
    }
    return static_cast<int>(counts.size()) == size - 1;
}

/**
 * @brief Test Erdos-deep property for an occurrence map
 *
 * Erdos-deep requires that all frequencies (values of the map) are unique.
 *
 * @param in Map of occurrences (distance -> frequency)
 * @return true if Erdos-deep, false otherwise
 */
inline bool isErdosDeep(std::map<int, int>& in) {
    std::set<int> seen;
    for (auto& pair : in) {
        if (!seen.insert(pair.second).second) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Compute a simple regression-based evenness measure for a rhythm
 *
 * Measures how far event positions deviate from perfectly even spacing over the
 * given total time units. Lower values indicate more even distribution.
 *
 * @param rhythm Vector of event positions (in time units)
 * @param totalTimeUnits Total cycle length (e.g., steps)
 * @return Sum of absolute deviations from ideal equally spaced positions.
 *         Returns `0.0` when @p rhythm is empty (no events to space).
 */
inline double calculateRegressionEvenness(std::vector<int>& rhythm, int totalTimeUnits) {
    int numNotes = static_cast<int>(rhythm.size());
    if (numNotes == 0) {
        return 0.0;
    }
    double idealInterval = static_cast<double>(totalTimeUnits) / numNotes;

    std::vector<double> idealPositions(static_cast<size_t>(numNotes));
    for (int i = 0; i < numNotes; ++i) {
        idealPositions[static_cast<size_t>(i)] = i * idealInterval;
    }

    std::vector<double> deviations(static_cast<size_t>(numNotes));
    for (int i = 0; i < numNotes; ++i) {
        deviations[static_cast<size_t>(i)] =
            std::abs(static_cast<double>(rhythm[static_cast<size_t>(i)]) -
                     idealPositions[static_cast<size_t>(i)]);
    }

    double regressionEvenness = std::accumulate(deviations.begin(), deviations.end(), 0.0);
    return regressionEvenness;
}

/**
 * @brief Calculate rhythmic oddity: number of antipodal pairs
 *
 * Counts pairs of onsets whose distances around the cycle are exactly equal
 * (i.e., they are opposite points on the circle). This is a simple measure
 * related to symmetry and antipodal structure.
 *
 * @param in position_vector of onsets
 * @return Integer count of antipodal pairs
 */
inline int calculateRhythmicOddity(position_vector& in) {
    int k = in.size();
    int rhythmic_oddity = 0;

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            int dist1 = (in[j] - in[i] + in.mod()) % in.mod();
            int dist2 = in.mod() - dist1;
            
            if (dist1 == dist2) {
                rhythmic_oddity++;
            }
        }
    }

    return rhythmic_oddity;
}


/**
 * @brief Compute transition complexity of onsets (number of edge changes)
 *
 * Converts the position vector into an onset pattern and counts the
 * number of transitions between 0 and 1 (i.e., on/offs). Useful as a
 * simple rhythmic complexity metric.
 *
 * @param in position_vector of onsets
 * @param mod Modulus (used internally by conversion routines)
 * @return Number of transitions in the onset pattern
 */
inline int computeTransitionComplexity(position_vector& in, int mod) {
    onset_vector onset_vec = positions_to_onset(in);
    if (onset_vec.size() == 0) {
        return 0;
    }

    int complexity = 0;
    for (size_t i = 1; i < onset_vec.size(); ++i) {
        if (onset_vec[i] != onset_vec[i - 1]) {
            ++complexity;
        }
    }
    if (onset_vec[onset_vec.size() - 1] != onset_vec[0]) {
        ++complexity;
    }

    return complexity;
}

/**
 * @brief Estimate Shannon entropy of the onset pattern
 *
 * Converts positions to an onset vector and computes a simple Shannon
 * entropy over the distribution of events. For sparse patterns this is a
 * coarse measure.
 *
 * @param in position_vector of onsets
 * @return Entropy in bits (base-2). Returns 0.0 for empty inputs.
 */
inline double computeEntropy(position_vector& in) {
    onset_vector onset_vec = positions_to_onset(in);
    if (onset_vec.size() == 0) {
        return 0.0;
    }

    std::unordered_map<int, int> frequency;
    for (int i = 0; i < static_cast<int>(onset_vec.size()); ++i) {
        ++frequency[onset_vec[i]];
    }

    double entropy = 0.0;
    for (auto& pair : frequency) {
        double probability = static_cast<double>(pair.second) / onset_vec.size();
        entropy -= probability * std::log2(probability);
    }

    return entropy;
}

/**
 * @brief Compute the length of the longest run of identical onset-vector values
 *
 * After converting positions to an onset vector, returns the maximum
 * length of a consecutive run of identical values (useful for measuring
 * clustering or gaps).
 *
 * @param in position_vector of onsets
 * @return Length of the longest subsequence
 */
inline int computeLongestSubsequence(position_vector& in) {
    onset_vector onset_vec = positions_to_onset(in);
    if (onset_vec.size() == 0) {
        return 0;
    }

    int longest = 1;
    int currentRun = 1;

    for (size_t i = 1; i < onset_vec.size(); ++i) {
        if (onset_vec[i] == onset_vec[i - 1]) {
            ++currentRun;
            if (currentRun > longest) {
                longest = currentRun;
            }
        } else {
            currentRun = 1;
        }
    }

    return longest;
}

/**
 * @brief Print pairwise distances between positions with labels
 *
 * @param in position_vector of positions
 * @param distances Flattened vector of distances in i<j order (same order as produced by geodesicDistances)
 */
inline void printDistances(position_vector& in, std::vector<int>& distances) {
    size_t index = 0;
    for (size_t i = 0; i < in.size(); ++i) {
        for (size_t j = i + 1; j < in.size(); ++j) {
            std::cout << "Distance between " << in[i] << " and " << in[j] << " is: " << distances[index++] << std::endl;
        }
    }
}

/**
 * @brief Pretty-print occurrence counts
 *
 * @param occurrences Map from value to frequency
 */
inline void printOccurrences(std::map<int, int>& occurrences) {
    for (auto& pair : occurrences) {
        std::cout << "Distance " << pair.first << " appears " << pair.second << " times" << std::endl;
    }
}

/**
 * @brief Print deepness classification (Winograd / Erdos)
 *
 * @param occurrences Map from distance to frequency
 * @param size Number of tones in the original set
 */
inline void printDeepness(std::map<int, int>& occurrences, int size) {
    bool winogradDeep = isWinogradDeep(occurrences, size);
    bool erdosDeep = isErdosDeep(occurrences);

    std::cout << "The vector is " << (winogradDeep ? "" : "not ") << "Winograd-deep" << std::endl;
    std::cout << "The vector is " << (erdosDeep ? "" : "not ") << "Erdos-deep" << std::endl;
}

/**
 * @brief Calculate distribution spectra for a scale
 *
 * For each generic interval (1..n-1) the function collects the set of
 * specific intervals that occur at that generic distance across the scale.
 *
 * @param in Input position_vector (scale)
 * @return Vector of sets where element k-1 contains the specific intervals for generic interval k
 */
inline std::vector<std::set<int>> calculateDistributionSpectra(position_vector& in) {
    std::vector<int> normalizedScale = in.data();
    if (normalizedScale.empty()) {
        return std::vector<std::set<int>>{};
    }
    
    std::vector<std::set<int>> distributionSpectra(normalizedScale.size() - 1);

    for (size_t i = 0; i < normalizedScale.size(); ++i) {
        for (size_t j = 1; j < normalizedScale.size(); ++j) {
            int genericInterval = j;
            int specificInterval = (normalizedScale[(i + j) % normalizedScale.size()] - normalizedScale[i] + in.mod()) % in.mod();
            distributionSpectra[genericInterval - 1].insert(specificInterval);
        }
    }
    
    return distributionSpectra;
}

/**
 * @brief Compute widths (max-min) of each distribution spectrum
 *
 * @param spectra Vector of sets (as returned by calculateDistributionSpectra)
 * @return Vector of widths (0 for empty spectra)
 */
inline std::vector<int> calculateSpectrumWidths(std::vector<std::set<int>>& spectra) {
    std::vector<int> widths;
    
    for (auto& spectrum : spectra) {
        if (spectrum.empty()) {
            widths.push_back(0);
        } else {
            int minVal = *spectrum.begin();
            int maxVal = *spectrum.rbegin();
            int width = maxVal - minVal;
            widths.push_back(width);
        }
    }
    
    return widths;
}

/**
 * @brief Compute a simple average spectrum variation
 *
 * Returns the mean of the spectrum widths normalized by the number of tones.
 *
 * @param widths Vector of spectrum widths
 * @param numberOfTones Number of tones in the scale
 * @return Average spectrum variation
 */
inline double calculateSpectrumVariation(std::vector<int>& widths, int numberOfTones) {
    if (numberOfTones <= 0) {
        return 0.0;
    }
    int sumOfWidths = 0;
    for (int width : widths) {
        sumOfWidths += width;
    }
    return static_cast<double>(sumOfWidths) / numberOfTones;
}

/**
 * @brief Find rotational symmetry axes for a scale
 *
 * Returns a list of transposition intervals that map the scale onto itself.
 *
 * @param scale Input position_vector representing the scale
 * @return Vector of integer transposition offsets that are symmetries
 */
inline std::vector<int> findRotationalSymmetryAxes(position_vector& scale) {
    std::vector<int> normalizedScale = scale.data();

    std::vector<int> axes;
    int n = normalizedScale.size();
    for (int interval = 1; interval < scale.mod(); ++interval) {
        std::vector<int> transposedScale(n);
        for (int i = 0; i < n; ++i) {
            transposedScale[i] = (normalizedScale[i] + interval) % scale.mod();
        }
        std::sort(transposedScale.begin(), transposedScale.end());
        if (transposedScale == normalizedScale) {
            axes.push_back(interval);
        }
    }
    return axes;
}

/**
 * @brief Find reflective symmetry axes for a scale (including half-integer axes)
 *
 * Axes are returned as double values (e.g., 0, 0.5, 1.0, ...). Values represent
 * axis positions in the same units as the scale (modulus space).
 *
 * @param scale Input position_vector representing the scale
 * @return Vector of axes where the scale is symmetric under reflection
 */
inline std::vector<double> findReflectiveSymmetryAxes(position_vector& scale) {
    std::vector<int> normalizedScale = scale.data();
    std::vector<double> axes;
    int n = normalizedScale.size();
    
    // Check all possible axes (both integer and half-integer)
    // We check axis values at 0.5 increments: 0, 0.5, 1, 1.5, ..., mod-0.5
    for (int axis_doubled = 0; axis_doubled < 2 * scale.mod(); ++axis_doubled) {
        double axis = axis_doubled / 2.0;
        bool isSymmetric = true;
        
        for (int i = 0; i < n; ++i) {
            // Reflect note across axis: reflected = 2*axis - note
            double reflected = 2 * axis - normalizedScale[i];
            
            // Normalize to [0, mod) range (round to reduce float drift at boundaries)
            const double mod_d = static_cast<double>(scale.mod());
            double wrapped = std::fmod(reflected, mod_d);
            if (wrapped < 0.0) {
                wrapped += mod_d;
            }
            int reflectedNote = static_cast<int>(std::lround(wrapped));
            reflectedNote = ((reflectedNote % scale.mod()) + scale.mod()) % scale.mod();
            
            if (std::find(normalizedScale.begin(), normalizedScale.end(), reflectedNote) ==
                normalizedScale.end()) {
                isSymmetric = false;
                break;
            }
        }
        
        if (isSymmetric) {
            axes.push_back(axis);
        }
    }
    
    return axes;
}

/**
 * @brief Simple primality test
 *
 * @param num Integer to test
 * @return true if num is prime, false otherwise
 */
inline bool isPrime(int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    for (int i = 3; i <= static_cast<int>(std::sqrt(static_cast<double>(num))); i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

/**
 * @brief Classify an integer modulus into aksak rhythm categories
 *
 * - authentic aksak: modulus is prime
 * - quasi-aksak: odd but composite
 * - pseudo-aksak: even
 *
 * @param mod Modulus (number of time units)
 */
inline void classifyAksakRhythm(int mod) {
    if (isPrime(mod)) {
        std::cout << "The rhythm is authentic aksak" << std::endl;
    } else if (mod % 2 != 0) {
        std::cout << "The rhythm is quasi-aksak" << std::endl;
    } else {
        std::cout << "The rhythm is pseudo-aksak" << std::endl;
    }
}

/**
 * @brief Check whether a scale is palindromic (has axis at 0)
 *
 * @param scale Input position_vector
 * @return true if reflective symmetry axis includes 0
 */
inline bool isPalindrome(position_vector& scale) {
    std::vector<double> reflectiveAxes = findReflectiveSymmetryAxes(scale);
    return std::find(reflectiveAxes.begin(), reflectiveAxes.end(), 0.0) != reflectiveAxes.end();
}

/**
 * @brief Test chirality of a scale (whether it is superposable with its mirror)
 *
 * @param scale Input position_vector
 * @return true if the scale is chiral (not superposable with its mirror)
 */
inline bool isChiral(position_vector& scale) {
    std::vector<int> normalizedScale = scale.data();
    std::vector<int> sortedNormalized = normalizedScale;
    std::sort(sortedNormalized.begin(), sortedNormalized.end());

    std::vector<int> mirroredScale = normalizedScale;
    for (int& note : mirroredScale) {
        note = (scale.mod() - note) % scale.mod();
    }

    std::sort(mirroredScale.begin(), mirroredScale.end());

    if (sortedNormalized == mirroredScale) {
        return false;
    }

    int n = normalizedScale.size();
    for (int interval = 1; interval < scale.mod(); ++interval) {
        std::vector<int> transposedMirroredScale(n);
        for (int i = 0; i < n; ++i) {
            transposedMirroredScale[i] = (mirroredScale[i] + interval) % scale.mod();
        }
        std::sort(transposedMirroredScale.begin(), transposedMirroredScale.end());
        if (transposedMirroredScale == sortedNormalized) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Test whether a scale is balanced (center of mass at origin)
 *
 * Projects pitches onto the unit circle and tests whether the vector sum is
 * (approximately) zero.
 *
 * @param scale Input position_vector
 * @return true if the scale is balanced
 */
inline bool isBalanced(position_vector& scale) {
    double x_sum = 0.0;
    double y_sum = 0.0;
    double angle_step = 2 * 3.141592653589793 / scale.mod();

    for (int note : scale.data()) {
        double angle = note * angle_step;
        x_sum += std::cos(angle);
        y_sum += std::sin(angle);
    }

    return std::abs(x_sum) < 1e-6 && std::abs(y_sum) < 1e-6;
}

/**
 * @brief Generate a cyclic sequence using multiplication modulo n
 *
 * Produces k values of (i * m) mod n. Optionally prints each step.
 *
 * @param m Multiplier
 * @param k Number of values to generate
 * @param n Modulus
 * @param printSteps If true prints each generated step
 * @return Generated sequence of length k
 */
inline std::vector<int> generate(int m, int k, int n, bool printSteps = false) {
    std::vector<int> sequence;
    for (int i = 0; i < k; ++i) {
        int value = (i * m) % n;
        if (printSteps) {
            std::cout << i << " x " << m << " mod " << n << " = " << value << std::endl;
        }
        sequence.push_back(value);
    }
    return sequence;
}

/**
 * @brief Test whether a vector is generated by a single multiplier modulo n
 *
 * Scans multipliers m in 1..n-1 and checks whether the sorted generated
 * sequence equals the input. Returns the multiplier if found.
 *
 * @param in Input (candidate) vector
 * @param n Modulus
 * @return pair(found, multiplier)
 */
inline std::pair<bool, int> isGenerated(std::vector<int>& in, int n) {
    int k = in.size();
    std::vector<int> sortedIn = in;
    std::sort(sortedIn.begin(), sortedIn.end());

    for (int m = 1; m < n; ++m) {
        std::vector<int> generatedSeq = generate(m, k, n);
        std::sort(generatedSeq.begin(), generatedSeq.end());

        if (generatedSeq == sortedIn) {
            return std::make_pair(true, m);
        }
    }
    return std::make_pair(false, -1);
}

/**
 * @brief Print generator information if the vector is generated by a multiplier
 *
 * @param in Input vector
 * @param mod Modulus
 */
inline void printGenerators(std::vector<int>& in, int mod) {
    std::pair<bool, int> result = isGenerated(in, mod);
    if (result.first) {
        std::cout << "The vector is generated by multiples of m = " << result.second << " mod " << mod << std::endl;
        std::cout << "Generators:" << std::endl;
        generate(result.second, in.size(), mod, true);
    } 
    else {
        std::cout << "The vector is not generated by multiples of any integer m under modulo " << mod << std::endl;
    }    
}

/**
 * @brief Pretty-print distribution spectra
 *
 * @param spectra Vector of sets representing distribution spectra
 */
inline void printDistributionSpectra(std::vector<std::set<int>>& spectra) {
    for (size_t i = 0; i < spectra.size(); ++i) {
        std::cout << "<" << i + 1 << "> = {";
        for (auto it = spectra[i].begin(); it != spectra[i].end(); ++it) {
            if (it != spectra[i].begin()) {
                std::cout << ",";
            }
            std::cout << *it;
        }
        std::cout << "}\n";
    }
}

/**
 * @brief Pretty-print spectrum widths
 *
 * @param widths Vector of spectrum widths as returned by calculateSpectrumWidths
 */
inline void printSpectrumWidths(std::vector<int>& widths) {
    for (size_t i = 0; i < widths.size(); ++i) {
        std::cout << "Width of <" << i + 1 << "> = " << widths[i] << "\n";
    }
}

/**
 * @brief Print integer-valued symmetry axes with a label
 *
 * @param axes Integer-valued axes
 * @param symmetryType Label for the symmetry type (e.g., "Rotational")
 */
inline void printSymmetryAxes(std::vector<int>& axes, const std::string& symmetryType) {
    std::cout << symmetryType << " symmetry axes: ";
    for (size_t i = 0; i < axes.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << axes[i];
    }
    std::cout << "\n";
}

/**
 * @brief Print floating-point symmetry axes with a label
 *
 * @param axes Floating-point axes (half-integer possible)
 * @param symmetryType Label for the symmetry type (e.g., "Reflective")
 */
inline void printSymmetryAxes(std::vector<double>& axes, const std::string& symmetryType) {
    std::cout << symmetryType << " symmetry axes: ";
    for (size_t i = 0; i < axes.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << axes[i];
    }
    std::cout << "\n";
}

/**
 * @brief Run a comprehensive textual analysis for a position_vector
 *
 * Prints positions, intervals, onsets, distances, occurrences, deepness tests,
 * generators, aksak classification, evenness, entropy, longest subsequence,
 * spectrum information, symmetry axes and basic complexity measures.
 *
 * @param p Input position_vector to analyze
 */
inline void printAnalysis(position_vector p) {
    std::vector<int> in = p.data();
    int mod = p.mod();
    interval_vector j = positions_to_intervals(p);
    std::vector<int> intervals = j.data();
    onset_vector onsets = positions_to_onset(p);
    std::vector<int> distances = geodesicDistances(p);
    std::map<int, int> occurrences = calculateOccurrences(distances);
    std::vector<std::set<int>> spectra = calculateDistributionSpectra(p);
    std::vector<int> widths = calculateSpectrumWidths(spectra);
    double variation = calculateSpectrumVariation(widths, p.size());

    std::cout << "Analysis Results:" << std::endl;
    std::cout << std::endl;

    std::cout << "Modulo:" << std::endl;
    std::cout << mod << std::endl;
    std::cout << std::endl;


    std::cout << "Positions:" << std::endl;
    for (int i : in) {
        std::cout << i << " ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "Intervals:" << std::endl;
    for (int i : intervals) {
        std::cout << i << " ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "Onsets:" << std::endl;
    for (int i : onsets.data()) {
        std::cout << i << " ";
    }
    std::cout << std::endl << std::endl;
    
    std::cout << "Distances:" << std::endl;
    printDistances(p, distances);
    std::cout << std::endl;

    std::cout << "Occurrences:" << std::endl;
    printOccurrences(occurrences);
    std::cout << std::endl;

    std::cout << "Deepness:" << std::endl;
    printDeepness(occurrences, in.size());
    std::cout << std::endl;

    std::cout << "Generators check:" << std::endl;
    printGenerators(in, mod);
    std::cout << std::endl;

    std::cout << "Aksak Check:" << std::endl;
    classifyAksakRhythm(mod);
    std::cout << std::endl;

    std::cout << "Regression Evenness:" << std::endl;
    double evenness = calculateRegressionEvenness(in, mod);
    std::cout << evenness << std::endl;
    std::cout << std::endl;

    std::cout << "Rhythmic Oddity:" << std::endl;
    int oddity = calculateRhythmicOddity(p);
    std::cout << oddity << std::endl;
    std::cout << std::endl;

    std::cout << "Shannon Entropy:" << std::endl;
    double entropy = computeEntropy(p);
    std::cout << entropy << std::endl;
    std::cout << std::endl;

    std::cout << "Longest Subsequence:" << std::endl;
    int subsequence = computeLongestSubsequence(p);
    std::cout << subsequence << std::endl;
    std::cout << std::endl;

    std::cout << "Euclidean String Check:" << std::endl;
    bool euclidean = isEuclidean(p, mod);
    std::cout << "The interval vector is " << (euclidean ? "" : "not ") << "an Euclidean string" << std::endl;
    std::cout << std::endl;

    std::cout << "Distribution Spectra:" << std::endl;
    printDistributionSpectra(spectra);
    std::cout << std::endl;

    std::cout << "Spectrum Widths:" << std::endl;
    printSpectrumWidths(widths);
    std::cout << std::endl;

    std::cout << "Spectrum Variation:" << std::endl;
    std::cout << variation << "\n";
    std::cout << std::endl;

    std::cout << "Symmetry Analysis:" << std::endl;
    std::vector<int> rotationalSymmetryAxes = findRotationalSymmetryAxes(p);
    std::vector<double> reflectiveSymmetryAxes = findReflectiveSymmetryAxes(p);
    printSymmetryAxes(rotationalSymmetryAxes, "Rotational");
    printSymmetryAxes(reflectiveSymmetryAxes, "Reflective");
    std::cout << std::endl;

    std::cout << "Transition complexity:" << std::endl;
    int complexity = computeTransitionComplexity(p, mod);
    std::cout << complexity << std::endl;
    std::cout << std::endl;

    std::cout << "Palindrome Check:" << std::endl;
    bool palindrome = isPalindrome(p);
    std::cout << "The vector is " << (palindrome ? "" : "not ") << "palindrome" << std::endl;
    std::cout << std::endl;

    std::cout << "Chirality Check:" << std::endl;
    bool chiral = isChiral(p);
    std::cout << "The vector is " << (chiral ? "" : "not ") << "chiral" << std::endl;
    std::cout << std::endl;

    std::cout << "Balance Check:" << std::endl;
    bool balanced = isBalanced(p);
    std::cout << "The vector is " << (balanced ? "" : "not ") << "balanced" << std::endl;
    std::cout << std::endl;
}

} // namespace musicpp

#endif