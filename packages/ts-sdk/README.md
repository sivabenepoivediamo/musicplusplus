# musicplusplus

Typed TS/JS port of the `cpp-sdk`.

## Contract

- C++ is the semantic reference.
- TS is the consumable package.
- Shared data lives in `../shared-data`.
- Parity is enforced through the C++ spec runner and TS tests.

## If You Change C++

1. Update the C++ implementation.
2. Update [`../cpp-sdk/tools/spec_runner.cpp`](../cpp-sdk/tools/spec_runner.cpp) if outputs changed or a new feature must be mirrored.
3. Update the relevant tests in [`./test`](./test).
4. Run:

```sh
npm run test --workspace=packages/ts-sdk
npm run test:smoke --workspace=packages/ts-sdk
```

## If You Change TS

1. Keep outputs identical to C++ for mirrored features.
2. If semantics change, update C++ too.
3. Run:

```sh
npm run typecheck --workspace=packages/ts-sdk
npm run test --workspace=packages/ts-sdk
npm run test:smoke --workspace=packages/ts-sdk
```

## Build

```sh
npm run build --workspace=packages/ts-sdk
```

- `dist/index.js`: ES5 CommonJS for Max
- `dist/index.mjs`: modern ESM
- `dist/index.d.ts`: types
- `npm run test`: parity and TS tests
- `npm run test:smoke`: packaging/import smoke checks

## Scope

Mirrored feature families:

- vectors and conversions
- selection, scales, chords
- distances and rhythms
- matrices and automations
- naming, measures, melody, slonimsky

Deliberately not mirrored 1:1:

- operator overloading
- `print*()` helpers
- stream operators
- mutable public-field API
