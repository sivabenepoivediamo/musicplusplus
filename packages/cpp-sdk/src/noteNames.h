#ifndef NOTENAMES_H
#define NOTENAMES_H

#include "positionVector.h"

/**
 * @file NoteNaming.h
 * @brief Musical note naming system with enharmonic handling
 * @author [not251]
 * @date 2025
 */

/**
 * @enum AlterationDirection
 * @brief Direction of musical note alteration
 */
enum class AlterationDirection {
    NATURAL,  ///< No alteration (e.g., C)
    RIGHT,    ///< Sharp alteration (e.g., C♯)
    LEFT,     ///< Flat alteration (e.g., D♭)
    NONE      ///< No specific alteration
};

/**
 * @struct ClassifiedNote
 * @brief Represents a musical note with its classification
 */
struct ClassifiedNote {
    string note;                    ///< The musical note string
    AlterationDirection label;      ///< The direction of alteration
    
    ClassifiedNote(const string& n, AlterationDirection l) 
        : note(n), label(l) {}
};

/**
 * @struct NoteResult
 * @brief Result of MIDI to note name conversion
 */
struct NoteResult {
    vector<string> noteNames; 
    vector<string> centsInfo;   
    
    NoteResult() = default;
    NoteResult(const vector<string>& names, const vector<string>& cents)
        : noteNames(names), centsInfo(cents) {}
};

/**
 * @struct NoteMapperOptions
 * @brief Configuration options for note mapping
 */
struct NoteMapperOptions {
    bool preferSharps;              ///< Prefer sharp notation over flats
    bool isDiatonicScale;           ///< Enforce consecutive note names
    int moduloValue;                ///< Modulo value for MIDI processing
    
    NoteMapperOptions(bool sharps = true, bool diatonic = false, int modulo = 12)
        : preferSharps(sharps), isDiatonicScale(diatonic), moduloValue(modulo) {}
};

/**
 * @class NoteNamingSystem
 * @brief Complete system for converting MIDI numbers to note names
 */
class NoteNamingSystem {
private:
    /// Enharmonically equivalent notes
    vector<vector<string>> noteArrays = {
        {"C", "B♯", "D♭♭"},
        {"C♯", "D♭"},
        {"D", "C♯♯", "E♭♭"},
        {"D♯", "E♭"},
        {"E", "D♯♯", "F♭"},
        {"F", "E♯", "G♭♭"},
        {"F♯", "G♭"},
        {"G", "F♯♯", "A♭♭"},
        {"G♯", "A♭"},
        {"A", "G♯♯", "B♭♭"},
        {"A♯", "B♭"},
        {"B", "A♯♯", "C♭"}
    };
    
    /// Classified notes with their alteration directions
    vector<vector<ClassifiedNote>> classifiedNotes;
    
    /// Note order for consecutive checking
    const vector<char> noteOrder = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
    
    /**
     * @brief Classifies notes based on alteration direction
     */
    void classifyNotes() {
        classifiedNotes.clear();
        
        for (const auto& array : noteArrays) {
            vector<ClassifiedNote> classified;
            
            if (array.size() == 3) {
                classified.emplace_back(array[0], AlterationDirection::NATURAL);
                classified.emplace_back(array[1], AlterationDirection::RIGHT);
                classified.emplace_back(array[2], AlterationDirection::LEFT);
            } else if (array.size() == 2) {
                classified.emplace_back(array[0], AlterationDirection::RIGHT);
                classified.emplace_back(array[1], AlterationDirection::LEFT);
            } else {
                for (const auto& note : array) {
                    classified.emplace_back(note, AlterationDirection::NONE);
                }
            }
            
            classifiedNotes.push_back(classified);
        }
    }
    
    /**
     * @brief Extracts basic note name without accidentals
     */
    char getBasicNoteName(const string& noteName) const {
        return noteName.empty() ? '\0' : noteName[0];
    }
    
