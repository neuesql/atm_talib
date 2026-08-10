# Multi-version CI/CD for `talib` — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Ship the `talib` DuckDB extension across DuckDB v1.5.2–v1.5.5 with a clean per-version-branch build gate (macOS + Linux only), signed Community-Extensions distribution, and multi-version self-hosted GitHub Pages + Releases.

**Architecture:** One long-lived branch per DuckDB target (`v1.5.2`–`v1.5.5`) plus `main` (tracks newest). Each branch's `distribution.yml` calls the DuckDB `_extension_distribution.yml` reusable workflow pinned 1:1 to the matching `extension-ci-tools` branch. A single centralized `deploy.yml` on `main` reacts to successful builds (via `workflow_run`), gzips the binaries into DuckDB's custom-repo layout, and pushes them into a shared `gh-pages` branch while preserving other versions, plus cuts a GitHub Release. Community Extensions signing is handled by a documented manual PR.

**Tech Stack:** GitHub Actions (reusable workflows, `workflow_run`), DuckDB `extension-ci-tools`, `peaceiris/actions-gh-pages`, `softprops/action-gh-release`, Bash, `actionlint` for validation.

## Global Constraints

- **Platforms:** macOS + Linux only. Every build must pass `exclude_archs: windows_amd64;windows_amd64_mingw;windows_amd64_rtools;wasm_mvp;wasm_eh;wasm_threads` (Linux amd64/arm64, macOS amd64/arm64 remain).
- **Extension name:** `talib` (`extension_name: talib`).
- **Version mapping (verified):** DuckDB `vX.Y.Z` ↔ `extension-ci-tools` `vX.Y.Z`, 1:1, for v1.5.2–v1.5.5.
- **GitHub Actions limitation:** a `uses:` ref cannot be an expression. The reusable-workflow ref is a hardcoded literal per branch, equal to that branch's ci-tools version. Only the `with:` inputs may be expressions.
- **Default resolution order for versions:** `workflow_dispatch` input → repo `vars.*` (optional, repo-wide) → hardcoded per-branch literal.
- **gh-pages layout:** `/<duckdb_version>/<platform>/talib.duckdb_extension.gz` — files gzipped, existing version directories preserved on every publish.
- **`main` tracks `v1.5.5`** as its default target.
- **Install (docs):** signed path `INSTALL talib FROM community;` first; custom-repo path requires `SET allow_unsigned_extensions = true;`.

## File Structure

- `scripts/build_pages_site.sh` (new) — pure function: turn CI build artifacts into a gzipped DuckDB custom-repo tree for one version. Independently testable.
- `test/scripts/test_build_pages_site.sh` (new) — local test for the above.
- `.github/workflows/distribution.yml` (new, replaces `main.yml`) — per-branch build + code-quality gate.
- `.github/workflows/deploy.yml` (new, replaces `release.yml`) — centralized publish (gh-pages + Release).
- `docs/RELEASING.md` (new) — version matrix, per-branch defaults/overrides, one-time repo setup, Community-Extensions submission checklist.
- `README.md` (modify) — install + platforms sections.
- Release branches `v1.5.2`–`v1.5.5` (new) — cut from `main`, each with its two version literals.

---

### Task 1: Pages site-builder script (`scripts/build_pages_site.sh`)

Extract the artifact→custom-repo transform into a testable script so the deploy workflow is thin and the gzip/layout logic can be verified locally.

**Files:**
- Create: `scripts/build_pages_site.sh`
- Test: `test/scripts/test_build_pages_site.sh`

**Interfaces:**
- Produces: `scripts/build_pages_site.sh <artifacts_dir> <duckdb_version> <out_dir> <ext_name>` — reads `<artifacts_dir>/<ext>-*extension-<arch>/*.duckdb_extension`, writes `<out_dir>/<duckdb_version>/<arch>/<ext>.duckdb_extension.gz` (gzip -9). Exits non-zero with a message if no `.duckdb_extension` files are found. Consumed by Task 3 (`deploy.yml`).

