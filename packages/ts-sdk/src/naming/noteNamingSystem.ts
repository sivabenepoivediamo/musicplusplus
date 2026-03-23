import { PositionVector } from "../core";
import type { ClassifiedNote, NoteMapperOptions, NoteResult } from "./types";

export class NoteNamingSystem {
  private readonly noteArrays: readonly (readonly string[])[] = [
    ["C", "B♯", "D♭♭"],
    ["C♯", "D♭"],
    ["D", "C♯♯", "E♭♭"],
    ["D♯", "E♭"],
    ["E", "D♯♯", "F♭"],
    ["F", "E♯", "G♭♭"],
    ["F♯", "G♭"],
    ["G", "F♯♯", "A♭♭"],
    ["G♯", "A♭"],
    ["A", "G♯♯", "B♭♭"],
    ["A♯", "B♭"],
    ["B", "A♯♯", "C♭"]
  ];
  private readonly noteOrder: readonly string[] = ["A", "B", "C", "D", "E", "F", "G"];
  private readonly classifiedNotes: readonly (readonly ClassifiedNote[])[];

  public constructor() {
    this.classifiedNotes = this.classifyNotes();
  }

  private classifyNotes(): readonly (readonly ClassifiedNote[])[] {
    var result: ClassifiedNote[][] = [];

    for (var groupIndex = 0; groupIndex < this.noteArrays.length; groupIndex += 1) {
      var group = this.noteArrays[groupIndex];
      var classified: ClassifiedNote[] = [];
      if (group.length === 3) {
        classified.push({ label: "natural", note: group[0] });
        classified.push({ label: "right", note: group[1] });
        classified.push({ label: "left", note: group[2] });
      } else if (group.length === 2) {
        classified.push({ label: "right", note: group[0] });
        classified.push({ label: "left", note: group[1] });
      } else {
        for (var noteIndex = 0; noteIndex < group.length; noteIndex += 1) {
          classified.push({ label: "none", note: group[noteIndex] });
        }
      }
      result.push(classified);
    }

    return result;
  }

  private getBasicNoteName(noteName: string): string {
    return noteName.length === 0 ? "" : noteName.charAt(0);
  }

  private areNotesConsecutive(noteNames: readonly string[]): boolean {
    if (noteNames.length === 0) {
      return true;
    }

    var basicNotes: string[] = [];
    for (var index = 0; index < noteNames.length; index += 1) {
      basicNotes.push(this.getBasicNoteName(noteNames[index]));
    }

    for (var start = 0; start < this.noteOrder.length; start += 1) {
      var isValid = true;
      for (index = 0; index < basicNotes.length; index += 1) {
        if (basicNotes[index] !== this.noteOrder[(start + index) % this.noteOrder.length]) {
          isValid = false;
          break;
        }
      }
      if (isValid) {
        return true;
      }
    }

    return false;
  }

  private hasDoubleAccidentals(noteNames: readonly string[]): boolean {
    for (var index = 0; index < noteNames.length; index += 1) {
      if (
        noteNames[index].indexOf("♯♯") !== -1 ||
        noteNames[index].indexOf("##") !== -1 ||
        noteNames[index].indexOf("♭♭") !== -1 ||
        noteNames[index].indexOf("bb") !== -1
      ) {
        return true;
      }
    }
    return false;
  }

  private scoreConfiguration(noteNames: readonly string[], preferSharps: boolean): number {
    var score = 0;
    for (var index = 0; index < noteNames.length; index += 1) {
      if (noteNames[index].indexOf("♯") !== -1 || noteNames[index].indexOf("#") !== -1) {
        score += preferSharps ? 10 : -10;
      } else if (noteNames[index].indexOf("♭") !== -1 || noteNames[index].indexOf("b") !== -1) {
        score += preferSharps ? -10 : 10;
      } else {
        score += 5;
      }
    }
    return score;
  }

  private findConsecutiveConfiguration(noteIndices: readonly number[], preferSharps: boolean): string[] {
    if (noteIndices.length !== 7) {
      return [];
    }

    var validConfigurations: string[][] = [];
    var startIndex = 0;

    for (startIndex = 0; startIndex < 7; startIndex += 1) {
      var candidate: string[] = [];
      var isValid = true;

      for (var scaleIndex = 0; scaleIndex < 7; scaleIndex += 1) {
        var requiredLetter = this.noteOrder[(startIndex + scaleIndex) % 7];
        var notesForIndex = this.noteArrays[noteIndices[scaleIndex]];
        var foundNote = "";

        for (var noteOption = 0; noteOption < notesForIndex.length; noteOption += 1) {
          if (this.getBasicNoteName(notesForIndex[noteOption]) === requiredLetter) {
            foundNote = notesForIndex[noteOption];
            break;
          }
        }

        if (foundNote.length === 0) {
          isValid = false;
          break;
        }

        candidate.push(foundNote);
      }

      if (isValid && this.areNotesConsecutive(candidate) && !this.hasDoubleAccidentals(candidate)) {
        validConfigurations.push(candidate);
      }
    }

    if (validConfigurations.length === 0) {
      return [];
    }

    var bestScore = Number.NEGATIVE_INFINITY;
    var bestConfiguration: string[] = [];
    for (startIndex = 0; startIndex < validConfigurations.length; startIndex += 1) {
      var score = this.scoreConfiguration(validConfigurations[startIndex], preferSharps);
      if (score > bestScore) {
        bestScore = score;
        bestConfiguration = validConfigurations[startIndex];
      }
    }

    return bestConfiguration;
  }

