#ifndef MUSICPP_CHORD_NAMES_H
#define MUSICPP_CHORD_NAMES_H 


/**
 * @file chord_names.h
 * @brief Chord name analysis and construction
 * @details Analyze MIDI note sets and build chord names from standard music-theory conventions.
 */

#include "utility.h"

#include <stdexcept>


namespace musicpp {

struct ChordAnalysis {
    int root = 0;
    std::string chordName;
    
    // Thirds
    bool hasThird = false;
    bool hasMajorThird = false;
    bool hasMinorThird = false;
    
    // Fifths
    bool hasFifth = false;
    bool hasPerfectFifth = false;
    bool hasAugmentedFifth = false;
    bool hasDiminishedFifth = false;
    
    bool hasCompleteTriad = false;
    
    // Sevenths
    bool hasSeventh = false;
    bool hasMinorSeventh = false;
    bool hasMajorSeventh = false;
    bool hasDiminishedSeventh = false;
    
    // Extensions
    bool hasNinth = false;
    bool hasFlatNinth = false;
    bool hasNaturalNinth = false;
    
    bool hasEleventh = false;
    bool hasNaturalEleventh = false;
    bool hasSharpEleventh = false;
    
    bool hasThirteenth = false;
    bool hasFlatThirteenth = false;
    bool hasNaturalThirteenth = false;
    
    // Added notes
    bool hasSecond = false;
    bool hasFlatSecond = false;
    bool hasNaturalSecond = false;
    
    bool hasFourth = false;
    bool hasNaturalFourth = false;
    bool hasSharpFourth = false;
    
    bool hasSixth = false;
    bool hasFlatSixth = false;
    bool hasNaturalSixth = false;
    
