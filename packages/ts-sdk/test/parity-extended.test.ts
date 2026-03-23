import { describe, expect, it } from "vitest";

import {
  chord,
  cloughDouthett,
  IntervalVector,
  PositionVector,
  Scale,
  select
} from "../src";
import { spec } from "./support/spec";

function normalizePosition(vector: PositionVector) {
  return vector.toJSON();
}

function normalizeInterval(vector: IntervalVector) {
  return vector.toJSON();
}

describe("musicplusplus extended parity", function () {
  it("matches the expanded selection example cases", function () {
    const cMajor = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const triadDegrees = new PositionVector([0, 2, 4], { mod: 7 });
    const thirds = new IntervalVector([2, 2, 3], { mod: 12, offset: 0 });
    const majorScaleIntervals = new IntervalVector([2, 2, 1, 2, 2, 2, 1], { mod: 12, offset: 0 });
    const grouping = new IntervalVector([2], { mod: 12, offset: 1 });
    const positions = new PositionVector([0, 2, 5], { mod: 12 });

    const positionCases = [
      { rotation: 0, voices: 0 },
      { rotation: 1, voices: 0 },
      { rotation: 3, voices: 0 },
      { rotation: -1, voices: 0 },
      { rotation: 0, voices: 2 },
      { rotation: 0, voices: 5 }
    ];

    for (const testCase of positionCases) {
      expect(normalizePosition(select(cMajor, triadDegrees, testCase))).toEqual(
        spec("select-pos-pos", {
          "criterion-data": "0,2,4",
          "criterion-mod": "7",
          rotation: String(testCase.rotation),
          "source-data": "0,2,4,5,7,9,11",
          "source-mod": "12",
          voices: String(testCase.voices)
        })
      );
    }

    const intervalCases = [
      { offset: 0, rotation: 0, voices: 0 },
      { offset: 1, rotation: 0, voices: 0 },
      { offset: 2, rotation: 0, voices: 0 },
      { offset: -1, rotation: 0, voices: 0 },
      { offset: 0, rotation: 0, voices: 7 }
    ];

    for (const testCase of intervalCases) {
      expect(normalizePosition(select(cMajor, thirds.withOffset(testCase.offset), testCase))).toEqual(
        spec("select-pos-int", {
          "criterion-data": "2,2,3",
          "criterion-mod": "12",
          "criterion-offset": String(testCase.offset),
          rotation: String(testCase.rotation),
          "source-data": "0,2,4,5,7,9,11",
          "source-mod": "12",
          voices: String(testCase.voices)
        })
      );
    }

    expect(normalizeInterval(select(majorScaleIntervals, grouping, { rotation: 0, voices: 3 }) as IntervalVector)).toEqual(
      spec("select-int-int", {
        "criterion-data": "2",
        "criterion-mod": "12",
        "criterion-offset": "1",
        rotation: "0",
        "source-data": "2,2,1,2,2,2,1",
        "source-mod": "12",
        "source-offset": "0",
        voices: "3"
      })
    );

    expect(normalizeInterval(select(majorScaleIntervals.withOffset(7), grouping.withOffset(2), { rotation: 0, voices: 6 }) as IntervalVector)).toEqual(
      spec("select-int-int", {
        "criterion-data": "2",
        "criterion-mod": "12",
        "criterion-offset": "2",
        rotation: "0",
        "source-data": "2,2,1,2,2,2,1",
        "source-mod": "12",
        "source-offset": "7",
        voices: "6"
      })
    );

    expect(normalizeInterval(select(majorScaleIntervals, positions, { rotation: 0 }) as IntervalVector)).toEqual(
      spec("select-int-pos", {
        "criterion-data": "0,2,5",
        "criterion-mod": "12",
        rotation: "0",
        "source-data": "2,2,1,2,2,2,1",
        "source-mod": "12",
        "source-offset": "0"
      })
    );

    expect(normalizeInterval(select(majorScaleIntervals, positions, { rotation: 2 }) as IntervalVector)).toEqual(
      spec("select-int-pos", {
        "criterion-data": "0,2,5",
        "criterion-mod": "12",
        rotation: "2",
        "source-data": "2,2,1,2,2,2,1",
        "source-mod": "12",
        "source-offset": "0"
      })
    );
  });

  it("matches the missing scale and chord overload cases", function () {
    const cMajor = new PositionVector([0, 2, 4, 5, 7, 9, 11], { mod: 12 });
    const intervalScale = new IntervalVector([2, 2, 1, 2, 2, 2, 1], { mod: 12, offset: 0 });
    const triadDegrees = new PositionVector([0, 2, 4], { mod: 12 });
    const grouping = new IntervalVector([2], { mod: 12, offset: 0 });

    expect(new Scale(intervalScale, { invert: true, inversionAxis: 4, mirror: true, mirrorAxis: 2, mode: 2, root: 5 }).toJSON()).toEqual(
      spec("scale-int", {
        degree: "0",
        "generator-data": "2,2,1,2,2,2,1",
        "generator-mod": "12",
        "generator-offset": "0",
        invert: "true",
        "inversion-axis": "4",
        mirror: "true",
        "mirror-axis": "2",
        mode: "2",
        root: "5"
      })
    );

    expect(chord(cMajor, triadDegrees, { invert: true, axis: 6, preVoices: 3 }).toJSON()).toEqual(
      spec("chord-pos-pos", {
        axis: "6",
        "criterion-data": "0,2,4",
        "criterion-mod": "12",
        invert: "true",
        position: "0",
        "pre-voices": "3",
        rotation: "0",
        "scale-data": "0,2,4,5,7,9,11",
        "scale-mod": "12",
        shift: "0"
      })
    );

    expect(chord(intervalScale, triadDegrees, { position: 1, preVoices: 4 }).toJSON()).toEqual(
      spec("chord-int-pos", {
        "criterion-data": "0,2,4",
        "criterion-mod": "12",
        position: "1",
        "pre-voices": "4",
        rotation: "0",
        "scale-data": "2,2,1,2,2,2,1",
        "scale-mod": "12",
        "scale-offset": "0",
        shift: "0"
      })
    );

    expect(chord(intervalScale, grouping, { negativeOrMirror: true, negativeOrMirrorPos: 2, preVoices: 4 }).toJSON()).toEqual(
      spec("chord-int-int", {
        "criterion-data": "2",
        "criterion-mod": "12",
        "criterion-offset": "0",
        "negative-or-mirror": "true",
        "negative-or-mirror-pos": "2",
        position: "0",
        "pre-voices": "4",
        rotation: "0",
        "scale-data": "2,2,1,2,2,2,1",
        "scale-mod": "12",
        "scale-offset": "0",
        shift: "0"
      })
    );
  });

  it("matches the clough-douthett rhythm helper", function () {
    expect(normalizePosition(cloughDouthett(16, 3, 0))).toEqual(
      spec("rhythm", { events: "3", kind: "clough-douthett", offset: "0", steps: "16" })
    );
  });
});