- [ ] **Step 1: Write the failing test**

Create `test/scripts/test_build_pages_site.sh`:

```bash
#!/usr/bin/env bash
# Test: scripts/build_pages_site.sh builds a gzipped DuckDB custom-repo tree.
set -euo pipefail
cd "$(dirname "$0")/../.."   # repo root

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

# Fake CI artifacts: two arches, matching the reusable workflow's "<ext>-*extension-<arch>" naming.
mkdir -p "$tmp/artifacts/talib-linux-extension-linux_amd64"
mkdir -p "$tmp/artifacts/talib-osx-extension-osx_arm64"
printf 'DUMMY-BINARY' > "$tmp/artifacts/talib-linux-extension-linux_amd64/talib.duckdb_extension"
printf 'DUMMY-BINARY' > "$tmp/artifacts/talib-osx-extension-osx_arm64/talib.duckdb_extension"

bash scripts/build_pages_site.sh "$tmp/artifacts" v1.5.4 "$tmp/site" talib

for p in v1.5.4/linux_amd64 v1.5.4/osx_arm64; do
  f="$tmp/site/$p/talib.duckdb_extension.gz"
  [ -f "$f" ] || { echo "FAIL: missing $f"; exit 1; }
  gzip -t "$f" || { echo "FAIL: not valid gzip: $f"; exit 1; }
done

# Empty artifacts must fail loudly (guards against silently publishing nothing).
mkdir -p "$tmp/empty"
if bash scripts/build_pages_site.sh "$tmp/empty" v1.5.4 "$tmp/site2" talib 2>/dev/null; then
  echo "FAIL: expected non-zero exit on empty artifacts"; exit 1
fi

echo "PASS"
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `bash test/scripts/test_build_pages_site.sh`
Expected: FAIL — `scripts/build_pages_site.sh` does not exist yet (`No such file or directory`).

- [ ] **Step 3: Write the script**

Create `scripts/build_pages_site.sh`:

```bash
#!/usr/bin/env bash
# Build a DuckDB custom-repo Pages tree from CI build artifacts.
#
# Usage: build_pages_site.sh <artifacts_dir> <duckdb_version> <out_dir> [ext_name]
#
# Reads <artifacts_dir>/<ext>-*extension-<arch>/*.duckdb_extension and writes
# <out_dir>/<duckdb_version>/<arch>/<ext>.duckdb_extension.gz (gzip -9), which is
# the layout `INSTALL <ext> FROM '<repo-url>'` expects.
set -euo pipefail

artifacts_dir="${1:?artifacts_dir required}"
version="${2:?duckdb_version required}"
out_dir="${3:?out_dir required}"
ext="${4:-talib}"

