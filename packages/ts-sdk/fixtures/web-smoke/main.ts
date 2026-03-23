import { PositionVector, positionsToIntervals } from "../../dist/index.mjs";

const source = new PositionVector([0, 4, 7], { mod: 12 });
const intervals = positionsToIntervals(source);

if (intervals.data.join(",") !== "4,3,5") {
  throw new Error("ESM smoke test failed");
}

document.getElementById("app")!.textContent = intervals.data.join(",");
