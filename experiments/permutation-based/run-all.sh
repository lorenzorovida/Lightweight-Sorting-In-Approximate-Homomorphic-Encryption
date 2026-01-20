#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
THIS_SCRIPT="$(basename "$0")"   # the current script name
mkdir -p "results"

for script in "$SCRIPT_DIR"/*.sh; do
    # Skip this run_all.sh itself
    [[ "$(basename "$script")" == "$THIS_SCRIPT" ]] && continue

    echo
    echo "========================================"
    echo "Running: $(basename "$script")"
    echo "========================================"

    bash "$script"
done