    /**
     * @brief Gets next consecutive note letter
     */
    char getNextNoteLetter(char currentNote) const {
        auto it = find(noteOrder.begin(), noteOrder.end(), currentNote);
        if (it == noteOrder.end()) return '\0';
        
        size_t index = distance(noteOrder.begin(), it);
        return noteOrder[(index + 1) % 7];
    }
    
    /**
     * @brief Checks if notes form consecutive sequence
     */
    bool areNotesConsecutive(const vector<string>& noteNames) const {
        if (noteNames.empty()) return true;
        
        vector<char> basicNotes;
        for (const auto& name : noteNames) {
            basicNotes.push_back(getBasicNoteName(name));
        }
        
        // Check all possible starting positions
        for (size_t start = 0; start < noteOrder.size(); ++start) {
            bool isValid = true;
            
            for (size_t i = 0; i < basicNotes.size(); ++i) {
                size_t expected = (start + i) % noteOrder.size();
                if (basicNotes[i] != noteOrder[expected]) {
                    isValid = false;
                    break;
                }
            }
            
            if (isValid) return true;
        }
        
        return false;
    }
    
    /**
     * @brief Checks if a configuration contains double sharps or double flats
     */
    bool hasDoubleAccidentals(const vector<string>& noteNames) const {
        for (const auto& note : noteNames) {
            if (note.find("♯♯") != string::npos || note.find("##") != string::npos ||
                note.find("♭♭") != string::npos || note.find("bb") != string::npos) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Scores a note configuration based on preference
     * Higher score = better match to preference
     */
    int scoreConfiguration(const vector<string>& noteNames, bool preferSharps) const {
        int score = 0;
        
        for (const auto& note : noteNames) {
            if (note.find("♯") != string::npos || note.find("#") != string::npos) {
                score += preferSharps ? 10 : -10;
            } else if (note.find("♭") != string::npos || note.find("b") != string::npos) {
                score += preferSharps ? -10 : 10;
            } else {
                score += 5; // Natural notes are neutral but slightly preferred
            }
        }
        
        return score;
    }
    
    /**
     * @brief Finds all possible consecutive note configurations
     * Returns the best configuration according to preferSharps
     */
    vector<string> findConsecutiveConfiguration(
        const vector<int>& noteIndices,
        bool preferSharps) const {
        
        if (noteIndices.size() != 7) {
            return {}; // Only works for 7-note scales
        }
        
        vector<vector<string>> validConfigurations;
        
        // Try all 7 possible starting letters for consecutive sequences
        for (size_t startIdx = 0; startIdx < 7; ++startIdx) {
            vector<string> candidate;
            bool isValid = true;
            
            // For each position in the scale
            for (size_t i = 0; i < 7; ++i) {
                char requiredLetter = noteOrder[(startIdx + i) % 7];
                int noteIndex = noteIndices[i];
                
                // Find an enharmonic equivalent with this letter
                const auto& possibleNotes = noteArrays[noteIndex];
                string foundNote = "";
                
                for (const auto& note : possibleNotes) {
                    if (getBasicNoteName(note) == requiredLetter) {
                        foundNote = note;
                        break;
                    }
                }
                
                if (foundNote.empty()) {
                    isValid = false;
                    break;
                }
                
                candidate.push_back(foundNote);
            }
            
            if (isValid && areNotesConsecutive(candidate)) {
                // For diatonic scales, filter out configurations with double accidentals
                if (!hasDoubleAccidentals(candidate)) {
                    validConfigurations.push_back(candidate);
                }
            }
        }
        
        // If no valid configurations found, return empty
        if (validConfigurations.empty()) {
            return {};
        }
        
        // Score all valid configurations and return the best one
        int bestScore = INT_MIN;
        vector<string> bestConfig;
        
        for (const auto& config : validConfigurations) {
            int score = scoreConfiguration(config, preferSharps);
            if (score > bestScore) {
                bestScore = score;
                bestConfig = config;
            }
        }
        
        return bestConfig;
    }
    
    /**
     * @brief Finds alternative enharmonic note with desired basic note
     */
    string findAlternativeWithBasicNote(const string& currentNote, 
                                       char desiredBasicNote) const {
        // Find current note's group
        for (const auto& group : noteArrays) {
            auto it = find(group.begin(), group.end(), currentNote);
            if (it != group.end()) {
                // Find alternative with desired basic note
                for (const auto& note : group) {
                    if (getBasicNoteName(note) == desiredBasicNote) {
                        return note;
                    }
                }
                break;
            }
        }
        
        return "";
    }
    
    /**
     * @brief Processes MIDI numbers with modulo and rounding
     */
    vector<pair<int, double>> processMidiNumbers(
        const vector<int>& midiNumbers,
        int moduloValue) const {
        
        vector<pair<int, double>> processed;
        
        for (int midi : midiNumbers) {
            // Calculate modulo-adjusted value
            double adjusted = static_cast<double>(midi);
            if (moduloValue > 0 && moduloValue != 12) {
                int modResult = ((midi % moduloValue) + moduloValue) % moduloValue;
                adjusted = modResult * (12.0 / moduloValue);
            }
            
            // Round to 2 decimal places
            adjusted = round(adjusted * 100.0) / 100.0;
            
            // Separate integer and decimal parts
            int intPart = static_cast<int>(floor(adjusted));
            double decPart = adjusted - intPart;
            decPart = round(decPart * 100.0) / 100.0;
            
            processed.emplace_back(intPart, decPart);
        }
        
        return processed;
    }
    
public:
    /**
     * @brief Constructor - initializes the note naming system
     */
    NoteNamingSystem() {
        classifyNotes();
    }
    
    /**
     * @brief Converts MIDI numbers to note names
     * 
     * @param midiNumbers Vector of MIDI note numbers (integers)
     * @param options Configuration options
     * @return NoteResult with note names and cents information
     */
    NoteResult midiNumbersToNoteNames(const vector<int>& midiNumbers,
                                     const NoteMapperOptions& options) const {
        if (midiNumbers.empty()) {
            return NoteResult();
        }
        
        // Process MIDI numbers
        auto processed = processMidiNumbers(midiNumbers, options.moduloValue);
        
        vector<int> integerParts;
        vector<double> decimalParts;
        
        for (const auto& [intPart, decPart] : processed) {
            integerParts.push_back(intPart);
            decimalParts.push_back(decPart);
        }
        
        // Get note indices (rounded for microtonal)
        vector<int> noteIndices;
        for (size_t i = 0; i < integerParts.size(); ++i) {
            int noteValue = integerParts[i];
            if (decimalParts[i] > 0.5) {
                noteValue++;
            }
            noteIndices.push_back(((noteValue % 12) + 12) % 12);
        }
        
        vector<string> result;
        
        // For diatonic 7-note scales, use the enhanced algorithm
        bool isDiatonic = options.isDiatonicScale && (noteIndices.size() == 7);
        
        if (isDiatonic) {
            // Try to find the best consecutive configuration
            result = findConsecutiveConfiguration(noteIndices, options.preferSharps);
            
            // If no valid consecutive configuration found, fall back to standard algorithm
            if (result.empty()) {
                isDiatonic = false; // Disable diatonic mode for fallback
            }
        }
        
        // Standard algorithm (non-diatonic or fallback)
        if (result.empty()) {
            for (size_t i = 0; i < noteIndices.size(); ++i) {
                int noteIndex = noteIndices[i];
                const auto& possibleNotes = classifiedNotes[noteIndex];
                
                // Prefer natural notes
                auto naturalIt = find_if(possibleNotes.begin(), possibleNotes.end(),
                    [](const ClassifiedNote& cn) { 
                        return cn.label == AlterationDirection::NATURAL; 
                    });
                
                if (naturalIt != possibleNotes.end()) {
                    result.push_back(naturalIt->note);
                } else if (options.preferSharps) {
                    auto sharpIt = find_if(possibleNotes.begin(), possibleNotes.end(),
                        [](const ClassifiedNote& cn) { 
                            return cn.label == AlterationDirection::RIGHT; 
                        });
                    result.push_back(sharpIt != possibleNotes.end() ? 
                                          sharpIt->note : possibleNotes[0].note);
                } else {
                    auto flatIt = find_if(possibleNotes.begin(), possibleNotes.end(),
                        [](const ClassifiedNote& cn) { 
                            return cn.label == AlterationDirection::LEFT; 
                        });
                    result.push_back(flatIt != possibleNotes.end() ? 
                                          flatIt->note : possibleNotes[0].note);
                }
            }
        }
        
        // Calculate cents information
        vector<string> centsInfo;
        for (size_t i = 0; i < decimalParts.size(); ++i) {
            if (decimalParts[i] > 0.0) {
                int cents = static_cast<int>(round(decimalParts[i] * 100.0));
                string finalNoteName = result[i];
                int finalCents = cents;
                
                // If cents > 50, we already rounded up, so adjust cents to be negative
                if (cents > 50) {
                    finalCents = cents - 100;
                }
                
                string centsStr = finalNoteName + " " + 
                                 (finalCents >= 0 ? "+" : "") + 
                                 to_string(finalCents) + " cents";
                centsInfo.push_back(centsStr);
            }
        }
        
        return NoteResult(result, centsInfo);
    }
    
    
    /**
     * @brief Converts a PositionVector to note names
     * 
     * @param pv PositionVector containing MIDI numbers
     * @param options Configuration options
     * @return NoteResult with note names and cents information
     */
    NoteResult positionVectorToNoteNames(const PositionVector& pv,
                                        const NoteMapperOptions& options) const {
        return midiNumbersToNoteNames(pv.getData(), options);
    }
    
    /**
     * @brief Prints test results for multiple test cases
     */
    void testMidiNumbersToNoteNames(const vector<vector<int>>& testCases) const {
        vector<NoteMapperOptions> optionsList = {
            NoteMapperOptions(true, true, 12),   // Sharps (Diatonic)
            NoteMapperOptions(false, true, 12),  // Flats (Diatonic)
            NoteMapperOptions(true, false, 12),  // Sharps (Non-diatonic)
            NoteMapperOptions(false, false, 12), // Flats (Non-diatonic)
        };
        
        vector<string> labels = {
            "Sharps (Diatonic scale)",
            "Flats (Diatonic scale)",
            "Sharps (Non diatonic scale)",
            "Flats (Non diatonic scale)"
        };
        
        for (size_t i = 0; i < testCases.size(); ++i) {
            cout << "\nTest Case #" << (i + 1) << ": Notes [";
            for (size_t j = 0; j < testCases[i].size(); ++j) {
                cout << testCases[i][j];
                if (j < testCases[i].size() - 1) cout << ", ";
            }
            cout << "]" << endl;
            
            for (size_t j = 0; j < optionsList.size(); ++j) {
                // Skip diatonic if not 7 notes
                if (optionsList[j].isDiatonicScale && testCases[i].size() != 7) {
                    continue;
                }
                
                NoteResult result = midiNumbersToNoteNames(testCases[i], optionsList[j]);
                
                cout << labels[j] << ":" << endl;
                cout << "  Notes: ";
                for (size_t k = 0; k < result.noteNames.size(); ++k) {
                    cout << result.noteNames[k];
                    if (k < result.noteNames.size() - 1) cout << " ";
                }
                cout << endl;
                
                if (!result.centsInfo.empty()) {
                    cout << "  Cents: ";
                    for (size_t k = 0; k < result.centsInfo.size(); ++k) {
                        cout << result.centsInfo[k];
                        if (k < result.centsInfo.size() - 1) cout << ", ";
                    }
                    cout << endl;
                }
            }
        }
    }
};

#endif // NOTENAMES_H
