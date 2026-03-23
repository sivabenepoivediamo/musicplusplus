import { describe, expect, it } from "vitest";

import {
  BinaryVector,
  getByComplexity,
  IntervalVector,
  PositionVector,
  variationDistance,
  Vectors
} from "../src";

describe("musicplusplus core API", function () {
  it("supports interval mirror helpers and LCM adaptation", function () {
    const source = new IntervalVector([1, 2, 3, 4, 5, 6], { mod: 12, offset: 0 });
    const adapted = IntervalVector.adaptToLCM([
      new IntervalVector([1, 2], { mod: 6, offset: 0 }),
      new IntervalVector([3, 4], { mod: 4, offset: 0 })
    ]);

    expect(source.doubleMirror(3).toArray()).toEqual([3, 2, 1, 6, 5, 4]);
    expect(source.crossMirror(3, true).toArray()).toEqual([1, 2, 3, 3, 2, 1]);
    expect(source.crossMirror(3, false).toArray()).toEqual([6, 5, 4, 4, 5, 6]);
    expect(adapted.map(function (vector) {
      return vector.toJSON();
    })).toEqual([
      { data: [2, 4], mod: 12, offset: 0 },
      { data: [9, 12], mod: 12, offset: 0 }
    ]);
  });

  it("supports binary metrics, set logic, and pulse analysis", function () {
    const source = new BinaryVector([1, 0, 1, 1, 0], { mod: 5, offset: 0 });
    const left = new BinaryVector([1, 0, 1, 0], { mod: 4, offset: 0 });
    const right = new BinaryVector([1, 0, 0, 1], { mod: 4, offset: 0 });

    expect(source.countPulses()).toBe(3);
    expect(source.density()).toBe(0.6);
    expect(source.getPulseIndices()).toEqual([0, 2, 3]);
    expect(source.getInterOnsetIntervals()).toEqual([2, 1, 2]);
    expect(left.or(right).toArray()).toEqual([1, 0, 1, 1]);
    expect(left.and(right).toArray()).toEqual([1, 0, 0, 0]);
    expect(left.xor(right).toArray()).toEqual([0, 0, 1, 1]);
    expect(left.nor(right).toArray()).toEqual([0, 1, 0, 0]);
    expect(left.nand(right).toArray()).toEqual([0, 1, 1, 1]);
    expect(left.xnor(right).toArray()).toEqual([1, 1, 0, 0]);
    expect(PositionVector.adaptToLCM([
      new PositionVector([0, 2], { mod: 6 }),
      new PositionVector([0, 1], { mod: 4 })
    ]).map(function (vector) {
      return vector.toJSON();
    })).toEqual([
      { data: [0, 4], mod: 12, range: 12, rangeUpdate: false, user: false, userRange: 12 },
      { data: [0, 3], mod: 12, range: 12, rangeUpdate: false, user: false, userRange: 12 }
    ]);
    expect(BinaryVector.adaptToLCM([
      new BinaryVector([1, 0], { mod: 2, offset: 0 }),
      new BinaryVector([1, 0, 1], { mod: 3, offset: 0 })
    ]).map(function (vector) {
      return vector.toJSON();
    })).toEqual([
      { data: [1, 0, 0, 0, 0, 0], mod: 6, offset: 0 },
      { data: [1, 0, 0, 0, 1, 0], mod: 6, offset: 0 }
    ]);
  });

  it("keeps Vectors synchronized across factory and transformation methods", function () {
    const majorTriad = Vectors.fromPositions([0, 4, 7]);
    const setA = Vectors.fromPositions([0, 2, 4, 6]);
    const setB = Vectors.fromPositions([0, 3, 6, 9]);

    expect(Vectors.fromIntervals([4, 3, 5]).toJSON()).toEqual(majorTriad.toJSON());
    expect(Vectors.fromBinary([1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0]).toJSON()).toEqual(majorTriad.toJSON());
    expect(Vectors.euclidean(5, 8).toJSON()).toMatchObject({
      binary: { data: [1, 0, 1, 1, 0, 1, 0, 1], mod: 8, offset: 0 },
      intervals: { data: [2, 1, 2, 2, 1], mod: 8, offset: 0 },
      positions: { data: [0, 2, 3, 5, 7], mod: 8 }
    });

    expect(majorTriad.transpose(5).positions.toArray()).toEqual([5, 9, 12]);
    expect(majorTriad.multiplyPositions(5).positions.toArray()).toEqual([0, 20, 35]);
    expect(majorTriad.rotatePositions(1).positions.toArray()).toEqual([7, 0, 4]);
    expect(majorTriad.rototranslatePositions(1).positions.toArray()).toEqual([4, 7, 12]);
    expect(majorTriad.invertPositions(0).positions.toArray()).toEqual([-7, -4, 0]);
    expect(majorTriad.complementPositions().positions.toArray()).toEqual([1, 2, 3, 5, 6, 8, 9, 10, 11]);
    expect(majorTriad.addToIntervals(2).intervals.toArray()).toEqual([6, 5, 7]);
    expect(majorTriad.multiplyIntervals(2).intervals.toArray()).toEqual([8, 6, 10]);
    expect(majorTriad.rotateIntervals(1).intervals.toArray()).toEqual([3, 5, 4]);
    expect(majorTriad.reverseIntervals().intervals.toArray()).toEqual([5, 3, 4]);
    expect(majorTriad.invertIntervals(0).intervals.toArray()).toEqual([5, 3, 4]);
    expect(majorTriad.rotateBinary(3).positions.toArray()).toEqual([1, 4, 9]);
    expect(majorTriad.complementBinary().positions.toArray()).toEqual([1, 2, 3, 5, 6, 8, 9, 10, 11]);
    expect(majorTriad.multiplyBinary(2).positions.toArray()).toEqual([0, 8, 14]);
    expect(Vectors.fromPositions([0, 2, 4, 7, 9]).divideBinary(2).positions.toArray()).toEqual([0, 1, 2]);
    expect(setA.or(setB).positions.toArray()).toEqual([0, 2, 3, 4, 6, 9]);
    expect(setA.and(setB).positions.toArray()).toEqual([0, 6]);
    expect(setA.xor(setB).positions.toArray()).toEqual([2, 3, 4, 9]);
  });

  it("surfaces core error conditions clearly", function () {
    expect(function () {
      return new BinaryVector([1, 2], { mod: 2 });
    }).toThrow("BinaryVector data must contain only 0s and 1s");

    expect(function () {
      return new BinaryVector([1, 0], { mod: 2 }).divideScalar(3);
    }).toThrow("k must be less than or equal to vector size");

    expect(function () {
      return new PositionVector([0, 4, 7], { mod: 12 }).divideScalar(0);
    }).toThrow("Division by zero");

    expect(function () {
      return variationDistance([], [1]);
    }).toThrow("Empty vector, distance calculation is impossible.");

    expect(function () {
      return getByComplexity([], 0);
    }).toThrow("Cannot get by complexity from empty matrix");
  });
});
