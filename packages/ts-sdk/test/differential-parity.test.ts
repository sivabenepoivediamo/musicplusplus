import { describe, expect, it } from "vitest";

import {
  align,
  autoScale,
  BinaryVector,
  computeCDF,
  degreeAutomationReference,
  degreeAutomationSequentialBackward,
  degreeAutomationVectorReference,
  forwardDegreeAutomation,
  forwardVoiceLeading,
  IntervalVector,
  normalizeDistribution,
  PositionVector,
  Vectors,
  voiceLeadingAutomationReference,
  voiceLeadingAutomationSequentialBackward,
  voiceLeadingAutomationVectorReference
} from "../src";
import { spec } from "./support/spec";

function normalizePosition(vector: PositionVector) {
  return vector.toJSON();
}

function normalizeInterval(vector: IntervalVector) {
  return vector.toJSON();
}

function normalizeBinary(vector: BinaryVector) {
  return vector.toJSON();
}

function normalizeVectors(value: Vectors) {
  return value.toJSON();
}

function encodeRows(values: readonly PositionVector[]): string {
  return values.map(function (value) {
    return value.toArray().join(",");
  }).join(";");
}

describe("musicplusplus differential parity", function () {
  it("matches PositionVector semantics across arithmetic, transforms, and utilities", function () {
    const value = new PositionVector([0, 3, 7, 10], { mod: 12, userRange: 24, rangeUpdate: true, user: false });
    const other = new PositionVector([12, 14], { mod: 12, userRange: 24, rangeUpdate: true, user: false });

    expect(normalizePosition(value)).toEqual(
      spec("position-op", {
        op: "identity",
        "value-data": "0,3,7,10",
        "value-mod": "12",
        "value-user-range": "24"
      })
    );
    expect(value.at(-1)).toEqual(
      spec("position-op", { index: "-1", op: "element", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(value.at(5)).toEqual(
      spec("position-op", { index: "5", op: "element", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.addScalar(5))).toEqual(
      spec("position-op", { op: "add-scalar", scalar: "5", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.subtractScalar(2))).toEqual(
      spec("position-op", { op: "subtract-scalar", scalar: "2", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.multiplyScalar(2))).toEqual(
      spec("position-op", { op: "multiply-scalar", scalar: "2", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.divideScalar(2))).toEqual(
      spec("position-op", { op: "divide-scalar", scalar: "2", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.moduloScalar(5))).toEqual(
      spec("position-op", { op: "modulo-scalar", scalar: "5", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.addVector([2, 5, 8]))).toEqual(
      spec("position-op", { op: "add-vector", other: "2,5,8", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.multiplyVector([2, 3], true))).toEqual(
      spec("position-op", { loop: "true", op: "multiply-vector", other: "2,3", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.rotate(-2))).toEqual(
      spec("position-op", { amount: "-2", op: "rotate", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.rotoTranslate(-1, 6))).toEqual(
      spec("position-op", { amount: "-1", length: "6", op: "roto-translate", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.complement())).toEqual(
      spec("position-op", { op: "complement", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.inversion(1, false))).toEqual(
      spec("position-op", { axis: "1", op: "inversion", sort: "false", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.negative(9, false, false))).toEqual(
      spec("position-op", { axis: "9", op: "negative", sort: "false", standard: "false", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.concatenate(other))).toEqual(
      spec("position-op", {
        op: "concatenate",
        "other-data": "12,14",
        "other-mod": "12",
        "other-user-range": "24",
        "value-data": "0,3,7,10",
        "value-mod": "12",
        "value-user-range": "24"
      })
    );
    expect(normalizePosition(value.repeat(3))).toEqual(
      spec("position-op", { op: "repeat", times: "3", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(normalizePosition(value.resize(3, -1))).toEqual(
      spec("position-op", { end: "-1", op: "resize", start: "3", "value-data": "0,3,7,10", "value-mod": "12", "value-user-range": "24" })
    );
    expect(PositionVector.adaptToLCM([
      new PositionVector([0, 2], { mod: 6 }),
      new PositionVector([0, 1], { mod: 4 })
    ]).map(function (vector) {
      return vector.toJSON();
    })).toEqual(
      spec("position-adapt-lcm", {
        "left-data": "0,2",
        "left-mod": "6",
        "right-data": "0,1",
        "right-mod": "4"
      })
    );
  });

  it("matches IntervalVector semantics across arithmetic, transforms, and utilities", function () {
    const value = new IntervalVector([2, 3, 5, 7], { mod: 12, offset: 1 });
    const other = new IntervalVector([4, 1], { mod: 12, offset: 0 });

    expect(normalizeInterval(value)).toEqual(
      spec("interval-op", { op: "identity", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(value.at(-1)).toEqual(
      spec("interval-op", { index: "-1", op: "element", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.addScalar(4))).toEqual(
      spec("interval-op", { op: "add-scalar", scalar: "4", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.divideVector([2, 3], true))).toEqual(
      spec("interval-op", { loop: "true", op: "divide-vector", other: "2,3", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.rotate(3, 6))).toEqual(
      spec("interval-op", { amount: "3", length: "6", op: "rotate", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.rotoTranslate(-5, 6))).toEqual(
      spec("interval-op", { amount: "-5", length: "6", op: "roto-translate", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.reverse())).toEqual(
      spec("interval-op", { op: "reverse", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.retrograde())).toEqual(
      spec("interval-op", { op: "retrograde", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.inversion(2))).toEqual(
      spec("interval-op", { axis: "2", op: "inversion", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.negate())).toEqual(
      spec("interval-op", { op: "negate", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(new IntervalVector([13, 25, -3, 14], { mod: 12, offset: 1 }).normalize(7))).toEqual(
      spec("interval-op", { modulo: "7", op: "normalize", "value-data": "13,25,-3,14", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(value.concatenate(other))).toEqual(
      spec("interval-op", {
        op: "concatenate",
        "other-data": "4,1",
        "other-mod": "12",
        "other-offset": "0",
        "value-data": "2,3,5,7",
        "value-mod": "12",
        "value-offset": "1"
      })
    );
    expect(normalizeInterval(value.repeat(2))).toEqual(
      spec("interval-op", { op: "repeat", times: "2", "value-data": "2,3,5,7", "value-mod": "12", "value-offset": "1" })
    );
    expect(normalizeInterval(new IntervalVector([1, 2, 3, 4, 5, 6], { mod: 12, offset: 0 }).singleMirror(3, true))).toEqual(
      spec("interval-op", { left: "true", op: "single-mirror", position: "3", "value-data": "1,2,3,4,5,6", "value-mod": "12", "value-offset": "0" })
    );
    expect(normalizeInterval(new IntervalVector([1, 2, 3, 4, 5, 6], { mod: 12, offset: 0 }).doubleMirror(3))).toEqual(
      spec("interval-op", { op: "double-mirror", position: "3", "value-data": "1,2,3,4,5,6", "value-mod": "12", "value-offset": "0" })
    );
    expect(normalizeInterval(new IntervalVector([1, 2, 3, 4, 5, 6], { mod: 12, offset: 0 }).crossMirror(3, false))).toEqual(
      spec("interval-op", { left: "false", op: "cross-mirror", position: "3", "value-data": "1,2,3,4,5,6", "value-mod": "12", "value-offset": "0" })
    );
    expect(IntervalVector.adaptToLCM([
      new IntervalVector([1, 2], { mod: 6, offset: 0 }),
      new IntervalVector([3, 4], { mod: 4, offset: 0 })
    ]).map(function (vector) {
      return vector.toJSON();
    })).toEqual(
      spec("interval-adapt-lcm", {
        "left-data": "1,2",
        "left-mod": "6",
        "right-data": "3,4",
        "right-mod": "4"
      })
    );
  });

  it("matches BinaryVector semantics across transforms, analysis, and set logic", function () {
    const value = new BinaryVector([1, 0, 1, 1, 0], { mod: 5, offset: 2 });
    const other = new BinaryVector([1, 1, 0], { mod: 3, offset: 0 });

    expect(normalizeBinary(value)).toEqual(
      spec("binary-op", { op: "identity", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(value.at(-1)).toEqual(
      spec("binary-op", { index: "-1", op: "element", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(normalizeBinary(value.multiplyScalar(2))).toEqual(
      spec("binary-op", { op: "multiply-scalar", scalar: "2", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(normalizeBinary(new BinaryVector([1, 0, 1, 0], { mod: 4, offset: 0 }).divideScalar(2))).toEqual(
      spec("binary-op", { op: "divide-scalar", scalar: "2", "value-data": "1,0,1,0", "value-mod": "4", "value-offset": "0" })
    );
    expect(normalizeBinary(value.rotate(2))).toEqual(
      spec("binary-op", { amount: "2", op: "rotate", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(normalizeBinary(value.complement())).toEqual(
      spec("binary-op", { op: "complement", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(normalizeBinary(value.inversion(1))).toEqual(
      spec("binary-op", { axis: "1", op: "inversion", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(normalizeBinary(value.transpose(3))).toEqual(
      spec("binary-op", { amount: "3", op: "transpose", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(normalizeBinary(value.concatenate(new BinaryVector([0, 1], { mod: 2, offset: 0 })))).toEqual(
      spec("binary-op", {
        op: "concatenate",
        "other-data": "0,1",
        "other-mod": "2",
        "other-offset": "0",
        "value-data": "1,0,1,1,0",
        "value-mod": "5",
        "value-offset": "2"
      })
    );
    expect(normalizeBinary(value.repeat(2))).toEqual(
      spec("binary-op", { op: "repeat", times: "2", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(value.countPulses()).toEqual(
      spec("binary-op", { op: "count-pulses", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(value.density()).toBeCloseTo(
      spec("binary-op", { op: "density", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" }),
      12
    );
    expect(value.getPulseIndices()).toEqual(
      spec("binary-op", { op: "pulse-indices", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(value.getInterOnsetIntervals()).toEqual(
      spec("binary-op", { op: "inter-onset-intervals", "value-data": "1,0,1,1,0", "value-mod": "5", "value-offset": "2" })
    );
    expect(normalizeBinary(new BinaryVector([1, 0, 1, 0], { mod: 4, offset: 0 }).nor(new BinaryVector([1, 1, 0], { mod: 3, offset: 0 })))).toEqual(
      spec("binary-op", {
        op: "nor",
        "other-data": "1,1,0",
        "other-mod": "3",
        "other-offset": "0",
        "value-data": "1,0,1,0",
        "value-mod": "4",
        "value-offset": "0"
      })
    );
    expect(normalizeBinary(new BinaryVector([1, 0, 1, 0], { mod: 4, offset: 0 }).xnor(other))).toEqual(
      spec("binary-op", {
        op: "xnor",
        "other-data": "1,1,0",
        "other-mod": "3",
        "other-offset": "0",
        "value-data": "1,0,1,0",
        "value-mod": "4",
        "value-offset": "0"
      })
    );
    expect(BinaryVector.adaptToLCM([
      new BinaryVector([1, 0], { mod: 2, offset: 0 }),
      new BinaryVector([1, 0, 1], { mod: 3, offset: 0 })
    ]).map(function (vector) {
      return vector.toJSON();
    })).toEqual(
      spec("binary-adapt-lcm", {
        "left-data": "1,0",
        "left-mod": "2",
        "right-data": "1,0,1",
        "right-mod": "3"
      })
    );
  });

  it("matches Vectors factory and transformation semantics", function () {
    const majorTriad = Vectors.fromPositions([0, 4, 7]);

    expect(normalizeVectors(Vectors.fromPositions([0, 4, 7]))).toEqual(
      spec("vectors-factory", { data: "0,4,7", kind: "positions", mod: "12" })
    );
    expect(normalizeVectors(Vectors.fromIntervals([4, 3, 5]))).toEqual(
      spec("vectors-factory", { data: "4,3,5", kind: "intervals", mod: "12" })
    );
    expect(normalizeVectors(Vectors.fromBinary([1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0]))).toEqual(
      spec("vectors-factory", { data: "1,0,0,0,1,0,0,1,0,0,0,0", kind: "binary", mod: "12", offset: "0" })
    );
    expect(normalizeVectors(Vectors.euclidean(5, 8))).toEqual(
      spec("vectors-factory", { kind: "euclidean", pulses: "5", steps: "8" })
    );
    expect(normalizeVectors(majorTriad.transpose(5))).toEqual(
      spec("vectors-op", { amount: "5", op: "transpose", "value-data": "0,4,7", "value-kind": "position", "value-mod": "12" })
    );
    expect(normalizeVectors(majorTriad.negative(10))).toEqual(
      spec("vectors-op", { axis: "10", op: "negative", "value-data": "0,4,7", "value-kind": "position", "value-mod": "12" })
    );
    expect(normalizeVectors(majorTriad.rototranslatePositions(2, 4))).toEqual(
      spec("vectors-op", { amount: "2", length: "4", op: "rototranslate-positions", "value-data": "0,4,7", "value-kind": "position", "value-mod": "12" })
    );
    expect(normalizeVectors(majorTriad.complementPositions())).toEqual(
      spec("vectors-op", { op: "complement-positions", "value-data": "0,4,7", "value-kind": "position", "value-mod": "12" })
    );
    expect(normalizeVectors(majorTriad.multiplyIntervals(2))).toEqual(
      spec("vectors-op", { op: "multiply-intervals", scalar: "2", "value-data": "0,4,7", "value-kind": "position", "value-mod": "12" })
    );
    expect(normalizeVectors(majorTriad.invertIntervals(0))).toEqual(
      spec("vectors-op", { axis: "0", op: "invert-intervals", "value-data": "0,4,7", "value-kind": "position", "value-mod": "12" })
    );
    expect(normalizeVectors(majorTriad.divideBinary(2))).toEqual(
      spec("vectors-op", { op: "divide-binary", scalar: "2", "value-data": "0,4,7", "value-kind": "position", "value-mod": "12" })
    );
    expect(normalizeVectors(Vectors.fromPositions([0, 2, 4, 6]).xor(Vectors.fromPositions([0, 3, 6, 9])))).toEqual(
      spec("vectors-op", {
        op: "xor",
        "other-data": "0,3,6,9",
        "other-kind": "position",
        "other-mod": "12",
        "value-data": "0,2,4,6",
        "value-kind": "position",
        "value-mod": "12"
      })
    );
  });

  it("matches automation sequence helpers and autoScale against the C++ oracle", function () {
    const scale = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const criterion = new IntervalVector([2, 2, 2, 2], { mod: 12, offset: 0 });
    const reference = new PositionVector([4, 7, 11, 12], { mod: 12 });
    const degrees = [0, 1, 2, 3, 4, 5, 6, 7];
    const complexities = [0, 5, 20, 7, 20, 10, 15, 0];
    const targets = [
      new PositionVector([0, 4, 7, 11], { mod: 12 }),
      new PositionVector([2, 5, 9, 12], { mod: 12 }),
      new PositionVector([7, 11, 14, 17], { mod: 12 }),
      new PositionVector([0, 4, 7, 11], { mod: 12 })
    ];
    const references = [
      new PositionVector([0, 4, 7, 11], { mod: 12 }),
      new PositionVector([4, 7, 11, 12], { mod: 12 }),
      new PositionVector([5, 9, 12, 14], { mod: 12 }),
      new PositionVector([5, 7, 11, 14], { mod: 12 })
    ];

    expect(normalizePosition(autoScale(scale, [61, 70]))).toEqual(
      spec("automation-auto-scale", { notes: "61,70", "scale-data": "0,2,4,5,7,9,11", "scale-mod": "12" })
    );
    expect(forwardVoiceLeading(targets, [0, 3, 1]).map(normalizePosition)).toEqual(
      spec("automation-voice-leading-forward-seq", {
        complexities: "0,3,1",
        "targets-data": encodeRows(targets),
        "targets-mod": "12"
      })
    );
    expect(voiceLeadingAutomationReference(targets, reference, [0, 3, 1, 2]).map(normalizePosition)).toEqual(
      spec("automation-voice-leading-reference-seq", {
        complexities: "0,3,1,2",
        "reference-data": "4,7,11,12",
        "reference-mod": "12",
        "targets-data": encodeRows(targets),
        "targets-mod": "12"
      })
    );
    expect(voiceLeadingAutomationSequentialBackward(targets, [0, 3, 1]).map(normalizePosition)).toEqual(
      spec("automation-voice-leading-backward-seq", {
        complexities: "0,3,1",
        "targets-data": encodeRows(targets),
        "targets-mod": "12"
      })
    );
    expect(voiceLeadingAutomationVectorReference(targets, references, [0, 1, 2, 3]).map(normalizePosition)).toEqual(
      spec("automation-voice-leading-vector-reference", {
        complexities: "0,1,2,3",
        "references-data": encodeRows(references),
        "references-mod": "12",
        "targets-data": encodeRows(targets),
        "targets-mod": "12"
      })
    );
    expect(degreeAutomationReference(scale, criterion, degrees, reference, complexities).map(normalizePosition)).toEqual(
      spec("automation-degree-reference-seq", {
        complexities: complexities.join(","),
        "criterion-data": "2,2,2,2",
        "criterion-mod": "12",
        "criterion-offset": "0",
        degrees: degrees.join(","),
        "reference-data": "4,7,11,12",
        "reference-mod": "12",
        "scale-data": "0,2,4,5,7,9,11",
        "scale-mod": "12"
      })
    );
    expect(forwardDegreeAutomation(scale, criterion, degrees, targets[0], complexities).map(normalizePosition)).toEqual(
      spec("automation-degree-forward-seq", {
        complexities: complexities.join(","),
        "criterion-data": "2,2,2,2",
        "criterion-mod": "12",
        "criterion-offset": "0",
        degrees: degrees.join(","),
        "reference-data": "0,4,7,11",
        "reference-mod": "12",
        "scale-data": "0,2,4,5,7,9,11",
        "scale-mod": "12"
      })
    );
    expect(degreeAutomationSequentialBackward(scale, criterion, degrees, targets[0], [0]).map(normalizePosition)).toEqual(
      spec("automation-degree-backward-seq", {
        complexities: "0",
        "criterion-data": "2,2,2,2",
        "criterion-mod": "12",
        "criterion-offset": "0",
        degrees: degrees.join(","),
        "reference-data": "0,4,7,11",
        "reference-mod": "12",
        "scale-data": "0,2,4,5,7,9,11",
        "scale-mod": "12"
      })
    );
    expect(degreeAutomationVectorReference(scale, criterion, degrees.slice(0, 4), references, [0, 1, 2, 3]).map(normalizePosition)).toEqual(
      spec("automation-degree-vector-reference", {
        complexities: "0,1,2,3",
        "criterion-data": "2,2,2,2",
        "criterion-mod": "12",
        "criterion-offset": "0",
        degrees: "0,1,2,3",
        "references-data": encodeRows(references),
        "references-mod": "12",
        "scale-data": "0,2,4,5,7,9,11",
        "scale-mod": "12"
      })
    );
  });

  it("matches align on representative chord pairs", function () {
    const reference = new PositionVector([0, 4, 7], { mod: 12 });
    const targets = [
      new PositionVector([7, 11, 14], { mod: 12 }),
      new PositionVector([2, 5, 9], { mod: 12 }),
      new PositionVector([-1, 2, 7], { mod: 12 })
    ];

    for (const target of targets) {
      expect(align(reference, target)).toEqual(
        spec("align", {
          "reference-data": reference.toArray().join(","),
          "reference-mod": "12",
          "target-data": target.toArray().join(","),
          "target-mod": "12"
        })
      );
    }
  });

  it("matches distribution helpers against the C++ oracle", function () {
    expect(normalizeDistribution([3, 2, 1])).toEqual(
      spec("distribution", { op: "normalize", values: "3,2,1" })
    );
    expect(computeCDF(normalizeDistribution([3, 2, 1]))).toEqual(
      spec("distribution", { op: "cdf", values: "3,2,1" })
    );
  });
});
