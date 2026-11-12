#ifndef MUSICPP_SCALE_DICTIONARY_H
#define MUSICPP_SCALE_DICTIONARY_H

/**
 * @file scale_dictionary.h
 * @brief Scale dictionary and lookup functionality
 * Provides a database of musical scales in 12TET (based on the work of Francesco Balena - The Scale Omnibus) and methods to find matching scales
 * based on input pitch class sets.
 */

#include "utility.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>


namespace musicpp {

class ScaleDatabase {
private:
    struct ScaleInfo {
        std::string sheetName;
        std::string scaleName;
        std::vector<int> intervals;
        
        bool operator==(const ScaleInfo& other) const {
            return intervals == other.intervals;
        }
    };
    
    std::vector<ScaleInfo> scales;
    
public:
    ScaleDatabase() {
        initializeAllScales();
    }
    
    std::vector<ScaleInfo> findScale(const std::vector<int>& inputIntervals) {
        std::vector<ScaleInfo> results;
        
        if (inputIntervals.empty()) return results;
        
        std::vector<int> normalizedInput;
        int root = inputIntervals[0];
        for (int interval : inputIntervals) {
            normalizedInput.push_back(interval - root);
        }
        
        std::vector<int> processedInput = normalizedInput;
        std::sort(processedInput.begin(), processedInput.end());
        processedInput.erase(std::unique(processedInput.begin(), processedInput.end()), processedInput.end());
        
        for (const auto& scale : scales) {
            std::vector<int> sortedScale = scale.intervals;
            std::sort(sortedScale.begin(), sortedScale.end());
            
            if (processedInput == sortedScale) {
                results.push_back(scale);
            }
        }
        
        return results;
    }
    
