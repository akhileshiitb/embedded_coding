#!/bin/bash
#
# run.sh - Embedded C Coding Practice Platform
# Compiles and tests solutions for embedded C/assembly questions.
#
# Usage: run.sh [OPTIONS]
#   Run from within a question folder (e.g., questions/001_set_ith_bit/)
#
# Options:
#   --help          Show this help message
#   --verbose       Show all test case results (default: minimal output)
#   --hint          Reveal the next progressive hint
#   --reset-hints   Reset hint progress for this question
#

set -e

# ============================================================================
# Configuration
# ============================================================================
BINARY_NAME=".test_runner"
HINT_PROGRESS_FILE=".hint_progress"
CC="gcc"
CFLAGS="-Wall -Wextra -std=c11"

# ============================================================================
# Colors for terminal output
# ============================================================================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# ============================================================================
# Functions
# ============================================================================

usage() {
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║       Embedded C Coding Practice Platform                   ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo ""
    echo "Usage: run.sh [OPTIONS]"
    echo ""
    echo "Run this script from within a question folder."
    echo ""
    echo "Options:"
    echo "  --help          Show this help message"
    echo "  --verbose       Show all test case results (pass and fail)"
    echo "  --hint          Reveal the next progressive hint"
    echo "  --reset-hints   Reset hint progress for this question"
    echo ""
    echo "Examples:"
    echo "  cd questions/001_set_ith_bit"
    echo "  run.sh              # Compile and run tests (minimal output)"
    echo "  run.sh --verbose    # Compile and run tests (detailed output)"
    echo "  run.sh --hint       # Get next hint"
    echo "  run.sh --reset-hints  # Reset hints to start over"
    echo ""
}

error_exit() {
    echo -e "${RED}Error: $1${NC}" >&2
    exit 1
}

info() {
    echo -e "${BLUE}$1${NC}"
}

# ============================================================================
# Hint Mechanism
# ============================================================================

show_hint() {
    local hints_file="hints.md"

    if [ ! -f "$hints_file" ]; then
        error_exit "No hints.md found in current directory."
    fi

    # Read current hint progress (default: 0 hints shown)
    local current_hint=0
    if [ -f "$HINT_PROGRESS_FILE" ]; then
        current_hint=$(cat "$HINT_PROGRESS_FILE")
    fi

    # Count total hints (lines starting with "## Hint")
    local total_hints
    total_hints=$(grep -c "^## Hint" "$hints_file")

    if [ "$current_hint" -ge "$total_hints" ]; then
        echo -e "${YELLOW}No more hints available. You've seen all $total_hints hints.${NC}"
        echo -e "${YELLOW}Use --reset-hints to start over.${NC}"
        exit 0
    fi

    # Get the next hint (hint numbers are 1-indexed)
    local next_hint=$((current_hint + 1))

    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}  Hint $next_hint of $total_hints${NC}"
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""

    # Extract the hint content between "## Hint N" and the next "## Hint" or end of file
    local hint_content
    hint_content=$(awk "/^## Hint $next_hint/{found=1; next} /^## Hint [0-9]/{if(found) exit} found{print}" "$hints_file")

    echo "$hint_content"
    echo ""

    # Update progress
    echo "$next_hint" > "$HINT_PROGRESS_FILE"

    if [ "$next_hint" -lt "$total_hints" ]; then
        echo -e "${BLUE}(Run --hint again for the next hint)${NC}"
    fi

    exit 0
}

reset_hints() {
    if [ -f "$HINT_PROGRESS_FILE" ]; then
        rm "$HINT_PROGRESS_FILE"
        echo -e "${GREEN}Hint progress reset. Run --hint to start from Hint 1.${NC}"
    else
        echo -e "${YELLOW}No hint progress to reset.${NC}"
    fi
    exit 0
}

# ============================================================================
# Compilation and Test Execution
# ============================================================================

