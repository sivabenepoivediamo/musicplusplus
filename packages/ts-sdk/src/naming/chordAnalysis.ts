import type { ChordAnalysis } from "./types";

function noteToString(midi: number): string {
  var notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
  return notes[((midi % 12) + 12) % 12];
}

function intervalToString(interval: number): string {
  var intervalNames: Record<number, string> = {
    1: "b2",
    2: "2",
    3: "m3",
    4: "M3",
    5: "4",
    6: "b5/#4",
    7: "5",
    8: "b6",
    9: "6",
    10: "7",
    11: "maj7",
    13: "b9",
    14: "9",
    17: "11",
    18: "#11",
    20: "b13",
    21: "13"
  };
  return intervalNames[interval] || String(interval);
}

function createEmptyChordAnalysis(root: number): ChordAnalysis {
  return {
    addedNotes: [],
    chordName: "",
    hasAugmentedFifth: false,
    hasCompleteTriad: false,
    hasDiminishedFifth: false,
    hasDiminishedSeventh: false,
    hasEleventh: false,
    hasFifth: false,
    hasFlatNinth: false,
    hasFlatSecond: false,
    hasFlatSixth: false,
    hasFlatThirteenth: false,
    hasFourth: false,
    hasMajorSeventh: false,
    hasMajorThird: false,
    hasMinorSeventh: false,
    hasMinorThird: false,
    hasNaturalEleventh: false,
    hasNaturalFourth: false,
    hasNaturalNinth: false,
    hasNaturalSecond: false,
    hasNaturalSixth: false,
    hasNaturalThirteenth: false,
    hasNinth: false,
    hasPerfectFifth: false,
    hasSecond: false,
    hasSeventh: false,
    hasSharpEleventh: false,
    hasSharpFourth: false,
    hasSixth: false,
    hasThird: false,
    hasThirteenth: false,
    root: root
  };
}

export function analyzeChord(midiNotes: readonly number[], rootIndex: number = 0): ChordAnalysis {
  var analysis = createEmptyChordAnalysis(midiNotes[rootIndex] || 0);
  var intervals: number[] = [];
  var usedIntervals: number[] = [];
  var index = 0;

  for (index = 0; index < midiNotes.length; index += 1) {
    if (index !== rootIndex) {
      var interval = midiNotes[index] - midiNotes[rootIndex];
      while (interval < 0) {
        interval += 12;
      }
      while (interval >= 24) {
        interval -= 12;
      }
      intervals.push(interval);
    }
  }

  if (intervals.indexOf(3) !== -1 && usedIntervals.indexOf(3) === -1) {
    analysis.hasThird = true;
    analysis.hasMinorThird = true;
    usedIntervals.push(3);
  } else if (intervals.indexOf(4) !== -1 && usedIntervals.indexOf(4) === -1) {
    analysis.hasThird = true;
    analysis.hasMajorThird = true;
    usedIntervals.push(4);
  }

  if (intervals.indexOf(7) !== -1 && usedIntervals.indexOf(7) === -1) {
    analysis.hasFifth = true;
    analysis.hasPerfectFifth = true;
    usedIntervals.push(7);
  } else if (intervals.indexOf(6) !== -1 && usedIntervals.indexOf(6) === -1 && analysis.hasThird) {
    analysis.hasFifth = true;
    analysis.hasDiminishedFifth = true;
    usedIntervals.push(6);
  } else if (intervals.indexOf(8) !== -1 && usedIntervals.indexOf(8) === -1 && analysis.hasMajorThird) {
    analysis.hasFifth = true;
    analysis.hasAugmentedFifth = true;
    usedIntervals.push(8);
  }

  analysis.hasCompleteTriad = analysis.hasThird && analysis.hasFifth;

  if (intervals.indexOf(11) !== -1 && usedIntervals.indexOf(11) === -1) {
    analysis.hasSeventh = true;
    analysis.hasMajorSeventh = true;
    usedIntervals.push(11);
  } else if (intervals.indexOf(10) !== -1 && usedIntervals.indexOf(10) === -1) {
    analysis.hasSeventh = true;
    analysis.hasMinorSeventh = true;
    usedIntervals.push(10);
  } else if (intervals.indexOf(9) !== -1 && usedIntervals.indexOf(9) === -1 && analysis.hasMinorThird && analysis.hasDiminishedFifth) {
    analysis.hasSeventh = true;
    analysis.hasDiminishedSeventh = true;
    usedIntervals.push(9);
  }

  if (intervals.indexOf(13) !== -1 && usedIntervals.indexOf(13) === -1) {
    analysis.hasNinth = true;
    analysis.hasFlatNinth = true;
    usedIntervals.push(13);
  } else if (intervals.indexOf(14) !== -1 && usedIntervals.indexOf(14) === -1) {
    analysis.hasNinth = true;
    analysis.hasNaturalNinth = true;
    usedIntervals.push(14);
  }

  if (intervals.indexOf(17) !== -1 && usedIntervals.indexOf(17) === -1) {
    analysis.hasEleventh = true;
    analysis.hasNaturalEleventh = true;
    usedIntervals.push(17);
  } else if (intervals.indexOf(18) !== -1 && usedIntervals.indexOf(18) === -1) {
    analysis.hasEleventh = true;
    analysis.hasSharpEleventh = true;
    usedIntervals.push(18);
  }

  if (intervals.indexOf(20) !== -1 && usedIntervals.indexOf(20) === -1) {
    analysis.hasThirteenth = true;
    analysis.hasFlatThirteenth = true;
    usedIntervals.push(20);
  } else if (intervals.indexOf(21) !== -1 && usedIntervals.indexOf(21) === -1) {
    analysis.hasThirteenth = true;
    analysis.hasNaturalThirteenth = true;
    usedIntervals.push(21);
  }

  if (intervals.indexOf(1) !== -1 && usedIntervals.indexOf(1) === -1) {
    analysis.hasSecond = true;
    analysis.hasFlatSecond = true;
    usedIntervals.push(1);
  } else if (intervals.indexOf(2) !== -1 && usedIntervals.indexOf(2) === -1) {
    analysis.hasSecond = true;
    analysis.hasNaturalSecond = true;
    usedIntervals.push(2);
  }

  if (intervals.indexOf(5) !== -1 && usedIntervals.indexOf(5) === -1) {
    analysis.hasFourth = true;
    analysis.hasNaturalFourth = true;
    usedIntervals.push(5);
  } else if (intervals.indexOf(6) !== -1 && usedIntervals.indexOf(6) === -1) {
    analysis.hasFourth = true;
    analysis.hasSharpFourth = true;
    usedIntervals.push(6);
  }

  if (intervals.indexOf(8) !== -1 && usedIntervals.indexOf(8) === -1) {
    analysis.hasSixth = true;
    analysis.hasFlatSixth = true;
    usedIntervals.push(8);
  }
  if (intervals.indexOf(9) !== -1 && usedIntervals.indexOf(9) === -1) {
    analysis.hasSixth = true;
    analysis.hasNaturalSixth = true;
    usedIntervals.push(9);
  }

  for (index = 0; index < intervals.length; index += 1) {
    if (usedIntervals.indexOf(intervals[index]) === -1) {
      analysis.addedNotes.push([intervals[index], intervalToString(intervals[index])]);
      usedIntervals.push(intervals[index]);
    }
  }

  return analysis;
}

