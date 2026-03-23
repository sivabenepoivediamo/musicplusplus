import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const packageDir = path.resolve(__dirname, "..");
const sourcePath = path.join(packageDir, "scales.json");
const generatedDir = path.join(packageDir, "generated");
const cppOutputPath = path.join(generatedDir, "scaleDictionaryData.inc");
const tsOutputPath = path.join(packageDir, "..", "ts-sdk", "src", "generated", "scales.generated.ts");

/**
 * @typedef {{
 *   sheetName: string;
 *   scaleName: string;
 *   intervals: number[];
 * }} ScaleRecord
 */

/**
 * @param {ScaleRecord[]} scales
 */
function buildCppInclude(scales) {
  return `${scales
    .map(
      (scale) =>
        `        addScale(${JSON.stringify(scale.sheetName)}, ${JSON.stringify(scale.scaleName)}, {${scale.intervals.join(", ")}});`
    )
    .join("\n")}\n`;
}

/**
 * @param {ScaleRecord[]} scales
 */
function buildTsModule(scales) {
  const rows = scales
    .map(
      (scale) =>
        `  { sheetName: ${JSON.stringify(scale.sheetName)}, scaleName: ${JSON.stringify(scale.scaleName)}, intervals: [${scale.intervals.join(", ")}] },`
    )
    .join("\n");

  return `export interface ScaleRecord {
  readonly sheetName: string;
  readonly scaleName: string;
  readonly intervals: readonly number[];
}

export const SCALE_RECORDS = [
${rows}
] as const satisfies readonly ScaleRecord[];
`;
}

if (!fs.existsSync(sourcePath)) {
  throw new Error(`Missing shared scale data at ${sourcePath}`);
}

/** @type {ScaleRecord[]} */
const scales = JSON.parse(fs.readFileSync(sourcePath, "utf8"));

if (!Array.isArray(scales) || scales.some((scale) => !scale || typeof scale.sheetName !== "string" || typeof scale.scaleName !== "string" || !Array.isArray(scale.intervals))) {
  throw new Error("Invalid scale catalog format");
}

fs.mkdirSync(generatedDir, { recursive: true });
fs.writeFileSync(cppOutputPath, buildCppInclude(scales));

const tsDir = path.dirname(tsOutputPath);
fs.mkdirSync(tsDir, { recursive: true });
fs.writeFileSync(tsOutputPath, buildTsModule(scales));

console.log(`Generated ${scales.length} scales`);
console.log(`- ${cppOutputPath}`);
console.log(`- ${tsOutputPath}`);