  private processMidiNumbers(midiNumbers: readonly number[], moduloValue: number): Array<[number, number]> {
    var processed: Array<[number, number]> = [];

    for (var index = 0; index < midiNumbers.length; index += 1) {
      var adjusted = midiNumbers[index];
      if (moduloValue > 0 && moduloValue !== 12) {
        var modResult = ((midiNumbers[index] % moduloValue) + moduloValue) % moduloValue;
        adjusted = Math.round((modResult * (12 / moduloValue)) * 100) / 100;
      }

      var integerPart = Math.floor(adjusted);
      var decimalPart = Math.round((adjusted - integerPart) * 100) / 100;
      processed.push([integerPart, decimalPart]);
    }

    return processed;
  }

  public midiNumbersToNoteNames(midiNumbers: readonly number[], options?: NoteMapperOptions): NoteResult {
    if (midiNumbers.length === 0) {
      return {
        centsInfo: [],
        noteNames: []
      };
    }

    var resolvedOptions = {
      isDiatonicScale: options && typeof options.isDiatonicScale === "boolean" ? options.isDiatonicScale : false,
      moduloValue: options && typeof options.moduloValue === "number" ? options.moduloValue : 12,
      preferSharps: options && typeof options.preferSharps === "boolean" ? options.preferSharps : true
    };

    var processed = this.processMidiNumbers(midiNumbers, resolvedOptions.moduloValue);
    var integerParts: number[] = [];
    var decimalParts: number[] = [];
    var noteIndices: number[] = [];
    var index = 0;

    for (index = 0; index < processed.length; index += 1) {
      integerParts.push(processed[index][0]);
      decimalParts.push(processed[index][1]);
    }

    for (index = 0; index < integerParts.length; index += 1) {
      var noteValue = integerParts[index];
      if (decimalParts[index] > 0.5) {
        noteValue += 1;
      }
      noteIndices.push(((noteValue % 12) + 12) % 12);
    }

    var result: string[] = [];
    var isDiatonic = resolvedOptions.isDiatonicScale && noteIndices.length === 7;

    if (isDiatonic) {
      result = this.findConsecutiveConfiguration(noteIndices, resolvedOptions.preferSharps);
      if (result.length === 0) {
        isDiatonic = false;
      }
    }

    if (result.length === 0) {
      for (index = 0; index < noteIndices.length; index += 1) {
        var possibleNotes = this.classifiedNotes[noteIndices[index]];
        var noteOption = 0;
        var naturalChoice = "";

        for (noteOption = 0; noteOption < possibleNotes.length; noteOption += 1) {
          if (possibleNotes[noteOption].label === "natural") {
            naturalChoice = possibleNotes[noteOption].note;
            break;
          }
        }

        if (naturalChoice.length > 0) {
          result.push(naturalChoice);
          continue;
        }

        var chosen = possibleNotes[0].note;
        if (resolvedOptions.preferSharps) {
          for (noteOption = 0; noteOption < possibleNotes.length; noteOption += 1) {
            if (possibleNotes[noteOption].label === "right") {
              chosen = possibleNotes[noteOption].note;
              break;
            }
          }
        } else {
          for (noteOption = 0; noteOption < possibleNotes.length; noteOption += 1) {
            if (possibleNotes[noteOption].label === "left") {
              chosen = possibleNotes[noteOption].note;
              break;
            }
          }
        }

        result.push(chosen);
      }
    }

    var centsInfo: string[] = [];
    for (index = 0; index < decimalParts.length; index += 1) {
      if (decimalParts[index] > 0) {
        var cents = Math.round(decimalParts[index] * 100);
        var finalCents = cents > 50 ? cents - 100 : cents;
        centsInfo.push(result[index] + " " + (finalCents >= 0 ? "+" : "") + String(finalCents) + " cents");
      }
    }

    return {
      centsInfo: centsInfo,
      noteNames: result
    };
  }

  public positionVectorToNoteNames(positionVector: PositionVector, options?: NoteMapperOptions): NoteResult {
    return this.midiNumbersToNoteNames(positionVector.data, options);
  }
}
