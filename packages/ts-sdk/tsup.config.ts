import { defineConfig } from "tsup";

export default defineConfig([
  {
    clean: true,
    dts: false,
    entry: ["src/index.ts"],
    format: ["cjs"],
    minify: false,
    outDir: "dist",
    outExtension: function () {
      return { js: ".js" };
    },
    sourcemap: false,
    target: "es5",
    treeshake: true
  },
  {
    clean: false,
    dts: false,
    entry: ["src/index.ts"],
    format: ["esm"],
    minify: false,
    outDir: "dist",
    outExtension: function () {
      return { js: ".mjs" };
    },
    sourcemap: false,
    target: "es2020",
    treeshake: true
  }
]);