detect_solution_file() {
    if [ -f "solution.c" ]; then
        echo "solution.c"
    elif [ -f "solution.s" ]; then
        echo "solution.s"
    else
        error_exit "No solution file found. Expected 'solution.c' or 'solution.s' in current directory."
    fi
}

compile_solution() {
    local solution_file="$1"
    local testcases_file="testcases.c"

    if [ ! -f "$testcases_file" ]; then
        error_exit "No testcases.c found in current directory."
    fi

    local file_ext="${solution_file##*.}"

    info "Compiling..."

    case "$file_ext" in
        c)
            # C source file
            if ! $CC $CFLAGS -o "$BINARY_NAME" "$solution_file" "$testcases_file" 2>&1; then
                echo ""
                error_exit "Compilation failed. Fix the errors above and try again."
            fi
            ;;
        s)
            # Assembly source file
            if ! $CC -o "$BINARY_NAME" "$solution_file" "$testcases_file" 2>&1; then
                echo ""
                error_exit "Compilation failed. Fix the errors above and try again."
            fi
            ;;
        *)
            error_exit "Unsupported file extension: .$file_ext"
            ;;
    esac
}

run_tests() {
    local verbose="$1"
    local output
    local exit_code

    # Run the test binary and capture output
    set +e
    output=$("./$BINARY_NAME" 2>&1)
    exit_code=$?
    set -e

    # Parse results from the summary line "Results: X/N passed"
    local results_line
    results_line=$(echo "$output" | grep "^Results:")
    local passed total
    passed=$(echo "$results_line" | sed -n 's/Results: \([0-9]*\)\/\([0-9]*\) passed/\1/p')
    total=$(echo "$results_line" | sed -n 's/Results: \([0-9]*\)\/\([0-9]*\) passed/\2/p')

    echo ""

    if [ "$verbose" = "1" ]; then
        # Verbose mode: show all test results
        echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo -e "${BOLD}  Test Results (Verbose)${NC}"
        echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo ""
        echo "$output" | while IFS= read -r line; do
            if [[ "$line" == "[PASS]"* ]]; then
                echo -e "  ${GREEN}✓${NC} ${line#\[PASS\] }"
            elif [[ "$line" == "[FAIL]"* ]]; then
                echo -e "  ${RED}✗${NC} ${line#\[FAIL\] }"
            elif [[ "$line" == "Results:"* ]]; then
                : # Skip, we print our own summary
            else
                echo "  $line"
            fi
        done
        echo ""
    else
        # Minimal mode: show only failures
        local failures
        failures=$(echo "$output" | grep "^\[FAIL\]" || true)
        if [ -n "$failures" ]; then
            echo -e "${BOLD}Failed Tests:${NC}"
            echo "$failures" | while IFS= read -r line; do
                echo -e "  ${RED}✗${NC} ${line#\[FAIL\] }"
            done
            echo ""
        fi
    fi

    # Print summary
    echo -e "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    if [ "$exit_code" -eq 0 ]; then
        echo -e "  ${GREEN}${BOLD}PASS ${passed}/${total} ✓${NC}  All tests passed!"
    else
        echo -e "  ${RED}${BOLD}FAIL ${passed}/${total} ✗${NC}  Some tests failed."
    fi
    echo -e "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    # Cleanup
    rm -f "$BINARY_NAME"

    exit $exit_code
}

# ============================================================================
# Main
# ============================================================================

VERBOSE=0

# Parse arguments
while [ $# -gt 0 ]; do
    case "$1" in
        --help|-h)
            usage
            exit 0
            ;;
        --verbose|-v)
            VERBOSE=1
            shift
            ;;
        --hint)
            show_hint
            ;;
        --reset-hints)
            reset_hints
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo ""
            usage
            exit 1
            ;;
    esac
done

# Detect and compile solution
SOLUTION_FILE=$(detect_solution_file)
info "Detected solution file: $SOLUTION_FILE"

compile_solution "$SOLUTION_FILE"

# Run tests
run_tests "$VERBOSE"
