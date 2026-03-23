import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import { dirname, resolve } from "node:path";
import { describe, expect, it } from "vitest";

import {
  applyTripleSelect,
  BinaryVector,
  chord,
  diminution,
  deepRhythm,
  degreeAutomation,
  euclidean,
  generate,
  hierarchy,
  infrapolation,
  infrapolationPermutation,
  infraInterpolation,
  infraInterUltrapolation,
  infraUltrapolation,
  interInfrapolation,
  interpolation,
  IntervalVector,
  intervals as slonimskyIntervals,
  isGenerated,
  measureSummary,
  minInterval,
  modalMatrix,
  modalRototranslation,
  modalSelection,
  NoteNamingSystem,
  normalizeNotes,
  noteFromPosition,
  ornamentLoop,
  PositionVector,
  positionsToBinary,
  positionsToIntervals,
  quantize,
  run,
  run2,
  rototranslationMatrix,
  Scale,
  ScaleDatabase,
  select,
  symmetricInterpolation,
  tihai,
  transposeNotes,
  transpositionMatrix,
  tripleSelect,
  ultrapolation,
  ultraInfraInterpolation,
  ultraInterpolation,
  interInfraInterpolation,
  interUltrapolation,
  analyzeChord,
  asymmetricInterpolation,
  buildChordName
} from "../src";

const currentDirectory = dirname(fileURLToPath(import.meta.url));
const specRunner = resolve(currentDirectory, "../../cpp-sdk/build/musicplusplus-spec");

function spec(command: string, args: Record<string, string>): any {
  const cliArgs: string[] = [command];
  const keys = Object.keys(args);
  for (let index = 0; index < keys.length; index += 1) {
    cliArgs.push(`--${keys[index]}`, args[keys[index]]);
  }
  const output = execFileSync(specRunner, cliArgs, { encoding: "utf8" }).trim();
  return output.startsWith("{") || output.startsWith("[") ? JSON.parse(output) : Number(output);
}

function normalizePosition(vector: PositionVector) {
  return vector.toJSON();
}

function normalizeInterval(vector: IntervalVector) {
  return vector.toJSON();
}

function normalizeBinary(vector: BinaryVector) {
  return vector.toJSON();
}

function normalizeModalMatrix(rows: readonly { readonly index: number; readonly vector: PositionVector | IntervalVector }[]) {
  return rows.map(function (row) {
    return {
      index: row.index,
      vector: row.vector instanceof PositionVector ? normalizePosition(row.vector) : normalizeInterval(row.vector)
    };
  });
}

