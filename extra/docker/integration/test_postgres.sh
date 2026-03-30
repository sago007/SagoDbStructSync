#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
FIXTURES="$SCRIPT_DIR/fixtures"
DBSYNC="/staging/dbsync"
CONNECT="postgresql:dbname=testdb;user=postgres;password=testpass;host=postgres"

export PGPASSWORD="testpass"
PSQL_CMD="psql -h postgres -U postgres -d testdb -t -A"

assert_eq() {
    local description="$1"
    local expected="$2"
    local actual="$3"
    if [ "$expected" != "$actual" ]; then
        echo "ASSERTION FAILED: $description"
        echo "  expected: '$expected'"
        echo "  actual:   '$actual'"
        exit 1
    fi
    echo "  OK: $description"
}

column_exists() {
    local table="$1"
    local column="$2"
    local result
    result=$($PSQL_CMD -c "SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA='public' AND TABLE_NAME='$table' AND COLUMN_NAME='$column'")
    echo "$result"
}

column_max_length() {
    local table="$1"
    local column="$2"
    local result
    result=$($PSQL_CMD -c "SELECT CHARACTER_MAXIMUM_LENGTH FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA='public' AND TABLE_NAME='$table' AND COLUMN_NAME='$column'")
    echo "$result"
}

column_count() {
    local table="$1"
    local result
    result=$($PSQL_CMD -c "SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA='public' AND TABLE_NAME='$table'")
    echo "$result"
}

table_exists() {
    local table="$1"
    local result
    result=$($PSQL_CMD -c "SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA='public' AND TABLE_NAME='$table'")
    echo "$result"
}

echo ""
echo "--- Test 1: Create tables ---"
$DBSYNC -i "$FIXTURES/01_base_model.json" --connectstring "$CONNECT"
assert_eq "test_table exists" "1" "$(table_exists test_table)"
assert_eq "item_id column exists" "1" "$(column_exists test_table item_id)"
assert_eq "title column exists" "1" "$(column_exists test_table title)"
assert_eq "status column exists" "1" "$(column_exists test_table status)"
assert_eq "title is VARCHAR(50)" "50" "$(column_max_length test_table title)"

echo ""
echo "--- Test 2: Append new fields ---"
$DBSYNC -i "$FIXTURES/02_added_fields.json" --connectstring "$CONNECT"
assert_eq "description column exists" "1" "$(column_exists test_table description)"
assert_eq "description is VARCHAR(200)" "200" "$(column_max_length test_table description)"
assert_eq "original columns still present" "1" "$(column_exists test_table title)"
assert_eq "original columns still present" "1" "$(column_exists test_table status)"

echo ""
echo "--- Test 3: Widen varchars ---"
$DBSYNC -i "$FIXTURES/03_wider_varchar.json" --connectstring "$CONNECT"
assert_eq "title widened to VARCHAR(100)" "100" "$(column_max_length test_table title)"
assert_eq "description unchanged at VARCHAR(200)" "200" "$(column_max_length test_table description)"

echo ""
echo "--- Test 4: Fewer fields do not break ---"
$DBSYNC -i "$FIXTURES/04_fewer_fields.json" --connectstring "$CONNECT"
assert_eq "status column still exists" "1" "$(column_exists test_table status)"
assert_eq "description column still exists" "1" "$(column_exists test_table description)"
assert_eq "title column still exists" "1" "$(column_exists test_table title)"
assert_eq "item_id column still exists" "1" "$(column_exists test_table item_id)"

echo ""
echo "--- Test 5: Varchars never narrowed ---"
$DBSYNC -i "$FIXTURES/05_narrower_varchar.json" --connectstring "$CONNECT"
assert_eq "title still VARCHAR(100), not narrowed" "100" "$(column_max_length test_table title)"

echo ""
echo "All PostgreSQL tests passed."