export function buildChordName(analysis: ChordAnalysis): string {
  var name = noteToString(analysis.root);
  var omitFifth = false;
  var omitThird = false;

  if (!analysis.hasThird && analysis.hasNaturalFourth && analysis.hasPerfectFifth) {
    if (analysis.hasMinorSeventh) {
      name += "7sus4";
    } else if (analysis.hasMajorSeventh) {
      name += "maj7sus4";
    } else {
      name += "sus4";
    }
  } else if (!analysis.hasThird && analysis.hasNaturalSecond && analysis.hasPerfectFifth) {
    if (analysis.hasMinorSeventh) {
      name += "7sus2";
    } else if (analysis.hasMajorSeventh) {
      name += "maj7sus2";
    } else {
      name += "sus2";
    }
  } else if (!analysis.hasThird && analysis.hasNaturalFourth && analysis.hasMinorSeventh) {
    name += "7sus4";
    omitFifth = !analysis.hasFifth;
  } else if (!analysis.hasThird && analysis.hasDiminishedFifth && analysis.hasMinorSeventh) {
    name += "7 b5";
    omitThird = true;
  } else if (!analysis.hasThird && analysis.hasPerfectFifth && analysis.hasMinorSeventh && !analysis.hasNaturalFourth && !analysis.hasNaturalSecond) {
    name += "7";
    omitThird = true;
  } else if (analysis.hasMajorThird && analysis.hasMinorSeventh && !analysis.hasFifth) {
    name += "7";
    omitFifth = true;
  } else if (analysis.hasMinorThird && analysis.hasMinorSeventh && !analysis.hasFifth) {
    name += "min7";
    omitFifth = true;
  } else if (analysis.hasMajorThird && !analysis.hasFifth) {
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

  var hasSeventh = name.indexOf("7") !== -1 || name.indexOf("maj7") !== -1;
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
        var position = name.indexOf("maj7");
        if (position !== -1) {
          name = name.slice(0, position) + "9" + name.slice(position + 4);
        } else if (name.indexOf("aug7") !== -1) {
          position = name.indexOf("aug7");
          name = name.slice(0, position + 3) + "9" + name.slice(position + 4);
        } else if (name.indexOf("min7b5") !== -1) {
          name += " 9";
        } else if (name.indexOf("min7") !== -1) {
          position = name.indexOf("min7");
          name = name.slice(0, position + 3) + "9" + name.slice(position + 4);
        } else if (name.indexOf("dim7") !== -1) {
          name += " 9";
        } else if (name.indexOf("7") !== -1) {
          position = name.indexOf("7");
          var isStandalone = true;
          if (position > 0) {
            var before = name.slice(Math.max(0, position - 3), position + 1);
            if (before.indexOf("maj") !== -1 || before.indexOf("min") !== -1 || before.indexOf("dim") !== -1 || before.indexOf("aug") !== -1) {
              isStandalone = false;
            }
          }
          if (isStandalone) {
            name = name.slice(0, position) + "9" + name.slice(position + 1);
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
    } else if (analysis.hasEleventh || analysis.hasThirteenth) {
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

  var isAddedNote = false;
  if (analysis.hasNaturalSecond && name.indexOf("sus2") === -1 && name.indexOf(" 9") === -1) {
    name += isAddedNote ? "" : " ";
    name += "2 ";
    isAddedNote = true;
  }
  if (analysis.hasFlatSecond) {
    name += isAddedNote ? "" : " ";
    name += "b2 ";
    isAddedNote = true;
  }
  if (analysis.hasNaturalFourth && name.indexOf("sus4") === -1) {
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

  for (var noteIndex = 0; noteIndex < analysis.addedNotes.length; noteIndex += 1) {
    name += " " + analysis.addedNotes[noteIndex][1];
  }

  if (omitFifth) {
    name += " (omit 5)";
  }
  if (omitThird) {
    name += " (omit 3)";
  }

  return name;
}