describe("musicplusplus parity", function () {
  it("matches vector conversion commands", function () {
    const source = new PositionVector([0, 4, 7], { mod: 12 });
    expect(normalizeInterval(positionsToIntervals(source))).toEqual(
      spec("positions-to-intervals", { "value-data": "0,4,7", "value-mod": "12" })
    );
    expect(normalizeBinary(positionsToBinary(source))).toEqual(
      spec("positions-to-binary", { "value-data": "0,4,7", "value-mod": "12" })
    );
  });

  it("matches selection overloads", function () {
    const source = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const criterion = new IntervalVector([2, 2, 3], { mod: 7, offset: 0 });
    expect(normalizePosition(select(source, criterion))).toEqual(
      spec("select-pos-int", {
        "criterion-data": "2,2,3",
        "criterion-mod": "7",
        "criterion-offset": "0",
        "source-data": "0,2,4,5,7,9,11",
        "source-mod": "12"
      })
    );
  });

  it("matches scale and chord generation", function () {
    const generator = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const scale = new Scale(generator, { mode: 1, root: 2 });
    const generatedChord = chord(generator, new IntervalVector([2, 2, 3], { mod: 7, offset: 0 }), { shift: 1 });

    expect(scale.toJSON()).toEqual(
      spec("scale-pos", {
        "generator-data": "0,2,4,5,7,9,11",
        "generator-mod": "12",
        mode: "1",
        root: "2"
      })
    );
    expect(generatedChord.toJSON()).toEqual(
      spec("chord-pos-int", {
        "criterion-data": "2,2,3",
        "criterion-mod": "7",
        "criterion-offset": "0",
        "scale-data": "0,2,4,5,7,9,11",
        "scale-mod": "12",
        shift: "1"
      })
    );
  });

  it("matches rhythm helpers", function () {
    expect(normalizeInterval(euclidean(8, 3, 1))).toEqual(
      spec("rhythm", { events: "3", kind: "euclidean-interval", offset: "1", steps: "8" })
    );
    expect(normalizePosition(deepRhythm(12, 5, 2, 1))).toEqual(
      spec("rhythm", { events: "5", kind: "deep-rhythm", multiplicity: "2", offset: "1", steps: "12" })
    );
    expect(normalizeBinary(tihai(8, 3, false, 0))).toEqual(
      spec("rhythm", { kind: "tihai-binary", offset: "0", pseudo: "false", repetitions: "3", steps: "8" })
    );
  });

  it("matches matrix generation", function () {
    const source = new PositionVector([0, 4, 7], { mod: 12 });
    expect(normalizeModalMatrix(modalMatrix(source))).toEqual(
      spec("modal-matrix-pos", { "value-data": "0,4,7", "value-mod": "12" })
    );
    expect(transpositionMatrix(source).map(function (row) {
      return { transposition: row.transposition, vector: normalizePosition(row.vector) };
    })).toEqual(
      spec("transposition-matrix", { "value-data": "0,4,7", "value-mod": "12" })
    );
    expect({
      center: rototranslationMatrix(source, 0).center,
      rows: rototranslationMatrix(source, 0).rows.map(function (row) {
        return { translation: row.translation, vector: normalizePosition(row.vector) };
      })
    }).toEqual(
      spec("rototranslation-matrix", { center: "0", "value-data": "0,4,7", "value-mod": "12" })
    );
  });

  it("matches modal selection and automation", function () {
    const source = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const criterion = new IntervalVector([2, 2, 3], { mod: 7, offset: 0 });
    const selectionRows = modalSelection(source, criterion, 1);
    const rototranslations = modalRototranslation(selectionRows as readonly { readonly modeIndex: number; readonly vector: PositionVector }[]);
    const automation = degreeAutomation(source, criterion, 1, new PositionVector([0, 4, 7], { mod: 12 }), 0);

    expect(selectionRows.map(function (row) {
      return { modeIndex: row.modeIndex, vector: normalizePosition(row.vector as PositionVector) };
    })).toEqual(
      spec("modal-selection-pos", {
        "criterion-data": "2,2,3",
        "criterion-mod": "7",
        "criterion-offset": "0",
        degree: "1",
        "source-data": "0,2,4,5,7,9,11",
        "source-mod": "12"
      })
    );
    expect(rototranslations.map(function (row) {
      return {
        modeIndex: row.modeIndex,
        matrix: {
          center: row.matrix.center,
          rows: row.matrix.rows.map(function (matrixRow) {
            return { translation: matrixRow.translation, vector: normalizePosition(matrixRow.vector) };
          })
        }
      };
    })).toEqual(
      spec("modal-rototranslation", {
        "criterion-data": "2,2,3",
        "criterion-mod": "7",
        "criterion-offset": "0",
        degree: "1",
        "source-data": "0,2,4,5,7,9,11",
        "source-mod": "12"
      })
    );
    expect({
      distance: automation.distance,
      modeIndex: automation.modeIndex,
      translationIndex: automation.translationIndex,
      vector: normalizePosition(automation.vector)
    }).toEqual(
      spec("automation-degree", {
        complexity: "0",
        "criterion-data": "2,2,3",
        "criterion-mod": "7",
        "criterion-offset": "0",
        degree: "1",
        "reference-data": "0,4,7",
        "reference-mod": "12",
        "scale-data": "0,2,4,5,7,9,11",
        "scale-mod": "12"
      })
    );
  });

  it("matches note naming and chord naming", function () {
    const system = new NoteNamingSystem();
    const noteResult = system.positionVectorToNoteNames(new PositionVector([60, 64, 67], { mod: 12 }), {
      isDiatonicScale: false,
      moduloValue: 12,
      preferSharps: true
    });
    expect(noteResult).toEqual(
      spec("note-names-position", {
        "is-diatonic-scale": "false",
        "modulo-value": "12",
        "prefer-sharps": "true",
        "value-data": "60,64,67",
        "value-mod": "12"
      })
    );

    const analysis = analyzeChord([60, 64, 67, 71], 0);
    analysis.chordName = buildChordName(analysis);
    expect({
      addedNotes: analysis.addedNotes.map(function (entry) {
        return { interval: entry[0], label: entry[1] };
      }),
      chordName: analysis.chordName,
      hasAugmentedFifth: analysis.hasAugmentedFifth,
      hasCompleteTriad: analysis.hasCompleteTriad,
      hasDiminishedFifth: analysis.hasDiminishedFifth,
      hasDiminishedSeventh: analysis.hasDiminishedSeventh,
      hasFifth: analysis.hasFifth,
      hasMajorSeventh: analysis.hasMajorSeventh,
      hasMajorThird: analysis.hasMajorThird,
      hasMinorSeventh: analysis.hasMinorSeventh,
      hasMinorThird: analysis.hasMinorThird,
      hasPerfectFifth: analysis.hasPerfectFifth,
      hasSeventh: analysis.hasSeventh,
      hasThird: analysis.hasThird,
      root: analysis.root
    }).toMatchObject(
      spec("analyze-chord", { notes: "60,64,67,71", "root-index": "0" })
    );
  });

  it("matches scale database lookup", function () {
    var database = new ScaleDatabase();
    expect(database.findScale([0, 2, 4, 5, 7, 9, 11])).toEqual(
      spec("scale-database-find", { notes: "0,2,4,5,7,9,11" })
    );
  });

  it("matches measure analysis helpers", function () {
    var source = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    expect(measureSummary(source)).toEqual(
      spec("measure-summary", { "value-data": "0,2,4,5,7,9,11", "value-mod": "12" })
    );
    expect(generate(5, 7, 12)).toEqual(
      spec("measure-generate", { m: "5", k: "7", n: "12" })
    );
    expect(isGenerated([0, 2, 4, 6, 8, 10], 12)).toEqual(
      spec("measure-is-generated", { data: "0,2,4,6,8,10", n: "12" })
    );
  });

  it("matches quantize and transpose helpers", function () {
    var inputScale = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    var outputScale = new PositionVector([0, 2, 3, 5, 7, 8, 10], { mod: 12 });
    var transposed = transposeNotes(inputScale, outputScale, [60, 62, 64, 65, 67]);

    expect(quantize(6, [0, 2, 4, 5, 7, 9, 11], true)).toEqual(
      spec("quantize", { left: "true", note: "6", scale: "0,2,4,5,7,9,11" })
    );
    expect({
      degrees: transposed.degrees.toJSON(),
      notes: transposed.notes.toJSON()
    }).toEqual(
      spec("transpose-notes", {
        "in-root": "0",
        notes: "60,62,64,65,67",
        "input-scale-data": "0,2,4,5,7,9,11",
        "input-scale-mod": "12",
        "out-root": "0",
        "output-scale-data": "0,2,3,5,7,8,10",
        "output-scale-mod": "12"
      })
    );
  });

  it("matches melody helpers", function () {
    var chordNotes = [0, 4, 7];
    var scaleNotes = [0, 2, 4, 5, 7, 9, 11];
    var chromatic = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11];
    var analysis = hierarchy(60, chordNotes, scaleNotes, chromatic, 12);
    var selections = tripleSelect(analysis, [
      { delta: -1, type: "chord" },
      { delta: 1, type: "scale" },
      { delta: 2, type: "chromatic" }
    ], chordNotes, scaleNotes, chromatic, 12);

    expect(diminution(5, 4, true, false)).toEqual(
      spec("melody-diminution", { degree: "5", left: "false", length: "4", up: "true" })
    );
    expect(run(5, 4, true)).toEqual(
      spec("melody-run", { degree: "5", direction: "true", length: "4" })
    );
    expect(run2(7, 3)).toEqual(
      spec("melody-run2", { end: "3", start: "7" })
    );
    expect(normalizeNotes([-1, 13, 25], 12)).toEqual(
      spec("melody-normalize-notes", { mod: "12", notes: "-1,13,25" })
    );
    expect(analysis).toEqual(
      spec("melody-hierarchy", {
        chord: "0,4,7",
        chromatic: "0,1,2,3,4,5,6,7,8,9,10,11",
        mod: "12",
        note: "60",
        scale: "0,2,4,5,7,9,11"
      })
    );
    expect(noteFromPosition(-1, chordNotes, 12)).toEqual(
      spec("melody-note-from-position", { mod: "12", position: "-1", vector: "0,4,7" })
    );
    expect(selections).toEqual(
      spec("melody-triple-select", {
        chord: "0,4,7",
        chromatic: "0,1,2,3,4,5,6,7,8,9,10,11",
        deltas: "-1,1,2",
        mod: "12",
        note: "60",
        scale: "0,2,4,5,7,9,11",
        types: "chord,scale,chromatic"
      })
    );
    expect(ornamentLoop([1, -1, 2], 5)).toEqual(
      spec("melody-ornament-loop", { length: "5", values: "1,-1,2" })
    );
    expect(applyTripleSelect([60, 62], chordNotes, scaleNotes, chromatic, 12, [1, -1, 2], ["scale"])).toEqual(
      spec("melody-apply-triple-select", {
        chord: "0,4,7",
        chromatic: "0,1,2,3,4,5,6,7,8,9,10,11",
        mod: "12",
        notes: "60,62",
        ornaments: "1,-1,2",
        scale: "0,2,4,5,7,9,11",
        types: "scale"
      })
    );
  });

  it("matches slonimsky helpers", function () {
    var sequence = [0, 4, 7, 10, 14];

    expect(slonimskyIntervals(sequence)).toEqual(
      spec("slonimsky", { op: "intervals", seq: "0,4,7,10,14" })
    );
    expect(minInterval(sequence)).toEqual(
      spec("slonimsky", { op: "min-interval", seq: "0,4,7,10,14" })
    );
    expect(infrapolationPermutation(5)).toEqual(
      spec("slonimsky", { m: "5", op: "infrapolation-permutation" })
    );
    expect(interpolation(sequence, 2)).toEqual(
      spec("slonimsky", { k: "2", op: "interpolation-int", seq: "0,4,7,10,14" })
    );
    expect(interpolation(sequence, [1, 2])).toEqual(
      spec("slonimsky", { k: "1,2", op: "interpolation-list", seq: "0,4,7,10,14" })
    );
    expect(symmetricInterpolation(sequence, 2)).toEqual(
      spec("slonimsky", { k: "2", op: "symmetric-interpolation-int", seq: "0,4,7,10,14" })
    );
    expect(symmetricInterpolation(sequence, [1, 2])).toEqual(
      spec("slonimsky", { k: "1,2", op: "symmetric-interpolation-list", seq: "0,4,7,10,14" })
    );
    expect(asymmetricInterpolation(sequence, [1, 2], [1, 3])).toEqual(
      spec("slonimsky", { k: "1,2", l: "1,3", op: "asymmetric-interpolation", seq: "0,4,7,10,14" })
    );
    expect(ultrapolation(sequence, 2)).toEqual(
      spec("slonimsky", { k: "2", op: "ultrapolation-int", seq: "0,4,7,10,14" })
    );
    expect(ultrapolation(sequence, [1, 2])).toEqual(
      spec("slonimsky", { k: "1,2", op: "ultrapolation-list", seq: "0,4,7,10,14" })
    );
    expect(infrapolation(sequence, 2)).toEqual(
      spec("slonimsky", { k: "2", op: "infrapolation-int", seq: "0,4,7,10,14" })
    );
    expect(infrapolation(sequence, [1, 2])).toEqual(
      spec("slonimsky", { k: "1,2", op: "infrapolation-list", seq: "0,4,7,10,14" })
    );
    expect(infraInterpolation(sequence, 1, 2)).toEqual(
      spec("slonimsky", { k: "1", l: "2", op: "infra-interpolation", seq: "0,4,7,10,14" })
    );
    expect(interInfrapolation(sequence, 1, 2)).toEqual(
      spec("slonimsky", { k: "1", l: "2", op: "inter-infrapolation", seq: "0,4,7,10,14" })
    );
    expect(infraUltrapolation(sequence, 1, 2)).toEqual(
      spec("slonimsky", { k: "1", l: "2", op: "infra-ultrapolation", seq: "0,4,7,10,14" })
    );
    expect(interUltrapolation(sequence, 1, 2)).toEqual(
      spec("slonimsky", { k: "1", l: "2", op: "inter-ultrapolation", seq: "0,4,7,10,14" })
    );
    expect(ultraInterpolation(sequence, 1, 2)).toEqual(
      spec("slonimsky", { k: "1", l: "2", op: "ultra-interpolation", seq: "0,4,7,10,14" })
    );
    expect(infraInterUltrapolation(sequence, 1, 2, 3)).toEqual(
      spec("slonimsky", { k: "1", l: "2", m: "3", op: "infra-inter-ultrapolation", seq: "0,4,7,10,14" })
    );
    expect(interInfraInterpolation(sequence, 1, 2, 3)).toEqual(
      spec("slonimsky", { k: "1", l: "2", m: "3", op: "inter-infra-interpolation", seq: "0,4,7,10,14" })
    );
    expect(ultraInfraInterpolation(sequence, 1, 2, 3)).toEqual(
      spec("slonimsky", { k: "1", l: "2", m: "3", op: "ultra-infra-interpolation", seq: "0,4,7,10,14" })
    );
  });
});
