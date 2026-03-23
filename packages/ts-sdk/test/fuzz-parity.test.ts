import { describe, expect, it } from "vitest";

import {
  BinaryVector,
  IntervalVector,
  PositionVector
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

function createRng(seed: number) {
  var state = seed >>> 0;
  return function () {
    state = (state * 1664525 + 1013904223) >>> 0;
    return state / 4294967296;
  };
}

function nextInt(random: () => number, min: number, max: number) {
  return Math.floor(random() * ((max - min) + 1)) + min;
}

function createNumberArray(random: () => number, length: number, min: number, max: number): number[] {
  var result: number[] = [];
  for (var index = 0; index < length; index += 1) {
    result.push(nextInt(random, min, max));
  }
  return result;
}

function createBinaryArray(random: () => number, length: number): number[] {
  var result: number[] = [];
  for (var index = 0; index < length; index += 1) {
    result.push(nextInt(random, 0, 1));
  }
  return result;
}

describe("musicplusplus deterministic fuzz parity", function () {
  it("matches PositionVector on randomized transform cases", function () {
    var random = createRng(123456);

    for (var iteration = 0; iteration < 12; iteration += 1) {
      var mod = [5, 7, 12][iteration % 3];
      var length = nextInt(random, 1, 5);
      var data = createNumberArray(random, length, -12, 24);
      var other = createNumberArray(random, nextInt(random, 1, 4), -5, 8);
      var amount = nextInt(random, -8, 8);
      var resizeStart = nextInt(random, -3, 4);
      var resizeEnd = nextInt(random, -3, 4);
      var axis = nextInt(random, -3, 5);
      var vector = new PositionVector(data, { mod: mod, rangeUpdate: iteration % 2 === 0, userRange: mod * 2, user: iteration % 4 === 0 });

      expect(normalizePosition(vector.rotate(amount))).toEqual(
        spec("position-op", { amount: String(amount), op: "rotate", "value-data": data.join(","), "value-mod": String(mod), "value-range-update": String(iteration % 2 === 0), "value-user": String(iteration % 4 === 0), "value-user-range": String(mod * 2) })
      );
      expect(normalizePosition(vector.rotoTranslate(amount, length + 2))).toEqual(
        spec("position-op", { amount: String(amount), length: String(length + 2), op: "roto-translate", "value-data": data.join(","), "value-mod": String(mod), "value-range-update": String(iteration % 2 === 0), "value-user": String(iteration % 4 === 0), "value-user-range": String(mod * 2) })
      );
      expect(normalizePosition(vector.addVector(other))).toEqual(
        spec("position-op", { op: "add-vector", other: other.join(","), "value-data": data.join(","), "value-mod": String(mod), "value-range-update": String(iteration % 2 === 0), "value-user": String(iteration % 4 === 0), "value-user-range": String(mod * 2) })
      );
      expect(normalizePosition(vector.multiplyVector(other, true))).toEqual(
        spec("position-op", { loop: "true", op: "multiply-vector", other: other.join(","), "value-data": data.join(","), "value-mod": String(mod), "value-range-update": String(iteration % 2 === 0), "value-user": String(iteration % 4 === 0), "value-user-range": String(mod * 2) })
      );
      expect(normalizePosition(vector.inversion(axis, iteration % 2 === 0))).toEqual(
        spec("position-op", { axis: String(axis), op: "inversion", sort: String(iteration % 2 === 0), "value-data": data.join(","), "value-mod": String(mod), "value-range-update": String(iteration % 2 === 0), "value-user": String(iteration % 4 === 0), "value-user-range": String(mod * 2) })
      );
      expect(normalizePosition(vector.negative(mod - 1, iteration % 2 === 0, iteration % 3 === 0))).toEqual(
        spec("position-op", { axis: String(mod - 1), op: "negative", sort: String(iteration % 3 === 0), standard: String(iteration % 2 === 0), "value-data": data.join(","), "value-mod": String(mod), "value-range-update": String(iteration % 2 === 0), "value-user": String(iteration % 4 === 0), "value-user-range": String(mod * 2) })
      );
      expect(normalizePosition(vector.resize(resizeStart, resizeEnd))).toEqual(
        spec("position-op", { end: String(resizeEnd), op: "resize", start: String(resizeStart), "value-data": data.join(","), "value-mod": String(mod), "value-range-update": String(iteration % 2 === 0), "value-user": String(iteration % 4 === 0), "value-user-range": String(mod * 2) })
      );
    }
  });

  it("matches IntervalVector and BinaryVector on randomized transform cases", function () {
    var random = createRng(654321);

    for (var iteration = 0; iteration < 12; iteration += 1) {
      var intervalData = createNumberArray(random, nextInt(random, 1, 5), -6, 9);
      var intervalOther = createNumberArray(random, nextInt(random, 1, 4), 1, 5);
      var intervalMod = [5, 8, 12][iteration % 3];
      var intervalOffset = nextInt(random, -4, 4);
      var intervalAmount = nextInt(random, -8, 8);
      var intervalAxis = nextInt(random, -3, 5);
      var intervalVector = new IntervalVector(intervalData, { mod: intervalMod, offset: intervalOffset });

      expect(normalizeInterval(intervalVector.rotate(intervalAmount, intervalData.length + 2))).toEqual(
        spec("interval-op", { amount: String(intervalAmount), length: String(intervalData.length + 2), op: "rotate", "value-data": intervalData.join(","), "value-mod": String(intervalMod), "value-offset": String(intervalOffset) })
      );
      expect(normalizeInterval(intervalVector.rotoTranslate(intervalAmount, intervalData.length + 2))).toEqual(
        spec("interval-op", { amount: String(intervalAmount), length: String(intervalData.length + 2), op: "roto-translate", "value-data": intervalData.join(","), "value-mod": String(intervalMod), "value-offset": String(intervalOffset) })
      );
      expect(normalizeInterval(intervalVector.divideVector(intervalOther, true))).toEqual(
        spec("interval-op", { loop: "true", op: "divide-vector", other: intervalOther.join(","), "value-data": intervalData.join(","), "value-mod": String(intervalMod), "value-offset": String(intervalOffset) })
      );
      expect(normalizeInterval(intervalVector.inversion(intervalAxis))).toEqual(
        spec("interval-op", { axis: String(intervalAxis), op: "inversion", "value-data": intervalData.join(","), "value-mod": String(intervalMod), "value-offset": String(intervalOffset) })
      );
      expect(normalizeInterval(intervalVector.normalize(intervalMod - 1 || intervalMod))).toEqual(
        spec("interval-op", { modulo: String(intervalMod - 1 || intervalMod), op: "normalize", "value-data": intervalData.join(","), "value-mod": String(intervalMod), "value-offset": String(intervalOffset) })
      );

      var binaryLength = nextInt(random, 2, 6);
      var binaryData = createBinaryArray(random, binaryLength);
      var binaryOther = createBinaryArray(random, nextInt(random, 2, 5));
      var binaryOffset = nextInt(random, -2, 2);
      var binaryAmount = nextInt(random, -5, 5);
      var binaryAxis = nextInt(random, -3, 5);
      var binaryVector = new BinaryVector(binaryData, { mod: binaryLength, offset: binaryOffset });

      expect(normalizeBinary(binaryVector.rotate(binaryAmount))).toEqual(
        spec("binary-op", { amount: String(binaryAmount), op: "rotate", "value-data": binaryData.join(","), "value-mod": String(binaryLength), "value-offset": String(binaryOffset) })
      );
      expect(normalizeBinary(binaryVector.inversion(binaryAxis))).toEqual(
        spec("binary-op", { axis: String(binaryAxis), op: "inversion", "value-data": binaryData.join(","), "value-mod": String(binaryLength), "value-offset": String(binaryOffset) })
      );
      expect(normalizeBinary(binaryVector.transpose(binaryAmount))).toEqual(
        spec("binary-op", { amount: String(binaryAmount), op: "transpose", "value-data": binaryData.join(","), "value-mod": String(binaryLength), "value-offset": String(binaryOffset) })
      );
      expect(normalizeBinary(binaryVector.repeat(2))).toEqual(
        spec("binary-op", { op: "repeat", times: "2", "value-data": binaryData.join(","), "value-mod": String(binaryLength), "value-offset": String(binaryOffset) })
      );
      expect(normalizeBinary(binaryVector.xor(new BinaryVector(binaryOther, { mod: binaryOther.length, offset: 0 })))).toEqual(
        spec("binary-op", { op: "xor", "other-data": binaryOther.join(","), "other-mod": String(binaryOther.length), "other-offset": "0", "value-data": binaryData.join(","), "value-mod": String(binaryLength), "value-offset": String(binaryOffset) })
      );
    }
  });
});
