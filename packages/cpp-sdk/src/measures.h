#ifndef MEASURES_H
#define MEASURES_H

#include "./vectors.h"
#include "./Vector.h"

/**
 * @file measures.h
 * @brief Collection of analysis and measure utilities for PositionVector and related types
 *
 * This header provides a variety of music-theoretic and rhythmic measures such as
 * geodesic distances, distribution spectra, symmetry checks, entropy and generators, useful for analysis. Functions are intended to be
 * small, self-contained utilities that operate on `PositionVector`, `IntervalVector`,
 * `BinaryVector` and plain integer vectors.
 */

/**
 * @brief Compute successive differences of an integer vector
 *
 * @param in Input integer vector
 * @return Vector of differences where out[i] = in[i+1] - in[i]
 * @note Returns an empty vector if input has less than two elements
 */
vector<int> differences(vector<int>& in) {
    vector<int> out;

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
int geodesicDistance(int a, int b, int mod) {
    int distance = (b - a + mod) % mod;
    if (distance > mod / 2) 
        distance = mod - distance;
    return distance;
}

/**
 * @brief Compute all pairwise geodesic distances for a PositionVector
 *
 * @param in Input PositionVector
 * @return Flattened vector of pairwise geodesic distances (i<j order)
 */
vector<int> geodesicDistances(PositionVector& in) {
    vector<int> distances;
    for (size_t i = 0; i < in.size(); ++i) {
        for (size_t j = i + 1; j < in.size(); ++j) {
            int distance = geodesicDistance(in[i], in[j], in.mod);
            distances.push_back(distance);
        }
    }
    return distances;
}

/**
 * @brief Test whether an interval sequence is an Euclidean rhythm
 *
 * The function checks whether the provided `PositionVector` (converted to
 * intervals) corresponds to an Euclidean rhythm by applying a simple rotation
 * and equality test.
 *
 * @param in PositionVector to test (positions will be converted to intervals)
 * @param mod Modulus used for normalization (not always required)
 * @return true if the interval vector is Euclidean, false otherwise
 */
bool isEuclidean(PositionVector in, int mod) {
    
    IntervalVector j = positionsToIntervals(in);
    vector<int> temp = j.data;
    vector<int> temp2 = temp;
    int n = temp.size();
    if (n == 0) return false;
    
    temp[0]++;
    temp[n-1]--;

    for (int i = 0; i < n; ++i) {
        if (temp == temp2) return true;
        rotate(temp.begin(), temp.begin() + 1, temp.end());
    }

    return false;
}

/**
 * @brief Count occurrences of integer values in a vector
 *
 * @param in Input integer vector
 * @return map where key = value from `in` and value = frequency
 */
map<int, int> calculateOccurrences(vector<int>& in) {
    map<int, int> occurrences;
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
bool isWinogradDeep(map<int, int>& in, int size) {
    set<int> seen;
    for (int i = 1; i < size; ++i) {
        if (in.find(i) == in.end()) {
            return false;
        }
        if (!seen.insert(in.at(i)).second) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Test Erdos-deep property for an occurrence map
 *
 * Erdos-deep requires that all frequencies (values of the map) are unique.
 *
 * @param in Map of occurrences (distance -> frequency)
 * @return true if Erdos-deep, false otherwise
 */
bool isErdosDeep(map<int, int>& in) {
    set<int> seen;
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
 * @return Sum of absolute deviations from ideal equally spaced positions
 */
double calculateRegressionEvenness(vector<int>& rhythm, int totalTimeUnits) {
    int numNotes = rhythm.size();
    double idealInterval = static_cast<double>(totalTimeUnits) / numNotes;

    vector<double> idealPositions(numNotes);
    for (int i = 0; i < numNotes; ++i) {
        idealPositions[i] = i * idealInterval;
    }

    vector<double> deviations(numNotes);
    for (int i = 0; i < numNotes; ++i) {
        deviations[i] = abs(rhythm[i] - idealPositions[i]);
    }

    double regressionEvenness = accumulate(deviations.begin(), deviations.end(), 0.0);
    return regressionEvenness;
}

/**
 * @brief Calculate rhythmic oddity: number of antipodal pairs
 *
 * Counts pairs of onsets whose distances around the cycle are exactly equal
 * (i.e., they are opposite points on the circle). This is a simple measure
 * related to symmetry and antipodal structure.
 *
 * @param in PositionVector of onsets
 * @return Integer count of antipodal pairs
 */
int calculateRhythmicOddity(PositionVector& in) {
    int k = in.size();
    int rhythmic_oddity = 0;

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            int dist1 = (in[j] - in[i] + in.mod) % in.mod;
            int dist2 = in.mod - dist1;
            
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
 * Converts the position vector into a binary onset pattern and counts the
 * number of transitions between 0 and 1 (i.e., on/offs). Useful as a
 * simple rhythmic complexity metric.
 *
 * @param in PositionVector of onsets
 * @param mod Modulus (used internally by conversion routines)
 * @return Number of transitions in the binary onset pattern
 */
int computeTransitionComplexity(PositionVector& in, int mod) {
    BinaryVector binaryVector = positionsToBinary(in);
    if (binaryVector.size() == 0) {
        return 0;
    }

    int complexity = 0;
    for (size_t i = 1; i < binaryVector.size(); ++i) {
        if (binaryVector[i] != binaryVector[i - 1]) {
            ++complexity;
        }
    }

    return complexity;
}

/**
 * @brief Estimate Shannon entropy of the onset pattern
 *
 * Converts positions to a binary onset vector and computes a simple Shannon
 * entropy over the distribution of events. For sparse patterns this is a
 * coarse measure.
 *
 * @param in PositionVector of onsets
 * @return Entropy in bits (base-2). Returns 0.0 for empty inputs.
 */
double computeEntropy(PositionVector& in) {
    BinaryVector binaryVector = positionsToBinary(in);
    if (binaryVector.size() == 0) {
        return 0.0;
    }

    unordered_map<int, int> frequency;
    for (int i = 0; i < binaryVector.size(); ++i) {
        ++frequency[binaryVector[i]];
    }

    double entropy = 0.0;
    for (auto& pair : frequency) {
        double probability = static_cast<double>(pair.second) / binaryVector.size();
        entropy -= probability * std::log2(probability);
    }

    return entropy;
}

/**
 * @brief Compute the length of the longest run of identical binary values
 *
 * After converting positions to a binary onset vector, returns the maximum
 * length of a consecutive run of identical values (useful for measuring
 * clustering or gaps).
 *
 * @param in PositionVector of onsets
 * @return Length of the longest subsequence
 */
int computeLongestSubsequence(PositionVector& in) {
    BinaryVector binaryVector = positionsToBinary(in);
    if (binaryVector.size() == 0) {
        return 0;
    }

    int longest = 1;
    int currentRun = 1;

    for (size_t i = 1; i < binaryVector.size(); ++i) {
        if (binaryVector[i] == binaryVector[i - 1]) {
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
 * @param in PositionVector of positions
 * @param distances Flattened vector of distances in i<j order (same order as produced by geodesicDistances)
 */
void printDistances(PositionVector& in, vector<int>& distances) {
    size_t index = 0;
    for (size_t i = 0; i < in.size(); ++i) {
        for (size_t j = i + 1; j < in.size(); ++j) {
            cout << "Distance between " << in[i] << " and " << in[j] << " is: " << distances[index++] << endl;
        }
    }
}

/**
 * @brief Pretty-print occurrence counts
 *
 * @param occurrences Map from value to frequency
 */
void printOccurrences(map<int, int>& occurrences) {
    for (auto& pair : occurrences) {
        cout << "Distance " << pair.first << " appears " << pair.second << " times" << endl;
    }
}

/**
 * @brief Print deepness classification (Winograd / Erdos)
 *
 * @param occurrences Map from distance to frequency
 * @param size Number of tones in the original set
 */
void printDeepness(map<int, int>& occurrences, int size) {
    bool winogradDeep = isWinogradDeep(occurrences, size);
    bool erdosDeep = isErdosDeep(occurrences);

    cout << "The vector is " << (winogradDeep ? "" : "not ") << "Winograd-deep" << endl;
    cout << "The vector is " << (erdosDeep ? "" : "not ") << "Erdos-deep" << endl;
}

/**
 * @brief Calculate distribution spectra for a scale
 *
 * For each generic interval (1..n-1) the function collects the set of
 * specific intervals that occur at that generic distance across the scale.
 *
 * @param in Input PositionVector (scale)
 * @return Vector of sets where element k-1 contains the specific intervals for generic interval k
 */
vector<set<int>> calculateDistributionSpectra(PositionVector& in) {
    vector<int> normalizedScale = in.data;
    
    vector<set<int>> distributionSpectra(normalizedScale.size() - 1);

    for (size_t i = 0; i < normalizedScale.size(); ++i) {
        for (size_t j = 1; j < normalizedScale.size(); ++j) {
            int genericInterval = j;
            int specificInterval = (normalizedScale[(i + j) % normalizedScale.size()] - normalizedScale[i] + in.mod) % in.mod;
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
vector<int> calculateSpectrumWidths(vector<set<int>>& spectra) {
    vector<int> widths;
    
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
double calculateSpectrumVariation(vector<int>& widths, int numberOfTones) {
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
 * @param scale Input PositionVector representing the scale
 * @return Vector of integer transposition offsets that are symmetries
 */
vector<int> findRotationalSymmetryAxes(PositionVector& scale) {
    vector<int> normalizedScale = scale.data;

    vector<int> axes;
    int n = normalizedScale.size();
    for (int interval = 1; interval < scale.mod; ++interval) {
        vector<int> transposedScale(n);
        for (int i = 0; i < n; ++i) {
            transposedScale[i] = (normalizedScale[i] + interval) % scale.mod;
        }
        sort(transposedScale.begin(), transposedScale.end());
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
 * @param scale Input PositionVector representing the scale
 * @return Vector of axes where the scale is symmetric under reflection
 */
vector<double> findReflectiveSymmetryAxes(PositionVector& scale) {
    vector<int> normalizedScale = scale.data;
    vector<double> axes;
    int n = normalizedScale.size();
    
    // Check all possible axes (both integer and half-integer)
    // We check axis values at 0.5 increments: 0, 0.5, 1, 1.5, ..., mod-0.5
    for (int axis_doubled = 0; axis_doubled < 2 * scale.mod; ++axis_doubled) {
        double axis = axis_doubled / 2.0;
        bool isSymmetric = true;
        
        for (int i = 0; i < n; ++i) {
            // Reflect note across axis: reflected = 2*axis - note
            double reflected = 2 * axis - normalizedScale[i];
            
            // Normalize to [0, mod) range
            int reflectedNote = (int)(fmod(reflected + 10 * scale.mod, scale.mod));
            
            if (find(normalizedScale.begin(), normalizedScale.end(), reflectedNote) == normalizedScale.end()) {
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
bool isPrime(int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    for (int i = 3; i <= sqrt(num); i += 2) {
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
void classifyAksakRhythm(int mod) {
    if (isPrime(mod)) {
        cout << "The rhythm is authentic aksak" << endl;
    } else if (mod % 2 != 0) {
        cout << "The rhythm is quasi-aksak" << endl;
    } else {
        cout << "The rhythm is pseudo-aksak" << endl;
    }
}

/**
 * @brief Check whether a scale is palindromic (has axis at 0)
 *
 * @param scale Input PositionVector
 * @return true if reflective symmetry axis includes 0
 */
bool isPalindrome(PositionVector& scale) {
    vector<double> reflectiveAxes = findReflectiveSymmetryAxes(scale);
    return find(reflectiveAxes.begin(), reflectiveAxes.end(), 0) != reflectiveAxes.end();
}

/**
 * @brief Test chirality of a scale (whether it is superposable with its mirror)
 *
 * @param scale Input PositionVector
 * @return true if the scale is chiral (not superposable with its mirror)
 */
bool isChiral(PositionVector& scale) {
    vector<int> normalizedScale = scale.data;

    vector<int> mirroredScale = normalizedScale;
    for (int& note : mirroredScale) {
        note = (scale.mod - note) % scale.mod;
    }

    sort(mirroredScale.begin(), mirroredScale.end());

    if (normalizedScale == mirroredScale) {
        return false;
    }

    int n = normalizedScale.size();
    for (int interval = 1; interval < scale.mod; ++interval) {
        vector<int> transposedMirroredScale(n);
        for (int i = 0; i < n; ++i) {
            transposedMirroredScale[i] = (mirroredScale[i] + interval) % scale.mod;
        }
        sort(transposedMirroredScale.begin(), transposedMirroredScale.end());
        if (transposedMirroredScale == normalizedScale) {
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
 * @param scale Input PositionVector
 * @return true if the scale is balanced
 */
bool isBalanced(PositionVector& scale) {
    double x_sum = 0.0;
    double y_sum = 0.0;
    double angle_step = 2 * 3.141592653589793 / scale.mod;

    for (int note : scale.data) {
        double angle = note * angle_step;
        x_sum += cos(angle);
        y_sum += sin(angle);
    }

    return abs(x_sum) < 1e-6 && abs(y_sum) < 1e-6;
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
vector<int> generate(int m, int k, int n, bool printSteps = false) {
    vector<int> sequence;
    for (int i = 0; i < k; ++i) {
        int value = (i * m) % n;
        if (printSteps) {
            cout << i << " x " << m << " mod " << n << " = " << value << endl;
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
pair<bool, int> isGenerated(vector<int>& in, int n) {
    int k = in.size();
    
    for (int m = 1; m < n; ++m) {
        vector<int> generatedSeq = generate(m, k, n);
        sort(generatedSeq.begin(), generatedSeq.end());
        
        if (generatedSeq == in) {
            return make_pair(true, m);
        }
    }
    return make_pair(false, -1);
}

/**
 * @brief Print generator information if the vector is generated by a multiplier
 *
 * @param in Input vector
 * @param mod Modulus
 */
void printGenerators(vector<int>& in, int mod) {
    pair<bool, int> result = isGenerated(in, mod);
    if (result.first) {
        cout << "The vector is generated by multiples of m = " << result.second << " mod " << mod << endl;
        cout << "Generators:" << endl;
        generate(result.second, in.size(), mod, true);
    } 
    else {
        cout << "The vector is not generated by multiples of any integer m under modulo " << mod << endl;
    }    
}

/**
 * @brief Pretty-print distribution spectra
 *
 * @param spectra Vector of sets representing distribution spectra
 */
void printDistributionSpectra(vector<set<int>>& spectra) {
    for (size_t i = 0; i < spectra.size(); ++i) {
        cout << "<" << i + 1 << "> = {";
        for (auto it = spectra[i].begin(); it != spectra[i].end(); ++it) {
            if (it != spectra[i].begin()) {
                cout << ",";
            }
            cout << *it;
        }
        cout << "}\n";
    }
}

/**
 * @brief Pretty-print spectrum widths
 *
 * @param widths Vector of spectrum widths as returned by calculateSpectrumWidths
 */
void printSpectrumWidths(vector<int>& widths) {
    for (size_t i = 0; i < widths.size(); ++i) {
        cout << "Width of <" << i + 1 << "> = " << widths[i] << "\n";
    }
}

/**
 * @brief Print integer-valued symmetry axes with a label
 *
 * @param axes Integer-valued axes
 * @param symmetryType Label for the symmetry type (e.g., "Rotational")
 */
void printSymmetryAxes(vector<int>& axes, const string& symmetryType) {
    cout << symmetryType << " symmetry axes: ";
    for (size_t i = 0; i < axes.size(); ++i) {
        if (i > 0) {
            cout << ", ";
        }
        cout << axes[i];
    }
    cout << "\n";
}

/**
 * @brief Print floating-point symmetry axes with a label
 *
 * @param axes Floating-point axes (half-integer possible)
 * @param symmetryType Label for the symmetry type (e.g., "Reflective")
 */
void printSymmetryAxes(vector<double>& axes, const string& symmetryType) {
    cout << symmetryType << " symmetry axes: ";
    for (size_t i = 0; i < axes.size(); ++i) {
        if (i > 0) {
            cout << ", ";
        }
        cout << axes[i];
    }
    cout << "\n";
}

/**
 * @brief Run a comprehensive textual analysis for a PositionVector
 *
 * Prints positions, intervals, onsets, distances, occurrences, deepness tests,
 * generators, aksak classification, evenness, entropy, longest subsequence,
 * spectrum information, symmetry axes and basic complexity measures.
 *
 * @param p Input PositionVector to analyze
 */
void printAnalysis(PositionVector p) {
    vector<int> in = p.data;
    int mod = p.range;
    IntervalVector j = positionsToIntervals(p);
    vector<int> intervals = j.data;
    BinaryVector onsets = positionsToBinary(p);
    vector<int> distances = geodesicDistances(p);
    map<int, int> occurrences = calculateOccurrences(distances);
    vector<set<int>> spectra = calculateDistributionSpectra(p);
    vector<int> widths = calculateSpectrumWidths(spectra);
    double variation = calculateSpectrumVariation(widths, p.size());

    cout << "Analysis Results:" << endl;
    cout << endl;

    cout << "Modulo:" << endl;
    cout << mod << endl;
    cout << endl;


    cout << "Positions:" << endl;
    for (int i : in) {
        cout << i << " ";
    }
    cout << endl << endl;

    cout << "Intervals:" << endl;
    for (int i : intervals) {
        cout << i << " ";
    }
    cout << endl << endl;

    cout << "Onsets:" << endl;
    for (int i : onsets.data) {
        cout << i << " ";
    }
    cout << endl << endl;
    
    cout << "Distances:" << endl;
    printDistances(p, distances);
    cout << endl;

    cout << "Occurrences:" << endl;
    printOccurrences(occurrences);
    cout << endl;

    cout << "Deepness:" << endl;
    printDeepness(occurrences, in.size());
    cout << endl;

    cout << "Generators check:" << endl;
    printGenerators(in, mod);
    cout << endl;

    cout << "Aksak Check:" << endl;
    classifyAksakRhythm(mod);
    cout << endl;

    cout << "Regression Evenness:" << endl;
    double evenness = calculateRegressionEvenness(in, mod);
    cout << evenness << endl;
    cout << endl;

    cout << "Rhythmic Oddity:" << endl;
    int oddity = calculateRhythmicOddity(p);
    cout << oddity << endl;
    cout << endl;

    cout << "Shannon Entropy:" << endl;
    double entropy = computeEntropy(p);
    cout << entropy << endl;
    cout << endl;

    cout << "Longest Subsequence:" << endl;
    int subsequence = computeLongestSubsequence(p);
    cout << subsequence << endl;
    cout << endl;

    cout << "Euclidean String Check:" << endl;
    bool euclidean = isEuclidean(p, mod);
    cout << "The interval vector is " << (euclidean ? "" : "not ") << "an Euclidean string" << endl;
    cout << endl;

    cout << "Distribution Spectra:" << endl;
    printDistributionSpectra(spectra);
    cout << endl;

    cout << "Spectrum Widths:" << endl;
    printSpectrumWidths(widths);
    cout << endl;

    cout << "Spectrum Variation:" << endl;
    cout << variation << "\n";
    cout << endl;

    cout << "Symmetry Analysis:" << endl;
    vector<int> rotationalSymmetryAxes = findRotationalSymmetryAxes(p);
    vector<double> reflectiveSymmetryAxes = findReflectiveSymmetryAxes(p);
    printSymmetryAxes(rotationalSymmetryAxes, "Rotational");
    printSymmetryAxes(reflectiveSymmetryAxes, "Reflective");
    cout << endl;

    cout << "Transition complexity:" << endl;
    int complexity = computeTransitionComplexity(p, mod);
    cout << complexity << endl;
    cout << endl;

    cout << "Palindrome Check:" << endl;
    bool palindrome = isPalindrome(p);
    cout << "The vector is " << (palindrome ? "" : "not ") << "palindrome" << endl;
    cout << endl;

    cout << "Chirality Check:" << endl;
    bool chiral = isChiral(p);
    cout << "The vector is " << (chiral ? "" : "not ") << "chiral" << endl;
    cout << endl;

    cout << "Balance Check:" << endl;
    bool balanced = isBalanced(p);
    cout << "The vector is " << (balanced ? "" : "not ") << "balanced" << endl;
    cout << endl;
}

#endif
