#!/usr/bin/env node
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';

const targets = ['build', 'build-release'];
const cwd = process.cwd();

function rmDirForceRecursive(dirPath) {
    try {
        fs.rmSync(dirPath, { recursive: true, force: true, maxRetries: 5, retryDelay: 200 });
        return true;
    } catch {
        return false;
    }
}

function powershellRemove(dirPath) {
    const ps = spawnSync('powershell', [
        '-NoProfile',
        '-ExecutionPolicy', 'Bypass',
        'Remove-Item', '-Recurse', '-Force', '-ErrorAction', 'SilentlyContinue', dirPath
    ], { stdio: 'ignore' });
    return ps.status === 0;
}

let exitCode = 0;
for (const t of targets) {
    const full = path.join(cwd, t);
    if (!fs.existsSync(full)) continue;
    let ok = rmDirForceRecursive(full);
    if (!ok && process.platform === 'win32') {
        ok = powershellRemove(full);
    }
    if (!ok) {
        console.error(`Failed to remove ${full}`);
        exitCode = 1;
    }
}

process.exit(exitCode);

