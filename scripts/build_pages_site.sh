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
