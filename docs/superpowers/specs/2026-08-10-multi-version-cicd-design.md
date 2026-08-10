# Multi-version CI/CD for the `talib` DuckDB extension

**Date:** 2026-08-10
**Status:** Approved design, ready for implementation planning

## Goal

Make `atm_talib` build and release cleanly across multiple DuckDB versions on
**macOS and Linux only**, using one long-lived branch per DuckDB target
(`v1.5.2`, `v1.5.3`, `v1.5.4`, `v1.5.5`). Each branch has default versions but
they are overridable at run time. Distribute through DuckDB Community Extensions
(the signed, zero-friction path) while also self-hosting binaries via GitHub
Pages and GitHub Releases.

## Context (current state)

- C++ DuckDB extension named `talib`, wrapping TA-Lib v0.6.4 (CMake `FetchContent`).
- Submodules `duckdb` and `extension-ci-tools` (dirs currently empty locally;
  need `git submodule update --init --recursive`).
- `description.yml` (Community Extensions metadata) already present.
- Existing CI: `main.yml` (build via `_extension_distribution.yml@v1.5.1`, pinned
  `duckdb_version: v1.5.2`) and `release.yml` (single-version GitHub Pages +
  GitHub Release).
- Tags: `v0.1.0`, `v1.5.2`. Only `main` branch exists.

### Verified facts

- `extension-ci-tools` has per-patch branches `v1.5.0`–`v1.5.5` (and `v1.4.x`),
  so the mapping is a clean 1:1: **DuckDB `vX.Y.Z` ↔ ci-tools `vX.Y.Z`**.
- DuckDB has release tags `v1.5.0`–`v1.5.5`.
- GitHub Actions does **not** allow an expression in a `uses:` ref
  (`uses: …@${{ vars.X }}` is illegal). The reusable-workflow ref must be a
  literal.

## Decisions

1. **Signing / distribution:** Community Extensions is the primary signed path
   (`INSTALL talib FROM community; LOAD talib;`, no flags). Also keep self-hosted
   GitHub Pages (custom repo) + GitHub Releases.
2. **Community submission:** manual PR to `duckdb/community-extensions`, driven by
   a documented checklist. No submission automation.
3. **Platforms:** macOS + Linux only. Exclude Windows and WASM
   (`exclude_archs: windows_amd64;windows_amd64_mingw;windows_amd64_rtools;wasm_mvp;wasm_eh;wasm_threads`).
4. Accept the literal-`uses:`-ref constraint (§ Version variables).
5. Pages publishing switches to a **git-pushed shared `gh-pages` branch** with
   gzipped files (§ Self-hosted distribution).

## Branch & version model

- `main` = development, tracks the newest target (currently `v1.5.5`).
- Four release branches, each pinning one DuckDB target:

  | Branch  | `duckdb_version` | `ci_tools_version` = `uses:` ref |
  | ------- | ---------------- | -------------------------------- |
  | `v1.5.2` | `v1.5.2`        | `v1.5.2`                         |
  | `v1.5.3` | `v1.5.3`        | `v1.5.3`                         |
  | `v1.5.4` | `v1.5.4`        | `v1.5.4`                         |
  | `v1.5.5` | `v1.5.5`        | `v1.5.5`                         |

- Branches are cut from `main`; the only per-branch difference is the two default
  version values plus the literal `uses:` ref. Fixes cherry-pick cleanly across
  branches.

## Version as configurable variables (with per-branch defaults)

- `DUCKDB_VERSION` and `CI_TOOLS_VERSION` are exposed as `workflow_dispatch`
  inputs with defaults, falling back to repo/branch `vars.*`. Overridable per run
  without editing files.
- **Constraint:** the reusable-workflow `uses:` ref stays a literal per branch
  (equal to that branch's ci-tools version). `duckdb_version` is fully variable;
  the `ci_tools_version` input drives the inner submodule checkout. A permanent
  ci-tools bump means editing the literal ref; a one-off input override must stay
  compatible with the pinned ref. This is the standard DuckDB-extension pattern.

## Build gate — "passes on macOS + Linux"

- `.github/workflows/distribution.yml` calls
  `duckdb/extension-ci-tools/.github/workflows/_extension_distribution.yml@<branch-ref>`
  with `duckdb_version`, `ci_tools_version`, `extension_name: talib`, and
  `exclude_archs` (Windows + WASM). Builds only `linux_amd64`, `linux_arm64`,
  `osx_amd64`, `osx_arm64`. Runs on push/PR/`workflow_dispatch`.
- SQL tests run via existing `LOAD_TESTS` in `extension_config.cmake`.
- Add `_extension_code_quality.yml` (format + tidy) for a clean CI signal.

## Distribution

### Community Extensions (primary, signed)

- Keep `description.yml` accurate (name, description, maintainers, repo, ref).
- Add `docs/RELEASING.md` with the manual checklist to open/update the
  `duckdb/community-extensions` PR against a tag.
- End users: `INSTALL talib FROM community; LOAD talib;` — no flags.

### Self-hosted GitHub Pages (custom repo)

- `.github/workflows/deploy.yml` publishes into a **shared `gh-pages` branch**
  under `/<duckdb_version>/<platform>/talib.duckdb_extension.gz`, writing only its
  own version subdir and preserving siblings (e.g. `peaceiris/actions-gh-pages`
  with `keep_files: true` or a scoped `destination_dir`).
- Fixes two current bugs:
  - artifact-based `deploy-pages` replaces the whole site and clobbers other
    versions;
  - DuckDB's custom-repo layout requires **gzipped** `.duckdb_extension.gz` files
    for `INSTALL talib FROM 'https://neuesql.github.io/atm_talib'` to resolve.
- Requires users to `SET allow_unsigned_extensions=true`.

### GitHub Release per version

- Attach the raw `.duckdb_extension` binaries (per arch) for manual download.

## README / docs

- Install section leads with the signed community path, then documents the
  custom-repo + `allow_unsigned_extensions` alternative, plus a version-support
  matrix.
- Add the local-dev note: `git submodule update --init --recursive` before `make`.

## File changes

- Rework `.github/workflows/main.yml` → `distribution.yml` (vars/inputs + quality
  check).
- Rework `.github/workflows/release.yml` → `deploy.yml` (multi-version `gh-pages`
  with gzip + keep-siblings, plus GitHub Release).
- Add `docs/RELEASING.md` (version matrix + community-submission checklist).
- Update `README.md` install section.
- Create the four release branches (`v1.5.2`–`v1.5.5`) with correct defaults.

## Out of scope

- Windows and WASM builds.
- Automated Community Extensions submission.
- Changes to the extension's C++ source or TA-Lib pinning.

## Verification

- Each branch's `distribution.yml` build+test must be green on all four
  macOS/Linux arches.
- `gh-pages` serves each version under its own subdir; a real
  `INSTALL talib FROM 'https://neuesql.github.io/atm_talib'` resolves for at least
  one version.
- `INSTALL talib FROM community` works once the community PR merges.
- Local `git submodule update --init --recursive && make` succeeds on macOS.
