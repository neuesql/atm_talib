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
- **Repo-wide override caution:** `vars.DUCKDB_VERSION` / `vars.CI_TOOLS_VERSION` apply to *every* branch. Because the deploy derives the published version from the built artifact, an override changes both what a branch builds and where it publishes (a `v1.5.4` branch built with `vars.DUCKDB_VERSION=v1.5.5` publishes under `/v1.5.5/`). Leave these unset unless you intend that.
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

Confirm CI is green on the new branch, then follow "Publishing". If this new version is now the newest, update the `LATEST_DUCKDB_VERSION` repo variable to it.

## Publishing (self-hosted Pages + GitHub Release)

Automatic: pushing to a `v1.5.*` branch runs `Build & Test (Distribution)`; on success, `deploy.yml` (on `main`) gzips the binaries into `gh-pages` under `/<version>/<platform>/talib.duckdb_extension.gz` (other versions preserved) and creates/updates the GitHub Release tagged `duckdb-<version>`.

The published `<version>` is derived from the built artifact names (`talib-<version>-extension-<arch>`), not from the branch name — so the gh-pages directory and release tag always match the DuckDB version the binary was actually built for, even if an override (below) changed it.

Release tags are `duckdb-vX.Y.Z` (deliberately distinct from the `vX.Y.Z` branch names, to avoid ambiguous git refs).

Manual redeploy: *Actions → Deploy Extension Binaries → Run workflow*, supplying only the build `run_id` (the version is auto-derived from that run's artifacts).

## One-time repo setup

1. **Settings → Pages → Source:** *Deploy from a branch* → `gh-pages` / `(root)`. The first successful `deploy.yml` run creates the `gh-pages` branch; set this afterwards.
2. Confirm **Settings → Actions → General → Workflow permissions** allows *Read and write* (needed for the release + gh-pages push).
3. **Settings → Secrets and variables → Actions → Variables:** set `LATEST_DUCKDB_VERSION` to the newest supported version (e.g. `v1.5.5`). The release for that version is marked GitHub "Latest"; others are not. Bump this when you cut a newer version branch.

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
