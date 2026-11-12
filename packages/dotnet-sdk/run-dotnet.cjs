/**
 * Runs `dotnet` with stable paths. SDK 9 + NuGet GetRestoreSettingsTask can throw (path1 null) when
 * NUGET_PACKAGES / related env vars are set but empty, or when restore paths are otherwise unset.
 */
const { spawnSync } = require("child_process");
const path = require("path");
const fs = require("fs");
const os = require("os");

const root = __dirname;
const defaultNuGetPackages = path.join(os.homedir(), ".nuget", "packages");

const env = { ...process.env };

function clearIfEmpty(key) {
  const v = env[key];
  if (v === undefined) return;
  if (String(v).trim() === "") delete env[key];
}

// Empty strings confuse NuGet path combination on some machines / CI images.
[
  "NUGET_PACKAGES",
  "NUGET_HTTP_CACHE_PATH",
  "NUGET_PLUGIN_PATHS",
  "DOTNET_ROOT",
].forEach(clearIfEmpty);

// Ensure a real global packages folder (same default as `dotnet` uses).
if (!env.NUGET_PACKAGES || String(env.NUGET_PACKAGES).trim() === "") {
  env.NUGET_PACKAGES = defaultNuGetPackages;
}

const restoreProps = [
  `-p:RestorePackagesPath=${defaultNuGetPackages}`,
  `-p:NuGetPackageRoot=${defaultNuGetPackages}`,
];

function run(args) {
  const r = spawnSync("dotnet", args, {
    stdio: "inherit",
    cwd: root,
    env,
    shell: false,
  });
  process.exit(r.status === 0 ? 0 : r.status ?? 1);
}

const cmd = process.argv[2] || "build";
if (cmd === "build") {
  const csproj = path.join(root, "Musicplusplus.csproj");
  if (!fs.existsSync(csproj)) {
    console.error("Missing:", csproj);
    process.exit(1);
  }
  run(["build", csproj, "-c", "Release", ...restoreProps, ...process.argv.slice(3)]);
} else if (cmd === "test") {
  const csproj = path.join(root, "tests", "Musicplusplus.Tests.csproj");
  if (!fs.existsSync(csproj)) {
    console.error("Missing:", csproj);
    process.exit(1);
  }
  run(["test", csproj, "-c", "Release", ...restoreProps, ...process.argv.slice(3)]);
} else {
  run(process.argv.slice(2));
}
