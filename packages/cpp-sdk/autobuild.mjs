#!/usr/bin/env node
import { execSync, spawnSync } from 'node:child_process';
import { existsSync } from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import fs from 'node:fs';

function hasCmd(cmd) {
  const which = process.platform === 'win32' ? 'where' : 'which';
  const res = spawnSync(which, [cmd], { stdio: 'ignore' });
  return res.status === 0;
}

function run(cmd, opts = {}) {
  execSync(cmd, { stdio: 'inherit', ...opts });
}

const args = process.argv.slice(2);
const action = args[0] || 'build'; // build | test | configure
const isRelease = args.includes('--release');
const cliPreset = (() => {
  const i = args.indexOf('--preset');
  if (i >= 0 && args[i + 1]) return args[i + 1];
  return null;
})();
const envPreset = process.env.CPP_SDK_PRESET || null;

const pkgDir = process.cwd();
if (!existsSync(path.join(pkgDir, 'CMakeLists.txt'))) {
  console.error('Run this script from the cpp-sdk package directory.');
  process.exit(1);
}

const platform = os.platform();
const binDir = path.join(pkgDir, isRelease ? 'build-release' : 'build');

let presetOrder = [];
if (platform === 'win32') {
  // Prefer Ninja, then MinGW, then Visual Studio to avoid MSBuild stalls
  presetOrder = isRelease
    ? ['ninja-release', 'windows-release', 'vs2022-release', 'vs2019-release', 'unix-release']
    : ['ninja', 'windows', 'vs2022', 'vs2019', 'unix'];
} else if (platform === 'darwin') {
  presetOrder = isRelease ? ['ninja-release', 'xcode-release', 'unix-release'] : ['ninja', 'xcode', 'unix'];
} else {
  presetOrder = isRelease ? ['ninja-release', 'unix-release'] : ['ninja', 'unix'];
}

function presetIsViable(p) {
  if (p.startsWith('ninja')) return hasCmd('ninja');
  if (p.startsWith('vs2022')) return hasCmd('cmake'); // rely on CMake to find VS
  if (p.startsWith('vs2019')) return hasCmd('cmake');
  if (p.startsWith('windows')) return hasCmd('mingw32-make') || hasCmd('gcc');
  if (p.startsWith('xcode')) return hasCmd('xcodebuild');
  if (p.startsWith('unix')) return hasCmd('make') || hasCmd('gcc') || hasCmd('clang');
  return true;
}

const presetToGenerator = (p) => {
  if (p.startsWith('ninja')) return 'Ninja';
  if (p.startsWith('vs2022')) return 'Visual Studio 17 2022';
  if (p.startsWith('vs2019')) return 'Visual Studio 16 2019';
  if (p.startsWith('windows')) return 'MinGW Makefiles';
  if (p.startsWith('xcode')) return 'Xcode';
  if (p.startsWith('unix')) return 'Unix Makefiles';
  return '';
};

let candidate = cliPreset || envPreset || presetOrder.find(presetIsViable);
if (candidate && !(presetIsViable(candidate))) {
  // Fallback to autodetect if forced preset is not viable
  candidate = presetOrder.find(presetIsViable);
}
if (!candidate) {
  console.error('No suitable build preset found. Install Ninja, Visual Studio, MinGW, Make, or Xcode.');
  process.exit(2);
}

// If an existing cache was configured with a different generator, clean it automatically
try {
  const cachePath = path.join(binDir, 'CMakeCache.txt');
  if (existsSync(cachePath)) {
    const content = fs.readFileSync(cachePath, 'utf8');
    const m = content.match(/^CMAKE_GENERATOR:INTERNAL=(.*)$/m) || content.match(/^CMAKE_GENERATOR:STRING=(.*)$/m);
    const existingGen = m ? m[1].trim() : null;
    const targetGen = presetToGenerator(candidate);
    if (existingGen && targetGen && existingGen !== targetGen) {
      // Remove entire build directory to safely switch generators in a shared build dir
      run(`cmake -E remove_directory "${binDir}"`, { cwd: pkgDir });
    }
  }
} catch {
  // ignore and let cmake error if something goes wrong
}

// Note: Do not clear FetchContent state to preserve dependency cache between builds.

const extraDefs = [
  '-DFETCHCONTENT_QUIET=OFF',
  '-DFETCHCONTENT_FULLY_DISCONNECTED=OFF',
  '-DCMAKE_MESSAGE_LOG_LEVEL=STATUS',
  `-DMUSICPP_BUILD_TESTS=${action === 'test' ? 'ON' : 'OFF'}`
].join(' ');
const configureCmd = `cmake --preset ${candidate} ${extraDefs}`;
const buildCmd = `cmake --build --preset ${candidate}`;
const testCmd = `ctest --preset ${candidate}`;

try {
  if (action === 'configure') {
    run(configureCmd, { cwd: pkgDir });
  } else if (action === 'build') {
    run(configureCmd, { cwd: pkgDir });
    run(buildCmd, { cwd: pkgDir });
  } else if (action === 'test') {
    run(configureCmd, { cwd: pkgDir });
    run(buildCmd, { cwd: pkgDir });
    run(testCmd, { cwd: pkgDir });
  } else {
    console.error(`Unknown action: ${action}. Use: configure | build | test`);
    process.exit(3);
  }
} catch (e) {
  process.exit(e.status || 1);
}

