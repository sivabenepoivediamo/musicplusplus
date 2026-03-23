import { execFileSync } from "node:child_process";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const currentDirectory = dirname(fileURLToPath(import.meta.url));
const specRunner = resolve(currentDirectory, "../../../cpp-sdk/build/musicplusplus-spec");

export function spec(command: string, args: Record<string, string>): any {
  const cliArgs: string[] = [command];
  const keys = Object.keys(args);

  for (let index = 0; index < keys.length; index += 1) {
    cliArgs.push(`--${keys[index]}`, args[keys[index]]);
  }

  const output = execFileSync(specRunner, cliArgs, { encoding: "utf8" }).trim();
  if (output.startsWith("{") || output.startsWith("[")) {
    return JSON.parse(output);
  }
  if (/^-?\d+(\.\d+)?$/.test(output)) {
    return Number(output);
  }
  return output;
}
