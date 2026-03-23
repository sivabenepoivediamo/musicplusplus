import { describe, expect, it } from "vitest";

import {
  align,
  analyzeChord,
  autoScale,
  buildChordName,
  calculateDistances,
  chord,
  containsAllNotesModulo,
  degreeAutomationReference,
  degreeAutomationSequentialBackward,
  filterModalMatrix,
  filterTranspositionMatrix,
  forwardDegreeAutomation,
  forwardVoiceLeading,
  getByComplexity,
  IntervalVector,
  modalMatrix,
  modalRototranslation,
  modalSelection,
  NoteNamingSystem,
  PositionVector,
  rototranslationMatrix,
  transpositionMatrix,
  voiceLeadingAutomationReference,
  voiceLeadingAutomationSequentialBackward
} from "../src";

describe("musicplusplus example coverage", function () {
  it("matches the autoscale example scenarios", function () {
    const cases = [
      { notes: [61, 70], scale: [0, 2, 4, 5, 7, 9, 11], expected: [1, 2, 4, 5, 7, 9, 10] },
      { notes: [61, 65], scale: [0, 2, 4, 7, 9], expected: [1, 2, 5, 7, 9] },
      { notes: [63, 70], scale: [7, 9, 11, 12, 14, 16, 18], expected: [7, 10, 11, 12, 14, 15, 18] },
      { notes: [61, 63], scale: [0, 2, 4, 6, 8, 10], expected: [1, 3, 4, 6, 8, 10] },
      { notes: [63, 66], scale: [0, 2, 4, 5, 7, 9, 11], expected: [0, 2, 3, 5, 6, 9, 11] },
      { notes: [67], scale: [1, 3, 5, 6, 8, 10, 12], expected: [1, 3, 5, 7, 8, 10, 12] },
      { notes: [70], scale: [0, 2, 4, 5, 7, 9, 11], expected: [0, 2, 4, 5, 7, 9, 10] }
    ];

    expect(cases.map(function (testCase) {
      return autoScale(new PositionVector(testCase.scale, { mod: 12 }), testCase.notes).toArray();
    })).toEqual(cases.map(function (testCase) {
      return testCase.expected;
    }));
  });

  it("matches the note naming examples", function () {
    const system = new NoteNamingSystem();

    expect(system.positionVectorToNoteNames(new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 }), {
      isDiatonicScale: true,
      moduloValue: 12,
      preferSharps: true
    })).toEqual({ centsInfo: [], noteNames: ["C", "D", "E", "F", "G", "A", "B"] });

    expect(system.positionVectorToNoteNames(new PositionVector([5, 7, 9, 10, 0, 2, 4], { mod: 12 }), {
      isDiatonicScale: true,
      moduloValue: 12,
      preferSharps: false
    })).toEqual({ centsInfo: [], noteNames: ["F", "G", "A", "B\u266d", "C", "D", "E"] });

    expect(system.positionVectorToNoteNames(new PositionVector([7, 11, 2, 5], { mod: 12 }), {
      isDiatonicScale: false,
      moduloValue: 12,
      preferSharps: true
    })).toEqual({ centsInfo: [], noteNames: ["G", "B", "D", "F"] });

    expect(system.positionVectorToNoteNames(
      new PositionVector([0, 5, 10, 13, 18, 23, 28], { mod: 31 }).rotoTranslate(2),
      { isDiatonicScale: false, moduloValue: 31, preferSharps: false }
    )).toEqual({
      centsInfo: ["E -13 cents", "F +3 cents", "G -3 cents", "A -10 cents", "B -16 cents", "D -6 cents"],
      noteNames: ["E", "F", "G", "A", "B", "C", "D"]
    });
  });

  it("matches representative chord naming examples", function () {
    const cases = [
      { notes: [60, 64, 67], rootIndex: 0, name: "C" },
      { notes: [60, 63, 67], rootIndex: 0, name: "Cmin" },
      { notes: [60, 64, 68], rootIndex: 0, name: "Caug" },
      { notes: [60, 63, 66], rootIndex: 0, name: "Cdim" },
      { notes: [60, 64, 67, 71], rootIndex: 0, name: "Cmaj7" },
      { notes: [60, 64, 67, 70], rootIndex: 0, name: "C7" },
      { notes: [60, 63, 67, 70], rootIndex: 0, name: "Cmin7" },
      { notes: [60, 63, 66, 69], rootIndex: 0, name: "Cdim7" },
      { notes: [60, 63, 66, 70], rootIndex: 0, name: "Cmin7b5" },
      { notes: [60, 65, 67], rootIndex: 0, name: "Csus4" },
      { notes: [60, 62, 67], rootIndex: 0, name: "Csus2" },
      { notes: [60, 65, 67, 70], rootIndex: 0, name: "C7sus4" }
    ];

    expect(cases.map(function (testCase) {
      return buildChordName(analyzeChord(testCase.notes, testCase.rootIndex));
    })).toEqual(cases.map(function (testCase) {
      return testCase.name;
    }));
  });

  it("matches the matrix distance example best rows", function () {
    const cMajor = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const cLydian = new PositionVector([0, 2, 4, 6, 7, 9, 11], { mod: 12 });
    const chordA = new PositionVector([0, 4, 7], { mod: 12 });
    const chordB = new PositionVector([7, 11, 14], { mod: 12 });
    const criterion = new IntervalVector([2, 2, 3], { mod: 12, offset: 0 });

    const transpositionBest = getByComplexity(calculateDistances(cLydian, transpositionMatrix(cMajor)), 0);
    const modeBest = getByComplexity(calculateDistances(cLydian, modalMatrix(cMajor)), 0);
    const rototranslationBest = getByComplexity(calculateDistances(chordA, rototranslationMatrix(chordB, align(chordA, chordB))), 0);
    const modalSelectionBest = getByComplexity(calculateDistances(chordB, modalSelection(cMajor, criterion, 0)), 0);
    const modalRototranslationBest = getByComplexity(
      calculateDistances(chordB, modalRototranslation(modalSelection(cMajor, criterion, 0) as readonly { readonly modeIndex: number; readonly vector: PositionVector }[])),
      0
    );

    expect({
      distance: transpositionBest.distance,
      transposition: transpositionBest.transposition,
      vector: transpositionBest.vector.toJSON()
    }).toEqual({
      distance: 0,
      transposition: 7,
      vector: new PositionVector([0, 2, 4, 6, 7, 9, 11], { mod: 12 }).toJSON()
    });

    expect({
      distance: modeBest.distance,
      index: modeBest.index,
      vector: modeBest.vector.toJSON()
    }).toEqual({
      distance: 0,
      index: 3,
      vector: new PositionVector([0, 2, 4, 6, 7, 9, 11], { mod: 12 }).toJSON()
    });

    expect({
      center: rototranslationBest.center,
      distance: rototranslationBest.distance,
      translation: rototranslationBest.translation,
      vector: rototranslationBest.vector.toJSON()
    }).toEqual({
      center: -2,
      distance: 3,
      translation: -2,
      vector: new PositionVector([-1, 2, 7], { mod: 12 }).toJSON()
    });

    expect({
      distance: modalSelectionBest.distance,
      modeIndex: modalSelectionBest.modeIndex,
      vector: modalSelectionBest.vector.toJSON()
    }).toEqual({
      distance: 18,
      modeIndex: 3,
      vector: new PositionVector([0, 5, 9], { mod: 12 }).toJSON()
    });

    expect({
      distance: modalRototranslationBest.distance,
      modeIndex: modalRototranslationBest.modeIndex,
      translationIndex: modalRototranslationBest.translationIndex,
      vector: modalRototranslationBest.vector.toJSON()
    }).toEqual({
      distance: 3,
      modeIndex: 0,
      translationIndex: 2,
      vector: new PositionVector([7, 12, 16], { mod: 12 }).toJSON()
    });
  });

  it("matches the matrix filtering example behavior", function () {
    const cMajor = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const filteredModes = filterModalMatrix(modalMatrix(cMajor) as readonly { readonly index: number; readonly vector: PositionVector }[], [63]);
    const filteredTranspositions = filterTranspositionMatrix(transpositionMatrix(cMajor), [63]);

    expect(filteredModes.map(function (row) {
      return row.index;
    })).toEqual([1, 2, 5, 6]);
    expect(filteredTranspositions.map(function (row) {
      return row.transposition;
    })).toEqual([1, 3, 4, 6, 8, 10, 11]);
    expect(filteredModes.every(function (row) {
      return containsAllNotesModulo(row.vector, [63]);
    })).toBe(true);
    expect(filteredTranspositions.every(function (row) {
      return containsAllNotesModulo(row.vector, [63]);
    })).toBe(true);
  });

  it("matches the sequential automation examples", function () {
    const scale = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const criterion = new IntervalVector([2, 2, 2, 2], { mod: 12, offset: 0 });
    const I = chord(scale, criterion, { shift: 0, preVoices: 4 }).toPositions();
    const II = chord(scale, criterion, { shift: 1, preVoices: 4 }).toPositions();
    const IV = chord(scale, criterion, { shift: 3, preVoices: 4 }).toPositions();
    const V = chord(scale, criterion, { shift: 4, preVoices: 4 }).toPositions();
    const VI = chord(scale, criterion, { shift: 5, preVoices: 4 }).toPositions();
    const VII = chord(scale, criterion, { shift: 6, preVoices: 4 }).toPositions();
    const IV6 = chord(scale, criterion, { shift: 1, preVoices: 4, position: 1 }).toPositions();
    const chords = [I, II, V, I, VI, IV, II, VII, I, V, VI, II, V, I];
    const degrees = [0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0];
    const octaveRule = [0, 5, 20, 7, 20, 10, 15, 0, 5, 15, 7, 20, 0, 15, 7];

    expect(forwardVoiceLeading(chords, [0]).map(function (vector) {
      return vector.toArray();
    })).toEqual([
      [0, 4, 7, 11], [0, 2, 5, 9], [-1, 2, 5, 7], [-1, 0, 4, 7], [-3, 0, 4, 7], [-3, 0, 4, 5], [-3, 0, 2, 5],
      [-3, -1, 2, 5], [-5, -1, 0, 4], [-5, -1, 2, 5], [-5, -3, 0, 4], [-7, -3, 0, 2], [-7, -5, -1, 2], [-8, -5, -1, 0]
    ]);

    expect(voiceLeadingAutomationSequentialBackward(chords, [0]).map(function (vector) {
      return vector.toArray();
    })).toEqual([
      [7, 11, 12, 16], [9, 12, 14, 17], [7, 11, 14, 17], [7, 11, 12, 16], [7, 9, 12, 16], [5, 9, 12, 16], [5, 9, 12, 14],
      [5, 9, 11, 14], [4, 7, 11, 12], [5, 7, 11, 14], [4, 7, 9, 12], [2, 5, 9, 12], [2, 5, 7, 11], [0, 4, 7, 11]
    ]);

    expect(voiceLeadingAutomationReference(chords, IV6, [0]).map(function (vector) {
      return vector.toArray();
    })).toEqual([
      [4, 7, 11, 12], [5, 9, 12, 14], [5, 7, 11, 14], [4, 7, 11, 12], [7, 9, 12, 16], [5, 9, 12, 16], [5, 9, 12, 14],
      [5, 9, 11, 14], [4, 7, 11, 12], [5, 7, 11, 14], [7, 9, 12, 16], [5, 9, 12, 14], [5, 7, 11, 14], [4, 7, 11, 12]
    ]);

    expect(forwardDegreeAutomation(scale, criterion, degrees, I, octaveRule).map(function (vector) {
      return vector.toArray();
    })).toEqual([
      [0, 4, 7, 11], [2, 5, 9, 12], [4, 7, 11, 14], [4, 5, 9, 12], [5, 7, 11, 14], [4, 7, 9, 12], [5, 9, 11, 14], [4, 7, 11, 12],
      [5, 9, 11, 14], [4, 7, 9, 12], [5, 7, 11, 14], [4, 5, 9, 12], [2, 4, 7, 11], [0, 2, 5, 9], [-1, 0, 4, 7]
    ]);

    expect(degreeAutomationSequentialBackward(scale, criterion, degrees, I, [0]).map(function (vector) {
      return vector.toArray();
    })).toEqual([
      [4, 7, 11, 12], [5, 9, 12, 14], [4, 7, 11, 14], [5, 9, 12, 16], [5, 7, 11, 14], [4, 7, 9, 12], [2, 5, 9, 11], [0, 4, 7, 11],
      [-1, 2, 5, 9], [-3, 0, 4, 7], [-5, -1, 2, 5], [-3, 0, 4, 5], [-1, 2, 4, 7], [0, 2, 5, 9], [0, 4, 7, 11]
    ]);

    expect(degreeAutomationReference(scale, criterion, degrees, I, [0]).map(function (vector) {
      return vector.toArray();
    })).toEqual([
      [0, 4, 7, 11], [0, 2, 5, 9], [2, 4, 7, 11], [0, 4, 5, 9], [2, 5, 7, 11], [0, 4, 7, 9], [2, 5, 9, 11], [0, 4, 7, 11],
      [2, 5, 9, 11], [0, 4, 7, 9], [2, 5, 7, 11], [0, 4, 5, 9], [2, 4, 7, 11], [0, 2, 5, 9], [0, 4, 7, 11]
    ]);
  });
});