    std::vector<std::pair<int, std::string>> addedNotes;
};

inline std::string noteToString(int midi) {
    const std::string notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    const int pc = ((midi % 12) + 12) % 12;
    return notes[pc];
}

inline std::string intervalToString(int interval) {
    static const std::map<int, std::string> intervalNames = {
        {1, "b2"}, {2, "2"}, {3, "m3"}, {4, "M3"}, {5, "4"}, {6, "b5/#4"}, 
        {7, "5"}, {8, "b6"}, {9, "6"}, {10, "7"}, {11, "maj7"},
        {13, "b9"}, {14, "9"}, {17, "11"}, {18, "#11"}, {20, "b13"}, {21, "13"}
    };
    if (intervalNames.count(interval)) return intervalNames.at(interval);
    return std::to_string(interval);
}

inline ChordAnalysis analyzeChord(const std::vector<int>& midiNotes, int rootIndex) {
    if (midiNotes.empty() || rootIndex < 0 || rootIndex >= static_cast<int>(midiNotes.size())) {
        throw std::out_of_range("analyzeChord: midiNotes is empty or rootIndex is out of range");
    }
    ChordAnalysis analysis;
    analysis.root = midiNotes[rootIndex];
    
    std::multiset<int> intervals;
    std::set<int> usedIntervals;
    
    for (size_t i = 0; i < midiNotes.size(); i++) {
        if (static_cast<int>(i) != rootIndex) {
            int interval = midiNotes[i] - midiNotes[rootIndex];
            while (interval < 0) interval += 12;
            while (interval >= 24) interval -= 12;
            intervals.insert(interval);
        }
    }
    
    if (intervals.count(3) && !usedIntervals.count(3)) {
        analysis.hasThird = true;
        analysis.hasMinorThird = true;
        usedIntervals.insert(3);
    } else if (intervals.count(4) && !usedIntervals.count(4)) {
        analysis.hasThird = true;
        analysis.hasMajorThird = true;
        usedIntervals.insert(4);
    }
    

    if (intervals.count(7) && !usedIntervals.count(7)) {
        analysis.hasFifth = true;
        analysis.hasPerfectFifth = true;
        usedIntervals.insert(7);
    } else if (intervals.count(6) && !usedIntervals.count(6) && analysis.hasThird) {
        analysis.hasFifth = true;
        analysis.hasDiminishedFifth = true;
        usedIntervals.insert(6);
    } else if (intervals.count(8) && !usedIntervals.count(8) && analysis.hasMajorThird) {
        analysis.hasFifth = true;
        analysis.hasAugmentedFifth = true;
        usedIntervals.insert(8);
    }
    
    analysis.hasCompleteTriad = analysis.hasThird && analysis.hasFifth;
    

    if (intervals.count(11) && !usedIntervals.count(11)) {
        analysis.hasSeventh = true;
        analysis.hasMajorSeventh = true;
        usedIntervals.insert(11);
    } else if (intervals.count(10) && !usedIntervals.count(10)) {
        analysis.hasSeventh = true;
        analysis.hasMinorSeventh = true;
        usedIntervals.insert(10);
    } else if (intervals.count(9) && !usedIntervals.count(9) && analysis.hasMinorThird && analysis.hasDiminishedFifth) {
        analysis.hasSeventh = true;
        analysis.hasDiminishedSeventh = true;
        usedIntervals.insert(9);
    }
    
    if (intervals.count(13) && !usedIntervals.count(13)) {
        analysis.hasNinth = true;
        analysis.hasFlatNinth = true;
        usedIntervals.insert(13);
    } else if (intervals.count(14) && !usedIntervals.count(14)) {
        analysis.hasNinth = true;
        analysis.hasNaturalNinth = true;
        usedIntervals.insert(14);
    }
    
    if (intervals.count(17) && !usedIntervals.count(17)) {
        analysis.hasEleventh = true;
        analysis.hasNaturalEleventh = true;
        usedIntervals.insert(17);
    } else if (intervals.count(18) && !usedIntervals.count(18)) {
        analysis.hasEleventh = true;
        analysis.hasSharpEleventh = true;
        usedIntervals.insert(18);
    }
    
    if (intervals.count(20) && !usedIntervals.count(20)) {
        analysis.hasThirteenth = true;
        analysis.hasFlatThirteenth = true;
        usedIntervals.insert(20);
    } else if (intervals.count(21) && !usedIntervals.count(21)) {
        analysis.hasThirteenth = true;
        analysis.hasNaturalThirteenth = true;
        usedIntervals.insert(21);
    }
    
    if (intervals.count(1) && !usedIntervals.count(1)) {
        analysis.hasSecond = true;
        analysis.hasFlatSecond = true;
        usedIntervals.insert(1);
    } else if (intervals.count(2) && !usedIntervals.count(2)) {
        analysis.hasSecond = true;
        analysis.hasNaturalSecond = true;
        usedIntervals.insert(2);
    }
    
    if (intervals.count(5) && !usedIntervals.count(5)) {
        analysis.hasFourth = true;
        analysis.hasNaturalFourth = true;
        usedIntervals.insert(5);
    } else if (intervals.count(6) && !usedIntervals.count(6)) {
        analysis.hasFourth = true;
        analysis.hasSharpFourth = true;
        usedIntervals.insert(6);
    }
    
    if (intervals.count(8) && !usedIntervals.count(8)) {
        analysis.hasSixth = true;
        analysis.hasFlatSixth = true;
        usedIntervals.insert(8);
    }
    if (intervals.count(9) && !usedIntervals.count(9)) {
        analysis.hasSixth = true;
        analysis.hasNaturalSixth = true;
        usedIntervals.insert(9);
    }
    
    for (int interval : intervals) {
        if (usedIntervals.count(interval) == 0) {
            analysis.addedNotes.push_back({interval, intervalToString(interval)});
            usedIntervals.insert(interval);
        }
    }
    
    return analysis;
}

inline std::string buildChordName(const ChordAnalysis& analysis) {
    std::string name = noteToString(analysis.root);
    bool omitFifth = false;
    bool omitThird = false;
    
    if (!analysis.hasThird && analysis.hasNaturalFourth && analysis.hasPerfectFifth) {
        if (analysis.hasMinorSeventh) {
            name += "7sus4";
        } else if (analysis.hasMajorSeventh) {
            name += "maj7sus4";
        } else {
            name += "sus4";
        }
    }
    else if (!analysis.hasThird && analysis.hasNaturalSecond && analysis.hasPerfectFifth) {
        if (analysis.hasMinorSeventh) {
            name += "7sus2";
        } else if (analysis.hasMajorSeventh) {
            name += "maj7sus2";
        } else {
            name += "sus2";
        }
    }
    else if (!analysis.hasThird && analysis.hasNaturalFourth && analysis.hasMinorSeventh) {
        name += "7sus4";
        omitFifth = !analysis.hasFifth;
    }
    else if (!analysis.hasThird && analysis.hasDiminishedFifth && analysis.hasMinorSeventh) {
        name += "7 b5";
        omitThird = true;
    }
    else if (!analysis.hasThird && analysis.hasPerfectFifth && analysis.hasMinorSeventh && !analysis.hasNaturalFourth && !analysis.hasNaturalSecond) {
        name += "7";
        omitThird = true;
    }
    else if (analysis.hasMajorThird && analysis.hasMinorSeventh && !analysis.hasFifth) {
        name += "7";
        omitFifth = true;
    }
    else if (analysis.hasMinorThird && analysis.hasMinorSeventh && !analysis.hasFifth) {
        name += "min7";
        omitFifth = true;
    }
    else if (analysis.hasMajorThird && !analysis.hasFifth) {
        if (analysis.hasMajorSeventh) {
            name += "maj7";
        } else {
            name += "maj";
        }
        omitFifth = true;
    } else if (analysis.hasMinorThird && !analysis.hasFifth) {
        if (analysis.hasMajorSeventh) {
            name += "min/maj7";
        } else {
            name += "min";
        }
        omitFifth = true;
    } else if (analysis.hasMajorThird && analysis.hasPerfectFifth) {
        if (analysis.hasMinorSeventh) {
            name += "7";
        } else if (analysis.hasMajorSeventh) {
            name += "maj7";
        } else {
            name += "";
        }
    } else if (analysis.hasMajorThird && analysis.hasAugmentedFifth) {
        if (analysis.hasMajorSeventh) {
            name += "aug/maj7";
        } else if (analysis.hasMinorSeventh) {
            name += "aug7";
        } else {
            name += "aug";
        }
    } else if (analysis.hasMajorThird && analysis.hasDiminishedFifth) {
        name += "maj b5";
    } else if (analysis.hasMinorThird && analysis.hasDiminishedFifth) {
        if (analysis.hasMajorSeventh) {
            name += "dim/maj7";
        } else if (analysis.hasMinorSeventh) {
            name += "min7b5";
        } else if (analysis.hasDiminishedSeventh) {
            name += "dim7";
        } else {
            name += "dim";
        }
    } else if (analysis.hasMinorThird && analysis.hasPerfectFifth) {
        if (analysis.hasMajorSeventh) {
            name += "min/maj7";
        } else if (analysis.hasMinorSeventh) {
            name += "min7";
        } else {
            name += "min";
        }
    }
    
    bool hasSeventh = (name.find("7") != std::string::npos || name.find("maj7") != std::string::npos);
    
    if (hasSeventh) {
        if (analysis.hasNinth) {
            if (analysis.hasFlatNinth) {
                name += "b9";
                if (analysis.hasEleventh) {
                    if (analysis.hasNaturalEleventh) {
                        name += "/11";
                    } else if (analysis.hasSharpEleventh) {
                        name += " #11";
                    }
                    
                    if (analysis.hasThirteenth) {
                        if (analysis.hasFlatThirteenth) {
                            name += "/b13";
                        } else if (analysis.hasNaturalThirteenth) {
                            name += "/13";
                        }
                    }
                } else if (analysis.hasThirteenth) {
                    if (analysis.hasFlatThirteenth) {
                        name += "/b13";
                    } else if (analysis.hasNaturalThirteenth) {
                        name += "/13";
                    }
                }
            } else if (analysis.hasNaturalNinth) {
                size_t pos;
                
                pos = name.find("maj7");
                if (pos != std::string::npos) {
                    name.replace(pos, 4, "9");
                }
                else if ((pos = name.find("aug7")) != std::string::npos) {
                    name.replace(pos + 3, 1, "9");
                }
                else if ((pos = name.find("min7b5")) != std::string::npos) {
                    // For min7b5, add 9 as extension
                    name += " 9";
                }
                else if ((pos = name.find("min7")) != std::string::npos) {
                    name.replace(pos + 3, 1, "9");
                }
                // FIXED: Handle dim7 with natural 9th
                else if ((pos = name.find("dim7")) != std::string::npos) {
                    // For dim7, add 9 as an extension rather than replacing
                    name += " 9";
                }
                else if ((pos = name.find("7")) != std::string::npos) {
                    bool isStandalone = true;
                    if (pos > 0) {
                        std::string before = name.substr(
                            static_cast<size_t>(std::max(0, (int)pos - 3)),
                            std::min(static_cast<size_t>(4), static_cast<size_t>(pos + 1)));
                        if (before.find("maj") != std::string::npos || 
                            before.find("min") != std::string::npos || 
                            before.find("dim") != std::string::npos ||
                            before.find("aug") != std::string::npos) {
                            isStandalone = false;
                        }
                    }
                    if (isStandalone) {
                        name.replace(pos, 1, "9");
                    }
                }
                
                if (analysis.hasEleventh) {
                    if (analysis.hasNaturalEleventh) {
                        name += "/11";
                    } else if (analysis.hasSharpEleventh) {
                        name += "/#11";
                    }
                    
                    if (analysis.hasThirteenth) {
                        if (analysis.hasFlatThirteenth) {
                            name += "/b13";
                        } else if (analysis.hasNaturalThirteenth) {
                            name += "/13";
                        }
                    }
                }
            }
        }
        else if (analysis.hasEleventh || analysis.hasThirteenth) {
            if (analysis.hasEleventh) {
                if (analysis.hasNaturalEleventh) {
                    name += "/11";
                } else if (analysis.hasSharpEleventh) {
                    name += "/#11";
                }
            }
            
            if (analysis.hasThirteenth) {
                if (analysis.hasFlatThirteenth) {
                    name += "/b13";
                } else if (analysis.hasNaturalThirteenth) {
                    name += "/13";
                }
            }
        }
    }
    
    bool isAddedNote = false;
    if (analysis.hasNaturalSecond && name.find("sus2") == std::string::npos && name.find(" 9") == std::string::npos) {
        name += isAddedNote ? "" : " ";
        name += "2 ";
        isAddedNote = true;
    }
    if (analysis.hasFlatSecond) {
        name += isAddedNote ? "" : " ";
        name += "b2 ";
        isAddedNote = true;
    }
    if (analysis.hasNaturalFourth && name.find("sus4") == std::string::npos) {
        name += isAddedNote ? "" : " ";
        name += "4 ";
        isAddedNote = true;
    }
    if (analysis.hasSharpFourth) {
        name += isAddedNote ? "" : " ";
        name += "#4 ";
        isAddedNote = true;
    }
    if (analysis.hasNaturalSixth) {
        name += isAddedNote ? "" : " ";
        name += "6 ";
        isAddedNote = true;
    }
    if (analysis.hasFlatSixth) {
        name += isAddedNote ? "" : " ";
        name += "b6 ";
        isAddedNote = true;
    }
    
    for (const auto& note : analysis.addedNotes) {
        name += " " + note.second;
    }
    
    if (omitFifth) {
        name += " (omit 5)";
    }
    if (omitThird) {
        name += " (omit 3)";
    }
    
    return name;
}

} // namespace musicpp

#endif // MUSICPP_CHORD_NAMES_H
