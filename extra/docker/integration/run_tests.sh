#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

passed=0
failed=0

run_suite() {
    local name="$1"
    local script="$2"
    echo "============================================"
    echo "  Running $name integration tests"
    echo "============================================"
    if bash "$script"; then
        echo "PASS: $name"
        passed=$((passed + 1))
    else
        echo "FAIL: $name"
        failed=$((failed + 1))
    fi
    echo ""
}

run_suite "MySQL"      "$SCRIPT_DIR/test_mysql.sh"
run_suite "PostgreSQL" "$SCRIPT_DIR/test_postgres.sh"

echo "============================================"
echo "  Results: $passed passed, $failed failed"
echo "============================================"

if [ "$failed" -gt 0 ]; then
    exit 1
fi
