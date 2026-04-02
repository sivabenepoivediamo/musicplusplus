# Musicplusplus (.NET)

Class library port of the `cpp-sdk` music theory utilities. Targets **.NET 8**; safe to reference from **vvvv gamma** or any .NET consumer.

**Prerequisite:** Install the [.NET 8 SDK](https://dotnet.microsoft.com/download/dotnet/8.0). `global.json` pins the 8.x SDK line for this folder.

**Turborepo:** Root `turbo.json` sets `envMode: "loose"` so tasks inherit a full OS environment. The default strict mode only passes a small allowlist and omits variables such as `ProgramFiles` that NuGet/MSBuild combine into paths; that can surface as `path1` null during restore on Windows.

- **Project:** `Musicplusplus.csproj` (package root)
- **Sources:** `src/`
- **Tests:** `tests/` (`npm run test` or `node run-dotnet.cjs test`)

Public API lives in the `Musicpp` namespace (with `Musicpp.Slonimsky` for Slonimsky helpers).

**Note:** `dotnet build` always runs a *restore* step (framework targeting packs, and test projects’ NuGet deps like xUnit). That uses the same NuGet machinery as `dotnet pack` / nuget.org, but you are **not** publishing anything unless you add a pack/publish step.
