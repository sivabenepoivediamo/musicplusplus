#ifndef SCALE_DICTIONARY_H
#define SCALE_DICTIONARY_H

/**
 * @file scaleDictionary.h
 * @brief Scale dictionary and lookup functionality
 * Provides a database of musical scales in 12TET (based on the work of Francesco Balena - The Scale Omnibus) and methods to find matching scales
 * based on input pitch class sets.
 */

#include "./utility.h"

class ScaleDatabase {
private:
    struct ScaleInfo {
        string sheetName;
        string scaleName;
        vector<int> intervals;
        
        bool operator==(const ScaleInfo& other) const {
            return intervals == other.intervals;
        }
    };
    
    vector<ScaleInfo> scales;
    
public:
    ScaleDatabase() {
        initializeAllScales();
    }
    
    vector<ScaleInfo> findScale(const vector<int>& inputIntervals) {
        vector<ScaleInfo> results;
        
        if (inputIntervals.empty()) return results;
        
        vector<int> normalizedInput;
        int root = inputIntervals[0];
        for (int interval : inputIntervals) {
            normalizedInput.push_back(interval - root);
        }
        
        vector<int> processedInput = normalizedInput;
        sort(processedInput.begin(), processedInput.end());
        processedInput.erase(unique(processedInput.begin(), processedInput.end()), processedInput.end());
        
        for (const auto& scale : scales) {
            vector<int> sortedScale = scale.intervals;
            sort(sortedScale.begin(), sortedScale.end());
            
            if (processedInput == sortedScale) {
                results.push_back(scale);
            }
        }
        
        return results;
    }
    
    void displayResults(const vector<int>& inputIntervals, const string& rootNote = "C") {
        vector<ScaleInfo> foundScales = findScale(inputIntervals);
        
        cout << "\nInput notes: ";
        for (size_t i = 0; i < inputIntervals.size(); ++i) {
            cout << inputIntervals[i];
            if (i < inputIntervals.size() - 1) cout << " ";
        }
        cout << endl;
        
        if (foundScales.empty()) {
            cout << "No matching scale found in database." << endl;
            return;
        }
        
        cout << "Found " << foundScales.size() << " matching scale(s):" << endl;
        cout << "=================================" << endl;
        
        for (const auto& scale : foundScales) {
            cout << "Category: " << scale.sheetName << endl;
            cout << "Scale: " << rootNote << " " << scale.scaleName << endl;
            
            // Display intervals
            cout << "Pitch Classes: ";
            for (size_t i = 0; i < scale.intervals.size(); ++i) {
                cout << scale.intervals[i];
                if (i < scale.intervals.size() - 1) cout << " ";
            }
            cout << endl << endl;
        }
    }
    
    // Get all unique interval sets (for debugging)
    set<vector<int>> getAllIntervalSets() {
        set<vector<int>> uniqueSets;
        for (const auto& scale : scales) {
            vector<int> sorted = scale.intervals;
            sort(sorted.begin(), sorted.end());
            uniqueSets.insert(sorted);
        }
        return uniqueSets;
    }
    
private:
    void initializeAllScales() {
#include "../../shared-data/generated/scaleDictionaryData.inc"
    }
    
    void addScale(const string& sheetName, const string& scaleName, const vector<int>& intervals) {
        ScaleInfo scale;
        scale.sheetName = sheetName;
        scale.scaleName = scaleName;
        scale.intervals = intervals;
        scales.push_back(scale);
    }
};

vector<int> parseInput(const string& input) {
    vector<int> result;
    istringstream iss(input);
    int num;
    while (iss >> num) {
        result.push_back(num);
    }
    return result;
}

string getRootNote(const vector<int>& intervals) {
    if (intervals.empty()) return "C";
    
    // TODO: link with the actual note naming system
    vector<string> noteNames = {
        "C", "C#", "D", "D#", "E", "F", 
        "F#", "G", "G#", "A", "A#", "B"
    };
    
    int root = intervals[0] % 12;
    return noteNames[root];
}

#endif // SCALE_DICTIONARY_H