found=0
for dir in "$artifacts_dir"/${ext}-*; do
  [ -d "$dir" ] || continue
  arch="${dir##*extension-}"          # ".../talib-...extension-linux_amd64" -> "linux_amd64"
  for f in "$dir"/*.duckdb_extension; do
    [ -f "$f" ] || continue
    mkdir -p "$out_dir/$version/$arch"
    gzip -9 -c "$f" > "$out_dir/$version/$arch/$ext.duckdb_extension.gz"
    found=$((found + 1))
  done
done

if [ "$found" -eq 0 ]; then
  echo "ERROR: no *.duckdb_extension files found under '$artifacts_dir'" >&2
  exit 1
fi

echo "Built $found platform binaries under $out_dir/$version"
```

Then make both executable:

```bash
chmod +x scripts/build_pages_site.sh test/scripts/test_build_pages_site.sh
```

- [ ] **Step 4: Run the test to verify it passes**

Run: `bash test/scripts/test_build_pages_site.sh`
Expected: `PASS`

- [ ] **Step 5: Commit**

```bash
git add scripts/build_pages_site.sh test/scripts/test_build_pages_site.sh
git commit -m "feat(ci): add tested Pages custom-repo site builder"
```

---

### Task 2: Build + code-quality workflow (`distribution.yml`)

Replace `main.yml` with a macOS/Linux-only build gate whose DuckDB and ci-tools versions are configurable via inputs/vars with a per-branch literal default. This runs on every branch and is the "build passes on macOS and Linux" gate.

**Files:**
- Create: `.github/workflows/distribution.yml`
- Delete: `.github/workflows/main.yml`

**Interfaces:**
- Produces: a workflow named **`Build & Test (Distribution)`** (Task 3's `deploy.yml` keys its `workflow_run` trigger off this exact name) that uploads per-arch artifacts named `talib-*extension-<arch>` (consumed by Task 1's script in Task 3).

- [ ] **Step 1: Verify the reusable workflows accept the inputs used below**

Run:

```bash
curl -fsSL https://raw.githubusercontent.com/duckdb/extension-ci-tools/v1.5.5/.github/workflows/_extension_distribution.yml | grep -nE 'duckdb_version|ci_tools_version|extension_name|exclude_archs'
curl -fsSL https://raw.githubusercontent.com/duckdb/extension-ci-tools/v1.5.5/.github/workflows/_extension_code_quality.yml | grep -nE 'duckdb_version|ci_tools_version|extension_name|format_checks'
```

Expected: both files list the inputs referenced in Step 2 under their `on.workflow_call.inputs:`.
If `_extension_code_quality.yml` does **not** exist at `v1.5.5` or lacks `format_checks`, drop the `code-quality` job in Step 2 (keep only `duckdb-stable-build`).

- [ ] **Step 2: Write `distribution.yml`**

Create `.github/workflows/distribution.yml`:

```yaml
name: Build & Test (Distribution)

on:
  push:
    branches: [main, v1.5.2, v1.5.3, v1.5.4, v1.5.5]
  pull_request:
    branches: [main, v1.5.2, v1.5.3, v1.5.4, v1.5.5]
  workflow_dispatch:
    inputs:
      duckdb_version:
        description: "DuckDB version tag (blank = branch default)"
        required: false
        default: ""
      ci_tools_version:
        description: "extension-ci-tools input (blank = branch default; must be compatible with the pinned uses: ref)"
        required: false
        default: ""

concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}-${{ github.head_ref || '' }}
  cancel-in-progress: true

jobs:
  duckdb-stable-build:
    name: Build extension binaries
    uses: duckdb/extension-ci-tools/.github/workflows/_extension_distribution.yml@v1.5.5
    with:
      duckdb_version: ${{ inputs.duckdb_version || vars.DUCKDB_VERSION || 'v1.5.5' }}
      ci_tools_version: ${{ inputs.ci_tools_version || vars.CI_TOOLS_VERSION || 'v1.5.5' }}
      extension_name: talib
      exclude_archs: windows_amd64;windows_amd64_mingw;windows_amd64_rtools;wasm_mvp;wasm_eh;wasm_threads

  code-quality:
    name: Code quality (format + tidy)
    uses: duckdb/extension-ci-tools/.github/workflows/_extension_code_quality.yml@v1.5.5
    with:
      duckdb_version: ${{ inputs.duckdb_version || vars.DUCKDB_VERSION || 'v1.5.5' }}
      ci_tools_version: ${{ inputs.ci_tools_version || vars.CI_TOOLS_VERSION || 'v1.5.5' }}
      extension_name: talib
      format_checks: 'format;tidy'
```

- [ ] **Step 3: Delete the old workflow**

```bash
git rm .github/workflows/main.yml
```

- [ ] **Step 4: Validate the workflow syntax**

Run (install once with `brew install actionlint` if missing):

```bash
actionlint .github/workflows/distribution.yml
```

Expected: no output (exit 0).
Fallback if `actionlint` is unavailable:

```bash
python3 -c "import yaml,sys; yaml.safe_load(open('.github/workflows/distribution.yml')); print('yaml ok')"
```

Expected: `yaml ok`.

- [ ] **Step 5: Commit**

```bash
git add .github/workflows/distribution.yml
git commit -m "ci: replace main.yml with configurable macOS/Linux build + quality gate"
```

---

### Task 3: Centralized deploy workflow (`deploy.yml`)

Replace `release.yml` with a single publisher on `main` that reacts to a successful build on a version branch, gzips binaries into the shared `gh-pages` tree (preserving other versions), and cuts a GitHub Release. Centralizing on `main` sidesteps the `workflow_run`-runs-from-default-branch rule and needs no per-branch deploy file.

**Files:**
- Create: `.github/workflows/deploy.yml`
- Delete: `.github/workflows/release.yml`

**Interfaces:**
- Consumes: the `Build & Test (Distribution)` workflow name and its `talib-*extension-<arch>` artifacts (Task 2); `scripts/build_pages_site.sh` (Task 1).
- Produces: `gh-pages` content at `/<version>/<arch>/talib.duckdb_extension.gz` and a GitHub Release tagged `<version>`.

- [ ] **Step 1: Write `deploy.yml`**

Create `.github/workflows/deploy.yml`:

```yaml
name: Deploy Extension Binaries

on:
  workflow_run:
    workflows: ["Build & Test (Distribution)"]
    types: [completed]
    branches: [v1.5.2, v1.5.3, v1.5.4, v1.5.5]
  workflow_dispatch:
    inputs:
      duckdb_version:
        description: "Version branch to (re)deploy, e.g. v1.5.4"
        required: true
      run_id:
        description: "Build run id to pull artifacts from (required for manual redeploy)"
        required: true

permissions:
  contents: write
  actions: read

concurrency:
  group: deploy-gh-pages
  cancel-in-progress: false

jobs:
  deploy:
    name: Publish binaries
    runs-on: ubuntu-latest
    if: ${{ github.event_name == 'workflow_dispatch' || github.event.workflow_run.conclusion == 'success' }}
    steps:
      - name: Checkout (for build scripts)
        uses: actions/checkout@v4

      - name: Resolve target version
        id: ver
        run: |
          V="${{ github.event.inputs.duckdb_version || github.event.workflow_run.head_branch }}"
          echo "duckdb_version=$V" >> "$GITHUB_OUTPUT"
          echo "Resolved version: $V"

      - name: Download build artifacts
        uses: actions/download-artifact@v4
        with:
          path: artifacts
          run-id: ${{ github.event.inputs.run_id || github.event.workflow_run.id }}
          github-token: ${{ secrets.GITHUB_TOKEN }}

      - name: Build gzipped Pages tree (DuckDB custom-repo layout)
        run: bash scripts/build_pages_site.sh artifacts "${{ steps.ver.outputs.duckdb_version }}" site talib

      - name: Stage GitHub Release assets (raw binaries)
        run: |
          mkdir -p release_assets
          for dir in artifacts/talib-*; do
            [ -d "$dir" ] || continue
            arch="${dir##*extension-}"
            for f in "$dir"/*.duckdb_extension; do
              [ -f "$f" ] || continue
              cp "$f" "release_assets/talib-${arch}.duckdb_extension"
            done
          done
          find release_assets -type f | sort

      - name: Publish to gh-pages (preserve other versions)
        uses: peaceiris/actions-gh-pages@v4
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./site
          keep_files: true
          user_name: 'github-actions[bot]'
          user_email: 'github-actions[bot]@users.noreply.github.com'
          commit_message: "deploy: talib binaries for ${{ steps.ver.outputs.duckdb_version }}"

      - name: Create/update GitHub Release
        uses: softprops/action-gh-release@v2
        with:
          tag_name: ${{ steps.ver.outputs.duckdb_version }}
          target_commitish: ${{ github.event.workflow_run.head_sha || github.sha }}
          name: "DuckDB TA-Lib Extension (DuckDB ${{ steps.ver.outputs.duckdb_version }})"
          body: |
            ## Install (signed, recommended)

            ```sql
            INSTALL talib FROM community;
            LOAD talib;
            ```

            ## Install (self-hosted custom repo, unsigned)

            ```sql
            SET allow_unsigned_extensions = true;
            INSTALL talib FROM 'https://neuesql.github.io/atm_talib';
            LOAD talib;
            ```

            ## Platforms
            - Linux x86_64 (`linux_amd64`), Linux ARM64 (`linux_arm64`)
            - macOS x86_64 (`osx_amd64`), macOS ARM64 (`osx_arm64`)
          files: release_assets/*
          make_latest: false
```

- [ ] **Step 2: Delete the old workflow**

```bash
git rm .github/workflows/release.yml
```

- [ ] **Step 3: Validate the workflow syntax**

Run:

```bash
actionlint .github/workflows/deploy.yml
```

Expected: no output (exit 0).
Fallback:

```bash
python3 -c "import yaml,sys; yaml.safe_load(open('.github/workflows/deploy.yml')); print('yaml ok')"
```

Expected: `yaml ok`.

- [ ] **Step 4: Commit**

```bash
git add .github/workflows/deploy.yml
git commit -m "ci: centralized multi-version gh-pages + release deploy"
```

---

### Task 4: Release + install documentation

Document the version matrix, per-branch defaults/overrides, one-time repo setup, and the manual Community-Extensions submission, and correct the README install/platform sections.

**Files:**
- Create: `docs/RELEASING.md`
- Modify: `README.md` (lines 9–19 install block; lines 131–139 platforms block)

- [ ] **Step 1: Write `docs/RELEASING.md`**

Create `docs/RELEASING.md`:

````markdown
# Releasing talib

## Version matrix

| Branch  | DuckDB (`duckdb_version`) | `extension-ci-tools` (`ci_tools_version` = `uses:` ref) |
| ------- | ------------------------- | ------------------------------------------------------- |
| `main`  | `v1.5.5` (newest)         | `v1.5.5`                                                 |
| `v1.5.2`| `v1.5.2`                  | `v1.5.2`                                                 |
| `v1.5.3`| `v1.5.3`                  | `v1.5.3`                                                 |
| `v1.5.4`| `v1.5.4`                  | `v1.5.4`                                                 |
| `v1.5.5`| `v1.5.5`                  | `v1.5.5`                                                 |

The mapping is 1:1 — every DuckDB `vX.Y.Z` has a matching `extension-ci-tools` `vX.Y.Z` branch.

## How versions are configured

`.github/workflows/distribution.yml` resolves each version as:

`workflow_dispatch` input → repo variable (`vars.DUCKDB_VERSION` / `vars.CI_TOOLS_VERSION`) → hardcoded per-branch literal.

- The **per-branch default** is the literal in that branch's `distribution.yml` (`|| 'v1.5.x'`) and its `uses: …@v1.5.x` refs.
- **One-off overrides:** run the workflow via *Actions → Build & Test (Distribution) → Run workflow* and fill `duckdb_version` / `ci_tools_version`.
- **GitHub Actions limitation:** the `uses:` ref cannot be an expression, so bumping ci-tools *permanently* means editing the literal ref in the file. A one-off `ci_tools_version` input must stay compatible with the pinned ref.

## Cutting a new DuckDB version branch

```bash
git checkout main && git pull
v=v1.5.6   # example
git checkout -B "$v" main
sed -i '' "s|_extension_distribution.yml@v1.5.5|_extension_distribution.yml@$v|" .github/workflows/distribution.yml
sed -i '' "s|_extension_code_quality.yml@v1.5.5|_extension_code_quality.yml@$v|" .github/workflows/distribution.yml
sed -i '' "s|'v1.5.5'|'$v'|g" .github/workflows/distribution.yml
git add .github/workflows/distribution.yml
git commit -m "ci($v): pin DuckDB and ci-tools to $v"
git push -u origin "$v"
```

Confirm CI is green on the new branch, then follow "Publishing".

## Publishing (self-hosted Pages + GitHub Release)

Automatic: pushing to a `v1.5.*` branch runs `Build & Test (Distribution)`; on success, `deploy.yml` (on `main`) gzips the binaries into `gh-pages` under `/<version>/<platform>/talib.duckdb_extension.gz` (other versions preserved) and creates/updates the `v1.5.x` GitHub Release.

Manual redeploy: *Actions → Deploy Extension Binaries → Run workflow*, supplying `duckdb_version` and the build `run_id`.

## One-time repo setup

1. **Settings → Pages → Source:** *Deploy from a branch* → `gh-pages` / `(root)`. The first successful `deploy.yml` run creates the `gh-pages` branch; set this afterwards.
2. Confirm **Settings → Actions → General → Workflow permissions** allows *Read and write* (needed for the release + gh-pages push).

## Community Extensions (signed) — manual submission

DuckDB signs extensions accepted into `duckdb/community-extensions`; users then run `INSTALL talib FROM community;` with no flags. Community CI rebuilds against every DuckDB version it supports from a single ref, so you submit one commit — not per-branch.

1. Pick the commit to ship (usually the tip of the newest green version branch) and update `description.yml` `repo.ref` to that commit SHA (and bump `extension.version` if changed).
2. Fork/clone `duckdb/community-extensions`, add/update `extensions/talib/description.yml` to match this repo's `description.yml`.
3. Open a PR. Wait for their CI matrix to pass and a maintainer to merge.
4. After merge, verify: `INSTALL talib FROM community; LOAD talib;`.

## Optional: match local submodules to a branch

CI ignores the pinned `duckdb`/`extension-ci-tools` submodules (the reusable workflow checks out its own). For local `make` builds against a branch's target, point them at the matching refs:

```bash
git -C duckdb fetch --tags && git -C duckdb checkout v1.5.4
git -C extension-ci-tools fetch && git -C extension-ci-tools checkout v1.5.4
```
````

- [ ] **Step 2: Update the README install block**

In `README.md`, replace the Quick Install block (lines 11–19, the ```sql fenced block under `## ⚡ Quick Install`):

```sql
-- Signed, recommended — no flags needed
INSTALL talib FROM community;
LOAD talib;

-- OR self-hosted custom repo (any supported DuckDB version; unsigned)
SET allow_unsigned_extensions = true;
INSTALL talib FROM 'https://neuesql.github.io/atm_talib';
LOAD talib;
```

- [ ] **Step 3: Update the README platforms block**

In `README.md`, replace the DuckDB-version callout (line 133):

```markdown
> ⚠️ **DuckDB version:** Only **v1.5.2** is supported.
```

with:

```markdown
> ✅ **DuckDB versions:** **v1.5.2, v1.5.3, v1.5.4, v1.5.5** (one release branch each). The custom-repo URL auto-resolves to the version you're running.
```

- [ ] **Step 4: Verify the docs are consistent**

Run:

```bash
grep -n "FROM community" README.md docs/RELEASING.md
grep -n "allow_unsigned_extensions" README.md docs/RELEASING.md
grep -n "v1.5.2, v1.5.3, v1.5.4, v1.5.5" README.md
```

Expected: the community-install line appears in both files, the unsigned flag appears in both, and the platforms line appears in the README.

- [ ] **Step 5: Commit**

```bash
git add docs/RELEASING.md README.md
git commit -m "docs: releasing guide + multi-version install/platform instructions"
```

---

### Task 5: Create the four release branches

Cut `v1.5.2`–`v1.5.5` from `main` with the correct per-branch version literals. Do this **after** Tasks 1–4 are merged to `main`, so each branch inherits the finished workflows/scripts/docs.

**Files:**
- Modify (per branch): `.github/workflows/distribution.yml` (the two `uses:` refs + the `'v1.5.5'` defaults)

**Interfaces:**
- Consumes: `distribution.yml` and `deploy.yml` from Task 2/3.

- [ ] **Step 1: Confirm you are on an up-to-date `main`**

Run:

```bash
git checkout main && git pull
git log --oneline -4
```

Expected: the Task 1–4 commits are present on `main`.

- [ ] **Step 2: Create each branch and pin its literals**

Run (macOS `sed -i ''`):

```bash
for v in v1.5.2 v1.5.3 v1.5.4; do
  git checkout -B "$v" main
  sed -i '' "s|_extension_distribution.yml@v1.5.5|_extension_distribution.yml@$v|" .github/workflows/distribution.yml
  sed -i '' "s|_extension_code_quality.yml@v1.5.5|_extension_code_quality.yml@$v|" .github/workflows/distribution.yml
  sed -i '' "s|'v1.5.5'|'$v'|g" .github/workflows/distribution.yml
  git add .github/workflows/distribution.yml
  git commit -m "ci($v): pin DuckDB and ci-tools to $v"
done
# v1.5.5 == main defaults already; just create the branch pointer.
git checkout -B v1.5.5 main
```

- [ ] **Step 3: Verify each branch pins the right version**

Run:

```bash
for v in v1.5.2 v1.5.3 v1.5.4 v1.5.5; do
  echo "== $v =="
  git show "$v:.github/workflows/distribution.yml" | grep -E "_extension_distribution.yml@|\|\| 'v1\.5"
done
```

Expected: branch `v1.5.N` shows `_extension_distribution.yml@v1.5.N` and `|| 'v1.5.N'` (and `v1.5.5` shows `v1.5.5`). The `on: … branches:` list (which lists all versions) is unchanged — that is correct and harmless.

- [ ] **Step 4: Push branches (confirm with the user first — this publishes to origin)**

```bash
git push -u origin v1.5.2 v1.5.3 v1.5.4 v1.5.5
git checkout main
```

- [ ] **Step 5: Post-push verification**

- Open *Actions* and confirm `Build & Test (Distribution)` ran and is **green on all four branches** (macOS + Linux arches only).
- After the first green build, confirm `deploy.yml` created the `gh-pages` branch, then set **Settings → Pages → `gh-pages` / (root)** (see `docs/RELEASING.md`).
- Verify a real install against one version:

```bash
duckdb -c "SET allow_unsigned_extensions=true; INSTALL talib FROM 'https://neuesql.github.io/atm_talib'; LOAD talib; SELECT t_sma([1.0,2.0,3.0,4.0,5.0], 3);"
```

Expected: the extension loads and returns an SMA list.

---

## Self-Review

**Spec coverage:**
- Build passes on macOS + Linux → Task 2 (`exclude_archs`, Global Constraints). ✓
- Per-version branches v1.5.2–v1.5.5 → Task 5. ✓
- Release with different versions → Task 3 (Release per version) + Task 5. ✓
- Unsigned-plugin problem → Task 4 (Community signed path + documented `allow_unsigned` fallback). ✓
- Clean CI/CD → Tasks 2–3 (replace `main.yml`/`release.yml`, add code-quality, centralize deploy). ✓
- ci-tools version + DuckDB target as variables with per-branch defaults → Task 2 (inputs/vars/literal chain) + Global Constraints (uses-ref caveat). ✓
- Multi-version gh-pages (gzip, preserve siblings) → Task 1 + Task 3. ✓

**Placeholder scan:** No TBD/TODO; all code blocks are complete; the one conditional (drop `code-quality` if the reusable workflow lacks `format_checks`) is gated on an explicit verification step. ✓

**Type/name consistency:** workflow name `Build & Test (Distribution)` matches between Task 2 (`name:`) and Task 3 (`workflow_run.workflows`). Artifact glob `talib-*` / `${dir##*extension-}` is identical in Task 1 (script) and Task 3 (release-asset staging). `scripts/build_pages_site.sh` signature matches its Task 3 call. ✓
