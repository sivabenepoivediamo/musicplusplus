const music = require("../../dist/index.js");

const source = new music.PositionVector([0, 4, 7], { mod: 12 });
const intervals = music.positionsToIntervals(source);
const chord = music.chord(source, new music.IntervalVector([2, 2, 3], { mod: 3, offset: 0 }));

if (intervals.data.join(",") !== "4,3,5") {
  throw new Error("CommonJS smoke test failed: unexpected interval output");
}

if (chord.toPositions().data.join(",") !== "0,7,16") {
  throw new Error("CommonJS smoke test failed: unexpected chord output");
}

process.stdout.write("max-js smoke passed\n");