    void displayResults(const std::vector<int>& inputIntervals, const std::string& rootNote = "C") {
        std::vector<ScaleInfo> foundScales = findScale(inputIntervals);
        
        std::cout << "\nInput notes: ";
        for (size_t i = 0; i < inputIntervals.size(); ++i) {
            std::cout << inputIntervals[i];
            if (i < inputIntervals.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
        
        if (foundScales.empty()) {
            std::cout << "No matching scale found in database." << std::endl;
            return;
        }
        
        std::cout << "Found " << foundScales.size() << " matching scale(s):" << std::endl;
        std::cout << "=================================" << std::endl;
        
        for (const auto& scale : foundScales) {
            std::cout << "Category: " << scale.sheetName << std::endl;
            std::cout << "Scale: " << rootNote << " " << scale.scaleName << std::endl;
            
            // Display intervals
            std::cout << "Pitch Classes: ";
            for (size_t i = 0; i < scale.intervals.size(); ++i) {
                std::cout << scale.intervals[i];
                if (i < scale.intervals.size() - 1) std::cout << " ";
            }
            std::cout << std::endl << std::endl;
        }
    }
    
    // Get all unique interval sets (for debugging)
    std::set<std::vector<int>> getAllIntervalSets() {
        std::set<std::vector<int>> uniqueSets;
        for (const auto& scale : scales) {
            std::vector<int> sorted = scale.intervals;
            std::sort(sorted.begin(), sorted.end());
            uniqueSets.insert(sorted);
        }
        return uniqueSets;
    }
    
private:
    void initializeAllScales() {
        // Sheet 1: Major and minor scales
        addScale("Major and minor scales", "Ionian (Major)", {0, 2, 4, 5, 7, 9, 11});
        addScale("Major and minor scales", "Dorian", {0, 2, 3, 5, 7, 9, 10});
        addScale("Major and minor scales", "Phrygian", {0, 1, 3, 5, 7, 8, 10});
        addScale("Major and minor scales", "Lydian", {0, 2, 4, 6, 7, 9, 11});
        addScale("Major and minor scales", "Mixolydian", {0, 2, 4, 5, 7, 9, 10});
        addScale("Major and minor scales", "Aeolian (Natural Minor)", {0, 2, 3, 5, 7, 8, 10});
        addScale("Major and minor scales", "Locrian", {0, 1, 3, 5, 6, 8, 10});
        addScale("Major and minor scales", "Melodic Minor", {0, 2, 3, 5, 7, 9, 11});
        addScale("Major and minor scales", "Dorian b2", {0, 1, 3, 5, 7, 9, 10});
        addScale("Major and minor scales", "Lydian Augmented", {0, 2, 4, 6, 8, 9, 11});
        addScale("Major and minor scales", "Lydian Dominant", {0, 2, 4, 6, 7, 9, 10});
        addScale("Major and minor scales", "Melodic Major", {0, 2, 4, 5, 7, 8, 10});
        addScale("Major and minor scales", "Half Diminished", {0, 2, 3, 5, 6, 8, 10});
        addScale("Major and minor scales", "Altered Dominant", {0, 1, 3, 4, 6, 8, 10});
        addScale("Major and minor scales", "Harmonic Minor", {0, 2, 3, 5, 7, 8, 11});
        addScale("Major and minor scales", "Locrian #6", {0, 1, 3, 5, 6, 9, 10});
        addScale("Major and minor scales", "Ionian Augmented", {0, 2, 4, 5, 8, 9, 11});
        addScale("Major and minor scales", "Romanian Minor", {0, 2, 3, 6, 7, 9, 10});
        addScale("Major and minor scales", "Phrygian Dominant", {0, 1, 4, 5, 7, 8, 10});
        addScale("Major and minor scales", "Lydian #2", {0, 3, 4, 6, 7, 9, 11});
        addScale("Major and minor scales", "Ultralocrian", {0, 1, 3, 4, 6, 8, 9});
        
        // Sheet 2: Symmetrical scales
        addScale("Symmetrical scales", "Whole-Tone", {0, 2, 4, 6, 8, 10});
        addScale("Symmetrical scales", "Augmented", {0, 3, 4, 7, 8, 11});
        addScale("Symmetrical scales", "Inverted Augmented", {0, 1, 4, 5, 8, 9});
        addScale("Symmetrical scales", "Diminished", {0, 2, 3, 5, 6, 8, 9, 11});
        addScale("Symmetrical scales", "Diminished Half-tone", {0, 1, 3, 4, 6, 7, 9, 10});
        addScale("Symmetrical scales", "Chromatic", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
        addScale("Symmetrical scales", "Tritone", {0, 1, 4, 6, 7, 10});
        addScale("Symmetrical scales", "Raga Neelangi", {0, 2, 3, 6, 8, 9});
        addScale("Symmetrical scales", "Messiaen 2nd Mode Truncated", {0, 1, 3, 6, 7, 9});
        addScale("Symmetrical scales", "Messiaen 3rd Mode", {0, 2, 3, 4, 6, 7, 8, 10, 11});
        addScale("Symmetrical scales", "Messiaen 4th Mode", {0, 1, 2, 5, 6, 7, 8, 11});
        addScale("Symmetrical scales", "Messiaen 4th Mode Inverse", {0, 3, 4, 5, 6, 9, 10, 11});
        addScale("Symmetrical scales", "Messiaen 5th Mode", {0, 1, 5, 6, 7, 11});
        addScale("Symmetrical scales", "Messiaen 5th Mode Inverse", {0, 4, 5, 6, 10, 11});
        addScale("Symmetrical scales", "Messiaen 6th Mode", {0, 2, 4, 5, 6, 8, 10, 11});
        addScale("Symmetrical scales", "Messiaen 6th Mode Inverse", {0, 1, 2, 4, 6, 7, 8, 10});
        addScale("Symmetrical scales", "Messiaen 7th Mode", {0, 1, 2, 3, 5, 6, 7, 8, 9, 11});
        addScale("Symmetrical scales", "Messiaen 7th Mode Inverse", {0, 2, 3, 4, 5, 6, 8, 9, 10, 11});
        addScale("Symmetrical scales", "Genus Chromaticum", {0, 1, 3, 4, 5, 7, 8, 9, 11});
        addScale("Symmetrical scales", "Two-semitone Tritone", {0, 1, 2, 6, 7, 8});
        addScale("Symmetrical scales", "Symmetrical Decatonic", {0, 1, 2, 4, 5, 6, 7, 8, 10, 11});
        addScale("Symmetrical scales", "Van Der Host", {0, 1, 3, 5, 6, 7, 9, 11});
        
        // Sheet 3: European Scales
        addScale("European Scales", "Adonai Malakh", {0, 1, 2, 3, 5, 7, 9, 10});
        addScale("European Scales", "Enigmatic (asc)", {0, 1, 4, 6, 8, 10, 11});
        addScale("European Scales", "Enigmatic (desc)", {0, 1, 4, 5, 8, 10, 11});
        addScale("European Scales", "Enigmatic Minor", {0, 1, 3, 6, 8, 10, 11});
        addScale("European Scales", "Enigmatic Mixed", {0, 1, 4, 5, 6, 8, 10, 11});
        addScale("European Scales", "Flamenco", {0, 1, 3, 4, 5, 7, 8, 10});
        addScale("European Scales", "Gypsy", {0, 2, 3, 6, 7, 8, 10});
        addScale("European Scales", "Gypsy Hexatonic", {0, 1, 4, 5, 7, 8, 9});
        addScale("European Scales", "Gypsy Inverse", {0, 1, 4, 5, 7, 9, 11});
        addScale("European Scales", "Gypsy Minor", {0, 2, 3, 6, 7, 8, 11});
        addScale("European Scales", "Hijaz Major", {0, 1, 5, 6, 8, 9, 10});
        addScale("European Scales", "Houseini", {0, 2, 3, 4, 5, 7, 8, 9, 10});
        addScale("European Scales", "Houzam", {0, 3, 4, 5, 7, 9, 11});
        addScale("European Scales", "Hungarian Major", {0, 3, 4, 6, 7, 9, 10});
        addScale("European Scales", "Hungarian Major Inverse", {0, 2, 3, 5, 6, 8, 9});
        addScale("European Scales", "Hungarian Minor b2", {0, 1, 2, 3, 6, 7, 8, 11});
        addScale("European Scales", "Istrian", {0, 1, 3, 4, 6, 7});
        addScale("European Scales", "Jeths", {0, 2, 3, 5, 6, 9, 11});
        addScale("European Scales", "Kiourdi", {0, 2, 3, 5, 6, 7, 8, 9, 10});
        addScale("European Scales", "Magen Abot", {0, 1, 3, 4, 6, 8, 9, 11});
        addScale("European Scales", "Moorish Phrygian", {0, 1, 3, 4, 5, 7, 8, 10, 11});
        addScale("European Scales", "Neapolitan Major", {0, 1, 3, 5, 7, 9, 11});
        addScale("European Scales", "Neapolitan Major b4", {0, 1, 3, 4, 7, 9, 11});
        addScale("European Scales", "Neapolitan Major b5", {0, 1, 3, 5, 6, 9, 11});
        addScale("European Scales", "Neapolitan Minor", {0, 1, 3, 5, 7, 8, 11});
        addScale("European Scales", "Harmonic Neapolitan Minor", {0, 1, 2, 3, 5, 7, 8, 11});
        addScale("European Scales", "Neseveri", {0, 1, 3, 6, 7, 8, 10, 11});
        addScale("European Scales", "Prokofiev", {0, 1, 3, 5, 6, 8, 10, 11});
        addScale("European Scales", "Prometheus", {0, 2, 4, 6, 9, 10});
        addScale("European Scales", "Prometheus Neapolitan", {0, 1, 4, 6, 9, 10});
        addScale("European Scales", "Romanian Major", {0, 1, 4, 6, 7, 9, 10});
        addScale("European Scales", "Sabach", {0, 2, 3, 4, 7, 8, 10});
        addScale("European Scales", "Sabach Maj7", {0, 2, 3, 4, 7, 8, 11});
        addScale("European Scales", "Scottish Hexatonic", {0, 2, 4, 5, 7, 9});
        addScale("European Scales", "Sengiach", {0, 3, 4, 5, 7, 8, 11});
        addScale("European Scales", "Shostakovich", {0, 1, 3, 4, 6, 7, 9, 11});
        addScale("European Scales", "Spanish Heptatonic", {0, 3, 4, 5, 6, 8, 10});
        addScale("European Scales", "Spanish Octatonic", {0, 1, 3, 4, 5, 6, 8, 10});
        
        // Sheet 4: Modal Scales
        addScale("Modal Scales", "Ionian b5", {0, 2, 4, 5, 6, 9, 11});
        addScale("Modal Scales", "Ionian #5", {0, 2, 4, 5, 8, 9, 11});
        addScale("Modal Scales", "Ionian Augmented #2", {0, 3, 4, 5, 8, 9, 11});
        addScale("Modal Scales", "Ionian Augmented b9", {0, 1, 4, 5, 8, 9, 11});
        addScale("Modal Scales", "Minor Hexatonic", {0, 2, 3, 5, 7, 10});
        addScale("Modal Scales", "Major Locrian", {0, 2, 4, 5, 6, 8, 10});
        addScale("Modal Scales", "Jazz Minor #5", {0, 2, 3, 5, 8, 9, 11});
        addScale("Modal Scales", "Full Minor All Flats", {0, 2, 3, 5, 7, 8, 9, 10, 11});
        addScale("Modal Scales", "Dorian Aeolian", {0, 2, 3, 5, 7, 8, 9, 10});
        addScale("Modal Scales", "Dorian b2 b4", {0, 1, 3, 4, 7, 9, 10});
        addScale("Modal Scales", "Dorian b2 Maj7", {0, 1, 3, 4, 6, 9, 11});
        addScale("Modal Scales", "Dorian b9 #11", {0, 1, 3, 6, 7, 9, 10});
        addScale("Modal Scales", "Phrygian Hexatonic", {0, 3, 5, 7, 8, 10});
        addScale("Modal Scales", "Phrygian Aeolian b4", {0, 1, 2, 3, 5, 7, 8, 10});
        addScale("Modal Scales", "Phrygian b4", {0, 1, 3, 4, 7, 8, 10});
        addScale("Modal Scales", "Phrygian b4 Maj7", {0, 1, 3, 4, 7, 8, 11});
        addScale("Modal Scales", "Double Phrygian", {0, 1, 3, 5, 6, 9});
        addScale("Modal Scales", "Ultraphrygian", {0, 1, 3, 4, 7, 8, 9});
        addScale("Modal Scales", "Lydian Hexatonic", {0, 2, 4, 7, 9, 11});
        addScale("Modal Scales", "Lydian #2 Hexatonic", {0, 3, 4, 7, 9, 11});
        addScale("Modal Scales", "Lydian #2 #6", {0, 3, 4, 6, 7, 10, 11});
        addScale("Modal Scales", "Lydian Dominant b6", {0, 2, 4, 6, 7, 8, 10});
        addScale("Modal Scales", "Lydian Mixolydian", {0, 2, 4, 5, 6, 7, 9, 10, 11});
        addScale("Modal Scales", "Lydian Diminished", {0, 2, 3, 6, 7, 9, 11});
        addScale("Modal Scales", "Lydian #6", {0, 2, 4, 6, 7, 10, 11});
        addScale("Modal Scales", "Lydian Augmented Dominant", {0, 2, 4, 6, 8, 9, 10});
        addScale("Modal Scales", "Mixolydian Hexatonic", {0, 2, 5, 7, 9, 10});
        addScale("Modal Scales", "Mixolydian b5", {0, 2, 4, 5, 6, 9, 10});
        addScale("Modal Scales", "Mixolydian Augmented", {0, 2, 4, 5, 8, 9, 10});
        addScale("Modal Scales", "Mixolydian Augmented Maj9", {0, 1, 4, 5, 8, 9, 10});
        addScale("Modal Scales", "Aeolian b1", {0, 3, 4, 6, 8, 9, 11});
        addScale("Modal Scales", "Locrian Dominant", {0, 1, 4, 5, 6, 8, 10});
        addScale("Modal Scales", "Locrian bb7", {0, 1, 3, 5, 6, 8, 9});
        addScale("Modal Scales", "Locrian bb3 bb7", {0, 1, 2, 5, 6, 8, 9});
        addScale("Modal Scales", "Locrian Maj7", {0, 1, 3, 5, 6, 8, 11});
        addScale("Modal Scales", "Semilocrian b4", {0, 2, 3, 4, 6, 8, 10});
        addScale("Modal Scales", "Superlocrian bb3", {0, 1, 2, 4, 6, 8, 10});
        addScale("Modal Scales", "Superlocrian Maj7", {0, 1, 3, 4, 6, 8, 11});
        addScale("Modal Scales", "Superlocrian bb6 bb7", {0, 1, 3, 4, 6, 7, 9});
        addScale("Modal Scales", "Superlocrian #6", {0, 1, 3, 4, 6, 9, 10});
        addScale("Modal Scales", "Ultralocrian bb3", {0, 1, 2, 4, 6, 8, 9});
        addScale("Modal Scales", "Harmonic Major", {0, 2, 4, 5, 7, 8, 11});
        addScale("Modal Scales", "Harmonic Major 2", {0, 2, 4, 5, 8, 9, 11});
        addScale("Modal Scales", "Harmonic Minor b5", {0, 2, 3, 5, 6, 8, 11});
        addScale("Modal Scales", "Harmonic Minor Inverse", {0, 1, 4, 5, 7, 9, 10});
        addScale("Modal Scales", "Double Harmonic", {0, 1, 4, 5, 7, 8, 11});
        addScale("Modal Scales", "Chromatic Dorian", {0, 1, 2, 5, 7, 8, 9});
        addScale("Modal Scales", "Chromatic Dorian Inverse", {0, 3, 4, 5, 7, 10, 11});
        addScale("Modal Scales", "Chromatic Diatonic Dorian", {0, 1, 2, 3, 5, 7, 8, 9, 10});
        addScale("Modal Scales", "Chromatic Phrygian", {0, 3, 4, 5, 8, 10, 11});
        addScale("Modal Scales", "Chromatic Phrygian Inverse", {0, 1, 2, 4, 7, 8, 9});
        addScale("Modal Scales", "Chromatic Lydian", {0, 1, 4, 5, 6, 9, 11});
        addScale("Modal Scales", "Chromatic Lydian Inverse", {0, 1, 3, 6, 7, 8, 11});
        addScale("Modal Scales", "Chromatic Mixolydian", {0, 1, 2, 5, 6, 7, 10});
        addScale("Modal Scales", "Chromatic Mixolydian 2", {0, 1, 2, 4, 6, 7, 10});
        addScale("Modal Scales", "Chromatic Mixolydian Inverse", {0, 2, 5, 6, 7, 10, 11});
        addScale("Modal Scales", "Chromatic Hypodorian", {0, 2, 3, 4, 7, 8, 9});
        addScale("Modal Scales", "Chromatic Hypodorian Inverse", {0, 3, 4, 5, 8, 9, 10});
        addScale("Modal Scales", "Chromatic Hypolydian", {0, 1, 4, 6, 7, 8, 11});
        addScale("Modal Scales", "Chromatic Hypophrygian Inverse", {0, 1, 2, 5, 6, 7, 9});
        addScale("Modal Scales", "Chromatic Permutated Diatonic Dorian", {0, 1, 2, 4, 5, 7, 8, 9, 11});
        addScale("Modal Scales", "Major Minor Mixed", {0, 2, 3, 4, 5, 7, 8, 9, 10, 11});
        addScale("Modal Scales", "Minor Pentatonic with Leading Tones", {0, 2, 3, 4, 5, 6, 7, 9, 10, 11});
        addScale("Modal Scales", "Leading Whole-Tone", {0, 2, 4, 6, 8, 10, 11});
        
        // Sheet 5: Pentatonic Scales
        addScale("Pentatonic Scales", "Major Pentatonic", {0, 2, 4, 7, 9});
        addScale("Pentatonic Scales", "Suspended Pentatonic", {0, 2, 5, 7, 10});
        addScale("Pentatonic Scales", "Man Gong", {0, 3, 5, 8, 10});
        addScale("Pentatonic Scales", "Ritusen", {0, 2, 5, 7, 9});
        addScale("Pentatonic Scales", "Minor Pentatonic", {0, 3, 5, 7, 10});
        addScale("Pentatonic Scales", "Dorian Pentatonic", {0, 2, 3, 7, 9});
        addScale("Pentatonic Scales", "Kokin-Choshi", {0, 1, 5, 7, 10});
        addScale("Pentatonic Scales", "Raga Hindol", {0, 4, 6, 9, 11});
        addScale("Pentatonic Scales", "Han-Kumoi", {0, 2, 5, 7, 8});
        addScale("Pentatonic Scales", "Minor Pentatonic 7 b5", {0, 3, 5, 6, 10});
        addScale("Pentatonic Scales", "Ionian Pentatonic", {0, 4, 5, 7, 11});
        addScale("Pentatonic Scales", "Pelog Pentatonic", {0, 1, 3, 7, 8});
        addScale("Pentatonic Scales", "Raga Hamsanada", {0, 2, 6, 7, 11});
        addScale("Pentatonic Scales", "Raga Khamaji Durga", {0, 4, 5, 9, 10});
        addScale("Pentatonic Scales", "Dominant Pentatonic", {0, 2, 4, 7, 10});
        addScale("Pentatonic Scales", "Chaio", {0, 2, 5, 8, 10});
        addScale("Pentatonic Scales", "Chin", {0, 3, 6, 8, 10});
        addScale("Pentatonic Scales", "Kyemyonjo", {0, 3, 5, 7, 9});
        addScale("Pentatonic Scales", "Kung", {0, 2, 4, 6, 9});
        addScale("Pentatonic Scales", "In", {0, 1, 5, 7, 8});
        addScale("Pentatonic Scales", "Hirajoshi", {0, 4, 6, 7, 11});
        addScale("Pentatonic Scales", "Ake-Bono", {0, 2, 3, 7, 8});
        addScale("Pentatonic Scales", "Iwato", {0, 1, 5, 6, 10});
        addScale("Pentatonic Scales", "Major Pentatonic b2", {0, 1, 4, 7, 9});
        addScale("Pentatonic Scales", "Major Pentatonic b2 b5", {0, 1, 4, 6, 9});
        addScale("Pentatonic Scales", "Major Pentatonic b3", {0, 1, 3, 6, 9});
        addScale("Pentatonic Scales", "Major Pentatonic b6", {0, 2, 4, 7, 8});
        addScale("Pentatonic Scales", "Major Pentatonic b7 #9", {0, 3, 4, 7, 10});
        addScale("Pentatonic Scales", "Mixolydian Pentatonic", {0, 4, 5, 7, 10});
        addScale("Pentatonic Scales", "Tcherepnin Major Pentatonic", {0, 2, 5, 7, 11});
        addScale("Pentatonic Scales", "Altered Pentatonic", {0, 1, 5, 7, 9});
        addScale("Pentatonic Scales", "Locrian Pentatonic", {0, 3, 4, 6, 10});
        addScale("Pentatonic Scales", "Pentatonic Whole-Tone", {0, 4, 6, 8, 10});
        addScale("Pentatonic Scales", "Center-Cluster PentaMirror", {0, 3, 4, 5, 8});
        addScale("Pentatonic Scales", "Raga Nagaswaravali", {0, 4, 5, 7, 9});
        addScale("Pentatonic Scales", "Raga Chitthakarshini", {0, 1, 3, 5, 8});
        addScale("Pentatonic Scales", "Raga Hamsadhvani 2", {0, 2, 4, 7, 11});
        addScale("Pentatonic Scales", "Pyeong Jo", {0, 2, 5, 9, 10});
        addScale("Pentatonic Scales", "Raga Shailaja", {0, 3, 7, 8, 10});
        addScale("Pentatonic Scales", "Pygmy", {0, 2, 3, 7, 10});
        addScale("Pentatonic Scales", "Raga Mamata", {0, 4, 7, 9, 11});
        addScale("Pentatonic Scales", "Raga Kokil Pancham", {0, 3, 5, 7, 8});
        addScale("Pentatonic Scales", "Romanian Bacovia", {0, 4, 5, 8, 11});
        addScale("Pentatonic Scales", "Syrian Pentatonic", {0, 1, 4, 5, 8});
        
        // Sheet 6: Jazz Scales
        addScale("Jazz Scales", "Blues", {0, 3, 5, 6, 7, 10});
        addScale("Jazz Scales", "Blues Heptatonic", {0, 2, 3, 5, 6, 9, 10});
        addScale("Jazz Scales", "Blues Heptatonic 2", {0, 3, 5, 6, 7, 9, 10});
        addScale("Jazz Scales", "Blues Octatonic", {0, 2, 3, 5, 6, 7, 9, 10});
        addScale("Jazz Scales", "Blues Enneatonic", {0, 2, 3, 4, 5, 7, 9, 10, 11});
        addScale("Jazz Scales", "Blues Enneatonic 2", {0, 2, 3, 4, 5, 6, 7, 9, 10});
        addScale("Jazz Scales", "Blues Dorian Hexatonic", {0, 1, 3, 4, 7, 9});
        addScale("Jazz Scales", "Blues Phrygian", {0, 1, 3, 5, 6, 7, 10});
        addScale("Jazz Scales", "Blues Minor Maj7", {0, 3, 5, 6, 7, 11});
        addScale("Jazz Scales", "Blues Modified", {0, 2, 3, 5, 6, 7, 10});
        addScale("Jazz Scales", "Blues Leading Tone", {0, 3, 5, 6, 7, 10, 11});
        addScale("Jazz Scales", "Rock 'n Roll", {0, 3, 4, 5, 7, 9, 10});
        addScale("Jazz Scales", "Bebop", {0, 2, 4, 5, 7, 9, 10, 11});
        addScale("Jazz Scales", "Bebop Major", {0, 2, 4, 5, 7, 8, 9, 11});
        addScale("Jazz Scales", "Bebop Major Hexatonic", {0, 2, 4, 7, 8, 9});
        addScale("Jazz Scales", "Bebop Major Heptatonic", {0, 2, 4, 5, 7, 8, 9});
        addScale("Jazz Scales", "Bebop Minor", {0, 2, 3, 4, 7, 9, 10});
        addScale("Jazz Scales", "Bebop Dorian", {0, 2, 3, 4, 5, 7, 9, 10});
        addScale("Jazz Scales", "Bebop Melodic Minor", {0, 2, 3, 5, 7, 8, 9, 11});
        addScale("Jazz Scales", "Bebop Harmonic Minor", {0, 2, 3, 5, 7, 8, 10, 11});
        addScale("Jazz Scales", "Bebop Half-diminished", {0, 1, 3, 5, 6, 7, 8, 11});
        addScale("Jazz Scales", "Bebop Locrian", {0, 1, 3, 5, 6, 7, 8, 10});
        addScale("Jazz Scales", "Bebop Chromatic", {0, 1, 2, 4, 5, 7, 9, 10, 11});
        
        // Sheet 7: Asian Scales
        addScale("Asian Scales", "Honkoshi", {0, 1, 3, 5, 6, 10});
        addScale("Asian Scales", "Ichilkotsucho", {0, 2, 4, 5, 6, 7, 9, 11});
        addScale("Asian Scales", "Insen", {0, 1, 5, 7, 8, 10});
        addScale("Asian Scales", "Maqam Shadd'araban", {0, 1, 3, 4, 5, 6, 9, 10});
        addScale("Asian Scales", "Maqam Hijaz", {0, 1, 4, 5, 7, 8, 10, 11});
        addScale("Asian Scales", "Maqam Shawq Afza", {0, 2, 3, 4, 5, 6, 7, 8, 9, 11});
        addScale("Asian Scales", "Maqam Tarzanuyn", {0, 1, 3, 4, 5, 6, 7, 8, 9, 10});
        addScale("Asian Scales", "Nando-Kyemyonjo", {0, 2, 3, 5, 7});
        addScale("Asian Scales", "Noh", {0, 2, 5, 7, 8, 9, 11});
        addScale("Asian Scales", "Nohkan", {0, 2, 5, 6, 8, 9, 11});
        addScale("Asian Scales", "Oriental", {0, 1, 4, 5, 6, 9, 10});
        addScale("Asian Scales", "Oriental 2", {0, 1, 4, 5, 6, 9, 10, 11});
        addScale("Asian Scales", "Pelog", {0, 2, 4, 6, 7, 8, 11});
        addScale("Asian Scales", "Persian", {0, 1, 4, 5, 6, 8, 11});
        addScale("Asian Scales", "Ritzu", {0, 1, 3, 5, 8, 10});
        addScale("Asian Scales", "Sho", {0, 2, 3, 5, 7, 9});
        addScale("Asian Scales", "Sho #2", {0, 1, 3, 4, 6, 10});
        addScale("Asian Scales", "Takemitzu Tree 1", {0, 2, 3, 6, 8, 11});
        addScale("Asian Scales", "Takemitzu Tree 2", {0, 2, 3, 6, 8, 10});
        addScale("Asian Scales", "Youlan", {0, 1, 2, 4, 5, 6, 7, 9, 10});
        
        // Sheet 8: Indian Scales
        addScale("Indian Scales", "Mela Bhavapriya", {0, 1, 3, 6, 7, 8, 10});
        addScale("Indian Scales", "Mela Calanata", {0, 3, 4, 5, 7, 10});
        addScale("Indian Scales", "Mela Dhavalambari", {0, 1, 4, 6, 7, 8, 9});
        addScale("Indian Scales", "Mela Dhatuvardhani", {0, 3, 4, 6, 7, 8, 11});
        addScale("Indian Scales", "Mela Divyamani", {0, 1, 3, 6, 7, 10, 11});
        addScale("Indian Scales", "Mela Ganamurti", {0, 1, 2, 5, 7, 8, 11});
        addScale("Indian Scales", "Mela Gavambodhi", {0, 1, 3, 6, 7, 8, 9});
        addScale("Indian Scales", "Mela Gayakapriya", {0, 1, 4, 5, 7, 9, 10, 11});
        addScale("Indian Scales", "Mela Hatakambari", {0, 1, 4, 5, 7, 10, 11});
        addScale("Indian Scales", "Mela Jalarnava", {0, 1, 2, 5, 6, 7, 9, 11});
        addScale("Indian Scales", "Mela Jhalavarli", {0, 1, 2, 5, 6, 7, 10, 11});
        addScale("Indian Scales", "Mela Jhankaradhvani", {0, 2, 3, 5, 7, 8, 9});
        addScale("Indian Scales", "Mela Jyotisvarupini", {0, 3, 4, 6, 7, 8, 10});
        addScale("Indian Scales", "Mela Kantamani", {0, 2, 4, 6, 7, 8, 9});
        addScale("Indian Scales", "Mela Manavati", {0, 1, 2, 5, 7, 9, 11});
        addScale("Indian Scales", "Mela Naganandini", {0, 2, 4, 5, 7, 10, 11});
        addScale("Indian Scales", "Mela Namanarayani", {0, 1, 4, 6, 7, 8, 10});
        addScale("Indian Scales", "Mela Navanitam", {0, 1, 2, 6, 7, 9, 10});
        addScale("Indian Scales", "Mela Nitimati", {0, 2, 3, 6, 7, 10, 11});
        addScale("Indian Scales", "Mela Pavani", {0, 1, 2, 6, 7, 9, 11});
        addScale("Indian Scales", "Mela Ragavardhani", {0, 3, 4, 5, 7, 8, 10});
        addScale("Indian Scales", "Mela Raghupriya", {0, 1, 2, 6, 7, 10, 11});
        addScale("Indian Scales", "Mela Ratnangi", {0, 1, 2, 5, 7, 8, 10});
        addScale("Indian Scales", "Mela Rupavati", {0, 1, 3, 5, 7, 10, 11});
        addScale("Indian Scales", "Mela Salaga", {0, 1, 2, 6, 7, 8, 9});
        addScale("Indian Scales", "Mela Syamalangi", {0, 2, 3, 6, 7, 8, 9});
        addScale("Indian Scales", "Mela Suvarnangi", {0, 1, 3, 6, 7, 9, 11});
        addScale("Indian Scales", "Mela Tenarupi", {0, 1, 2, 5, 7, 10, 11});
        addScale("Indian Scales", "Mela Venaspati", {0, 1, 2, 5, 7, 9, 10});
        addScale("Indian Scales", "Mela Varunapriya", {0, 2, 3, 5, 7, 10, 11});
        addScale("Indian Scales", "Mela Visvambhari", {0, 1, 4, 6, 7, 10, 11});
        addScale("Indian Scales", "Mela Yagapriya", {0, 3, 4, 5, 7, 8, 9});
        
        // Add Raga scales - Page 10-11
        addScale("Indian Scales", "Raga Abhogi", {0, 2, 3, 5, 9});
        addScale("Indian Scales", "Raga Aivarati", {0, 2, 4, 6, 7, 9});
        addScale("Indian Scales", "Raga Amarasenapriya", {0, 2, 3, 6, 7, 11});
        addScale("Indian Scales", "Raga Audav Tukhari", {0, 2, 3, 5, 8});
        addScale("Indian Scales", "Raga Bhatiyar", {0, 1, 4, 5, 6, 7, 9, 11});
        addScale("Indian Scales", "Raga Bhinna Pancama", {0, 2, 5, 7, 8, 11});
        addScale("Indian Scales", "Raga Brindabani", {0, 2, 5, 7, 10, 11});
        addScale("Indian Scales", "Raga Bowli (asc)", {0, 1, 4, 7, 8});
        addScale("Indian Scales", "Raga Bowli (desc)", {0, 1, 4, 7, 8, 11});
        addScale("Indian Scales", "Raga Budhamanohari", {0, 2, 4, 5, 7});
        addScale("Indian Scales", "Raga Chandrajyoti", {0, 1, 2, 6, 7, 9});
        addScale("Indian Scales", "Raga Chandrakauns Kafi", {0, 3, 5, 9, 10});
        addScale("Indian Scales", "Raga Chandrakauns Kiravani", {0, 3, 5, 8, 11});
        addScale("Indian Scales", "Raga Chandrakauns Modern", {0, 3, 5, 9, 11});
        addScale("Indian Scales", "Raga Chaya Todi", {0, 1, 3, 6, 8});
        addScale("Indian Scales", "Raga Chinthamani", {0, 2, 3, 6, 7, 8, 9, 10});
        addScale("Indian Scales", "Raga Deshgaur", {0, 1, 7, 8, 11});
        addScale("Indian Scales", "Raga Devaranjani", {0, 5, 7, 8, 11});
        addScale("Indian Scales", "Raga Dhavalangam", {0, 1, 4, 6, 7, 8});
        addScale("Indian Scales", "Raga Dhavalashri", {0, 4, 6, 7, 9});
        addScale("Indian Scales", "Raga Dipak", {0, 2, 4, 5, 6, 7});
        addScale("Indian Scales", "Raga Gamakakriya", {0, 1, 4, 6, 7, 11});
        addScale("Indian Scales", "Raga Gandharavam", {0, 1, 3, 5, 7, 10});
        addScale("Indian Scales", "Raga Gangatarangini", {0, 4, 5, 6, 8, 11});
        addScale("Indian Scales", "Raga Gaula", {0, 1, 4, 5, 7, 10});
        addScale("Indian Scales", "Raga Gaurikriya", {0, 3, 6, 7, 10, 11});
        addScale("Indian Scales", "Raga Ghantana", {0, 2, 3, 5, 8, 11});
        addScale("Indian Scales", "Raga Gopikatilaka", {0, 2, 3, 6, 7, 10});
        addScale("Indian Scales", "Raga Gowla (asc)", {0, 1, 5, 7, 11});
        addScale("Indian Scales", "Raga Gowla (desc)", {0, 1, 4, 5, 7, 11});
        addScale("Indian Scales", "Raga Gurjari Todi", {0, 1, 3, 6, 8, 10});
        addScale("Indian Scales", "Raga Hamsadhvani", {0, 2, 3, 7, 11});
        addScale("Indian Scales", "Raga Hansanandi", {0, 1, 4, 6, 9, 11});
        addScale("Indian Scales", "Raga Hamsa Vinodini", {0, 2, 4, 5, 9, 11});
        addScale("Indian Scales", "Raga Hari Nata", {0, 4, 5, 7, 9, 11});
        addScale("Indian Scales", "Raga Hejjajji", {0, 1, 4, 6, 8, 9});
        addScale("Indian Scales", "Raga Jaganmohanam", {0, 2, 6, 7, 8, 10});
        addScale("Indian Scales", "Raga Jivantika", {0, 1, 5, 7, 9, 11});
        addScale("Indian Scales", "Raga Jyoty", {0, 4, 6, 7, 8, 10});
        addScale("Indian Scales", "Raga Kalagada", {0, 1, 4, 7, 8, 9});
        addScale("Indian Scales", "Raga Kalakanthi", {0, 2, 3, 7, 8, 9});
        addScale("Indian Scales", "Raga Kalavati", {0, 1, 4, 5, 7, 9});
        addScale("Indian Scales", "Raga Kamalamanohari", {0, 2, 6, 7, 9, 10});
        addScale("Indian Scales", "Raga Kashyapi", {0, 1, 3, 7, 8, 10});
        addScale("Indian Scales", "Raga Kedaram (asc)", {0, 4, 5, 7, 11});
        addScale("Indian Scales", "Raga Kedaram (desc)", {0, 2, 4, 5, 7, 11});
        
        // Page 12
        addScale("Indian Scales", "Raga Khamach (asc)", {0, 4, 5, 7, 9, 10, 11});
        addScale("Indian Scales", "Raga Khamach (desc)", {0, 2, 4, 5, 7, 9, 10});
        addScale("Indian Scales", "Raga Kshanika", {0, 1, 5, 8, 11});
        addScale("Indian Scales", "Raga Kumarapriya", {0, 1, 2, 8, 11});
        addScale("Indian Scales", "Raga Kumurdaki", {0, 2, 4, 6, 11});
        addScale("Indian Scales", "Raga Kuntvarali", {0, 5, 7, 9, 10});
        addScale("Indian Scales", "Raga Lalita", {0, 1, 4, 5, 8, 11});
        addScale("Indian Scales", "Raga Lalita Bhairav", {0, 1, 4, 5, 8, 10});
        addScale("Indian Scales", "Raga Latika", {0, 2, 4, 7, 8, 11});
        addScale("Indian Scales", "Raga Madhukauns", {0, 3, 6, 7, 9, 10});
        addScale("Indian Scales", "Raga Malarani", {0, 2, 6, 7, 10, 11});
        addScale("Indian Scales", "Raga Malayamarutam", {0, 1, 4, 7, 9, 10});
        addScale("Indian Scales", "Raga Malahari (asc)", {0, 1, 5, 7, 8});
        addScale("Indian Scales", "Raga Malahari (desc)", {0, 1, 4, 5, 7, 8});
        addScale("Indian Scales", "Raga Malkauns", {0, 3, 5, 8, 10, 11});
        addScale("Indian Scales", "Raga Malini", {0, 1, 3, 5, 7, 8, 9});
        addScale("Indian Scales", "Raga Manaranjani", {0, 1, 4, 7, 10});
        addScale("Indian Scales", "Raga Manavi", {0, 2, 3, 7, 9, 10});
        addScale("Indian Scales", "Raga Manohari", {0, 3, 5, 7, 9, 10});
        addScale("Indian Scales", "Raga Marwa Thaat", {0, 1, 4, 6, 7, 9, 11});
        addScale("Indian Scales", "Raga Matha Kokila", {0, 2, 7, 9, 10});
        addScale("Indian Scales", "Raga Megharamji", {0, 1, 4, 5, 11});
        addScale("Indian Scales", "Raga Miam Ki Malhar", {0, 2, 3, 5, 7, 9, 10, 11});
        addScale("Indian Scales", "Raga Mohanangi", {0, 3, 4, 7, 9});
        addScale("Indian Scales", "Raga Mruganandana", {0, 2, 4, 6, 9, 11});
        addScale("Indian Scales", "Raga Multani", {0, 2, 5, 8, 9, 11});
        addScale("Indian Scales", "Raga Nabhomani", {0, 1, 2, 6, 7});
        addScale("Indian Scales", "Raga Nagagandhari", {0, 2, 5, 7, 9, 11});
        addScale("Indian Scales", "Raga Nattai (asc)", {0, 3, 4, 5, 7, 10, 11});
        addScale("Indian Scales", "Raga Nattai (desc)", {0, 3, 5, 7, 11});
        addScale("Indian Scales", "Raga Nattaikurinji", {0, 2, 4, 5, 9, 10});
        addScale("Indian Scales", "Raga Navamanohari", {0, 2, 5, 7, 8, 10});
        addScale("Indian Scales", "Raga Neroshta", {0, 2, 4, 9, 11});
        addScale("Indian Scales", "Raga Nishadi", {0, 2, 6, 7, 9, 11});
        addScale("Indian Scales", "Raga Padi", {0, 1, 5, 7, 8, 11});
        addScale("Indian Scales", "Raga Pahadi", {0, 2, 4, 5, 7, 8, 9, 10, 11});
        addScale("Indian Scales", "Raga Paras (asc)", {0, 4, 5, 7, 8, 11});
        addScale("Indian Scales", "Raga Paras (desc)", {0, 1, 4, 5, 7, 8, 11});
        addScale("Indian Scales", "Raga Priyadharshini", {0, 2, 5, 8, 11});
        
        // Page 13
        addScale("Indian Scales", "Raga Puruhutika", {0, 5, 7, 9, 11});
        addScale("Indian Scales", "Raga Putrika", {0, 1, 2, 8, 9});
        addScale("Indian Scales", "Raga Rageshri", {0, 2, 4, 5, 9, 10, 11});
        addScale("Indian Scales", "Raga Ramkali", {0, 1, 4, 5, 6, 7, 8, 11});
        addScale("Indian Scales", "Raga Rangini", {0, 2, 3, 6, 9, 11});
        addScale("Indian Scales", "Raga Rasamanjari", {0, 2, 5, 6, 8, 9, 11});
        addScale("Indian Scales", "Raga Rasavali", {0, 1, 5, 7, 9, 10});
        addScale("Indian Scales", "Raga Rasranjani", {0, 2, 5, 9, 11});
        addScale("Indian Scales", "Raga Ratnakanthi", {0, 2, 4, 6, 7, 11});
        addScale("Indian Scales", "Raga Rudra Pancama", {0, 1, 4, 5, 9, 10});
        addScale("Indian Scales", "Raga Rukmangi", {0, 1, 3, 7, 10});
        addScale("Indian Scales", "Raga Salagavarali", {0, 1, 3, 7, 9, 10});
        addScale("Indian Scales", "Raga Samudhra Priya", {0, 3, 6, 7, 10});
        addScale("Indian Scales", "Raga Santanamanjari", {0, 3, 4, 6, 7, 8, 9});
        addScale("Indian Scales", "Raga Sarasanana", {0, 2, 4, 5, 8, 11});
        addScale("Indian Scales", "Raga Sarasvati", {0, 2, 6, 7, 9, 10});
        addScale("Indian Scales", "Raga Saravati", {0, 4, 5, 7, 8, 9});
        addScale("Indian Scales", "Raga Saugandhini", {0, 1, 6, 7, 8});
        addScale("Indian Scales", "Raga Saurashtra", {0, 1, 4, 5, 7, 8, 9, 11});
        addScale("Indian Scales", "Raga Shreeranjani", {0, 2, 3, 5, 9, 10});
        addScale("Indian Scales", "Raga Shri Kalyan", {0, 2, 6, 7, 9});
        addScale("Indian Scales", "Raga Shubravarni", {0, 2, 6, 9, 10});
        addScale("Indian Scales", "Raga Sindhura Kafi", {0, 2, 3, 5, 7, 11});
        addScale("Indian Scales", "Raga Sindhi-Bhairavi", {0, 1, 2, 3, 4, 5, 7, 8, 10, 11});
        addScale("Indian Scales", "Raga Siva Kambhoji", {0, 2, 4, 5, 7, 10});
        addScale("Indian Scales", "Raga Sorati", {0, 2, 5, 7, 9, 10, 11});
        addScale("Indian Scales", "Raga Suddha Mukhari", {0, 1, 2, 5, 8, 9});
        addScale("Indian Scales", "Raga Suddha Simantini", {0, 1, 3, 5, 7, 8});
        addScale("Indian Scales", "Raga Syamalam", {0, 2, 3, 6, 7, 8});
        addScale("Indian Scales", "Raga Takka", {0, 3, 5, 7, 8, 11});
        addScale("Indian Scales", "Raga Tilang", {0, 4, 5, 7, 10, 11});
        addScale("Indian Scales", "Raga Trimurti", {0, 2, 3, 7, 8, 10});
        addScale("Indian Scales", "Raga Valaji", {0, 4, 7, 9, 10});
        addScale("Indian Scales", "Raga Vasanta (asc)", {0, 4, 5, 9, 11});
        addScale("Indian Scales", "Raga Vasanta (desc)", {0, 1, 4, 5, 9, 11});
        addScale("Indian Scales", "Raga Vegavahini (asc)", {0, 4, 5, 7, 9, 10});
        addScale("Indian Scales", "Raga Vegavahini (desc)", {0, 1, 4, 5, 7, 9, 10});
        addScale("Indian Scales", "Raga Vijayanagari", {0, 2, 3, 6, 7, 9});
        addScale("Indian Scales", "Raga Vijayasri", {0, 1, 2, 6, 7, 11});
        
        addScale("Indian Scales", "Raga Vijayavasanta", {0, 4, 6, 7, 10, 11});
        addScale("Indian Scales", "Raga Viyogavarali", {0, 1, 3, 5, 8, 11});
        addScale("Indian Scales", "Raga Vutari", {0, 4, 6, 7, 9, 10});
        addScale("Indian Scales", "Raga Zilaf", {0, 4, 6, 7, 9, 10});
        
        // Sheet 9: Miscellaneous scales
        addScale("Miscellaneous scales", "Algerian Octatonic", {0, 2, 3, 5, 6, 7, 8, 11});
        addScale("Miscellaneous scales", "Algerian", {0, 2, 3, 6, 7, 8, 11});
        addScale("Miscellaneous scales", "Eskimo Hexatonic", {0, 2, 4, 6, 8, 9});
        addScale("Miscellaneous scales", "Eskimo Hexatonic 2", {0, 2, 4, 6, 8, 11});
        addScale("Miscellaneous scales", "Hamel", {0, 1, 3, 5, 7, 8, 10, 11});
        addScale("Miscellaneous scales", "Hawaiian", {0, 2, 3, 7, 9, 11});
        addScale("Miscellaneous scales", "LG Octatonic", {0, 1, 3, 4, 5, 7, 9, 10});
        addScale("Miscellaneous scales", "Pyramid Hexatonic", {0, 2, 3, 5, 6, 9});
        addScale("Miscellaneous scales", "Nonatonic 2", {0, 1, 3, 4, 5, 6, 7, 9, 10});
        addScale("Miscellaneous scales", "Symmetrical Nonatonic", {0, 1, 2, 4, 6, 7, 8, 10, 11});
    }
    
    void addScale(const std::string& sheetName, const std::string& scaleName, const std::vector<int>& intervals) {
        ScaleInfo scale;
        scale.sheetName = sheetName;
        scale.scaleName = scaleName;
        scale.intervals = intervals;
        scales.push_back(scale);
    }
};

std::vector<int> parseInput(const std::string& input) {
    std::vector<int> result;
    std::istringstream iss(input);
    int num;
    while (iss >> num) {
        result.push_back(num);
    }
    return result;
}

std::string getRootNote(const std::vector<int>& intervals) {
    if (intervals.empty()) return "C";
    
    // TODO: link with the actual note naming system
    std::vector<std::string> noteNames = {
        "C", "C#", "D", "D#", "E", "F", 
        "F#", "G", "G#", "A", "A#", "B"
    };
    
    int root = intervals[0] % 12;
    return noteNames[root];
}

} // namespace musicpp

#endif // MUSICPP_SCALE_DICTIONARY_H
