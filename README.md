# Embedded C Coding Practice Platform

A lightweight, terminal-based coding practice platform (like LeetCode) for embedded C programming. Practice bitwise operations, data structures, and embedded concepts — all compiled and tested locally with GCC.

## Features

- **LeetCode-style questions** — Each question has a problem statement, empty function template, and comprehensive test cases
- **Automated testing** — One command to compile and run all test cases
- **Progressive hints** — Get hints one at a time when stuck
- **Assembly support** — Solve problems in ARM assembly (`.s` files) in addition to C
- **AI-powered question generation** — Add new questions using the included AI prompt
- **Minimal & clean** — No server, no web UI, just your editor and terminal

## Prerequisites

- **GCC** (any recent version with C11 support)
- **Bash** shell
- A text editor of your choice

Verify GCC is installed:
```bash
gcc --version
```

## Setup

1. Clone or copy this project to your machine.

2. Add the `scripts/` directory to your PATH so you can run `run.sh` from any question folder:

```bash
# Add to your ~/.bashrc or ~/.zshrc:
export PATH="/path/to/embedded_coding/scripts:$PATH"
```

Or create a symlink:
```bash
sudo ln -s /path/to/embedded_coding/scripts/run.sh /usr/local/bin/run.sh
```

3. Verify it works:
```bash
run.sh --help
```

## How to Use

### Workflow

```
1. Pick a question folder     →  cd questions/001_set_ith_bit/
2. Read the problem           →  cat question.md  (or open in editor)
3. Write your solution        →  edit solution.c
4. Test your solution         →  run.sh
5. Need help?                 →  run.sh --hint
```

### Solving a Question

```bash
# Navigate to a question
cd questions/001_set_ith_bit/

# Read the problem
cat question.md

# Edit the solution (use your preferred editor)
vim solution.c   # or code solution.c, nano solution.c, etc.

# Run tests
run.sh

# If tests fail, you'll see which ones failed.
# Fix your solution and run again.
```

### Command Reference

| Command              | Description                                      |
|----------------------|--------------------------------------------------|
| `run.sh`             | Compile and run tests (minimal output)           |
| `run.sh --verbose`   | Compile and run tests (show all test results)    |
| `run.sh --hint`      | Reveal the next hint (progressive)               |
| `run.sh --reset-hints` | Reset hint progress to start over             |
| `run.sh --help`      | Show usage information                           |

### Output Examples

**All tests pass (minimal mode):**
```
Compiling...
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  PASS 16/16 ✓  All tests passed!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

**Some tests fail (minimal mode):**
```
Compiling...

Failed Tests:
  ✗ Test 3: Set bit 3 of 0 | Input: num=0x00000000, i=3 | Expected: 0x00000008, Got: 0x00000000

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  FAIL 15/16 ✗  Some tests failed.
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## Project Structure

```
embedded_coding/
├── README.md                     # This file
├── plan.txt                      # Original design document
├── scripts/
│   └── run.sh                    # Main test runner script
├── prompts/
│   └── add_question.md           # AI prompt for generating new questions
└── questions/
    └── 001_set_ith_bit/          # Question folder
        ├── question.md           # Problem statement
        ├── solution.c            # Your solution (edit this!)
        ├── testcases.c           # Test harness (don't edit)
        ├── hints.md              # Progressive hints
        ├── metadata.json         # Question metadata (for cloud/API use)
        └── testcases.json        # Test data in portable JSON format
```

### Question Folder Contents

| File             | Purpose                                              |
|------------------|------------------------------------------------------|
| `question.md`    | LeetCode-style problem statement with examples       |
| `solution.c`     | Function template to implement (this is what you edit)|
| `testcases.c`    | Comprehensive test cases (struct-based harness)      |
| `hints.md`       | 3 progressive hints (approach → technique → solution)|
| `metadata.json`  | Structured question metadata (for future cloud/API)  |
| `testcases.json` | Test data in portable JSON format (for future cloud) |

## Adding New Questions

New questions can be added using an AI agent with the provided prompt:

1. Open `prompts/add_question.md` — this contains detailed instructions for an AI agent.
2. Provide the AI with a freeform description of the problem you want to create.
3. The AI will generate all 4 files in a new numbered question folder.

Example prompt to an AI agent:
```
Using the instructions in prompts/add_question.md, create a new question about
"counting the number of set bits (1s) in a 32-bit integer (popcount/Hamming weight)"
```

## Assembly Questions

The platform supports ARM assembly solutions. To solve a question in assembly:

1. Create a `solution.s` file (instead of `solution.c`) in the question folder.
2. Implement the function using the appropriate calling convention.
3. Run `run.sh` — it auto-detects `.s` files and compiles with GCC.

Note: The `testcases.c` uses `extern` function declarations, so it links seamlessly with both C and assembly implementations.

## Tips

- Start with the empty `solution.c` template — it already has the correct function signature
- Use `run.sh --verbose` to see exactly which tests pass and fail
- Hints are progressive: Hint 1 gives the approach, Hint 2 the technique, Hint 3 the near-solution
- The `.hint_progress` file tracks which hints you've seen (it's in `.gitignore`-friendly dot-file format)
- Test cases are designed to catch common mistakes (off-by-one, wrong operator, overflow)

## License

Personal learning project. Use freely.
