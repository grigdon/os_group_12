# Shell Project 1: Group 12

## Project Overview

This shell implementation is designed to replicate core functionalities found in a generic bash shell by directly interfacing with the operating system's process control and I/O management mechanisms.

### Core Implemented Features:

**Process Management**: Creates and manages child processes for command execution using `fork()` and `execv()` system calls.

**Command Execution**: Searches the `$PATH` environment variable to locate and run external commands.

**I/O Redirection**: Redirects standard input (`<`) and standard output (`>`) for commands to and from files.

**Piping**: Chains commands by connecting the standard output of one process to the standard input of another, supporting unlimited pipes, e.g., `cmd1 | cmd2 | cmd3 | cmd4 | ...`.

**Background Processing**: Allows commands to run as background jobs (`&`) and provides basic job control and status notifications.

**Variable and Tilde Expansion**: Expands environment variables (e.g., $USER) and the tilde character (`~`) to their respective values.

**Built-in Commands**: Implements essential internal commands (`cd`, `exit`, `jobs`) that are handled directly by the shell without creating new processes.

**Piping + I/O Redirection**: Supports combining pipes and I/O redirection in a single command (e.g., `cmd1 < input.txt | cmd2 | cmd3 > output.txt`).

**Shell-ception**: Can execute itself recursively - the shell can be launched from within an already running shell instance.

## Group Members
- **Gabe Rigdon**: gcr22@fsu.edu
- **Bruno Page**: bap21i@fsu.edu
- **Luke Stanton**: les22@fsu.edu

## Division of Labor

### Part 1: Prompt
- **Responsibilities**: Display shell prompt in the format `USER@MACHINE:PWD>`, expanding environment variables ($USER, $MACHINE, $PWD) to show current username, machine name, and working directory.
- **Assigned to**: Gabe Rigdon

### Part 2: Environment Variables
- **Responsibilities**: Implement automatic expansion of tokens prefixed with `$` (e.g., `$USER`, `$HOME`) by replacing them with their corresponding environment variable values using `getenv()`.
- **Assigned to**: Gabe Rigdon

### Part 3: Tilde Expansion
- **Responsibilities**: Expand tilde (`~`) at the beginning of paths to the `$HOME` environment variable, handling both standalone `~` and `~/path` formats.
- **Assigned to**: Gabe Rigdon

### Part 4: $PATH Search
- **Responsibilities**: Implement path search functionality to locate executable files by scanning directories specified in the `$PATH` environment variable for commands without slashes.
- **Assigned to**: Gabe Rigdon

### Part 5: External Command Execution
- **Responsibilities**: Execute external commands by forking child processes and using `execv()`, properly handling commands with multiple arguments.
- **Assigned to**: Gabe Rigdon

### Part 6: I/O Redirection
- **Responsibilities**: Implement input (`<`) and output (`>`) redirection, managing file permissions and handling file creation/overwriting for output redirection.
- **Assigned to**: Luke Stanton

### Part 7: Piping
- **Responsibilities**: Implement piping functionality to connect multiple commands, allowing output from one command to serve as input to the next (supporting up to 2 pipes initially).
- **Assigned to**: Luke Stanton

### Part 8: Background Processing
- **Responsibilities**: Enable background job execution with `&`, track job status, print job notifications on start/completion, and implement job management with non-reusable job numbers.
- **Assigned to**: Luke Stanton

### Part 9: Internal Command Execution
- **Responsibilities**: Implement built-in commands (`exit`, `cd`, `jobs`) that execute within the shell process without forking, including command history tracking for `exit`.
- **Assigned to**: Gabe Rigdon

### Extra Credit
- **Unlimited Piping [2 pts]**: Extend piping to support unlimited number of pipes in a single command.
  - **Assigned to**: Bruno Page
- **Piping & I/O Redirection [2 pts]**: Enable simultaneous use of pipes and I/O redirection in a single command.
  - **Assigned to**: Gabe Rigdon
- **Shell-ception [1 pt]**: Support recursive shell execution (running the shell from within itself).
  - **Assigned to**: Gabe Rigdon

## File Listing
```
os_group_12/
│
├── src/
│   ├── helper.c
│   ├── io_redirection.c
│   ├── jobs.c
│   ├── lexer.c
│   ├── main.c
│   └── shell.c
│   
│
├── include/
│   ├── helper.h
│   ├── io_redirection.h
│   ├── jobs.h
│   ├── lexer.h 
│   └── shell.h
│
├── obj/        # .o files (generated during compilation)
│
├── bin/        # "shell" executable (generated during compilation)
│
├── .gitignore
├── Makefile
└── README.md

```

## How to Compile & Execute

### Requirements
- **Compiler**: `gcc`
- **Environment**: Tested on linprog (linprog2.cs.fsu.edu)

### Compilation
```bash
make
```
This will build an executable in `bin/` called `shell`

### Execution
```bash
make run
```
This will run the `shell` executable. 

Alternatively, you can run directly:
```bash
./bin/shell
```

### Clean Environment
```bash
make clean
```
This will delete `shell` and all `.o` files. 

## Development Log
Each member records their contributions here.

### Gabe Rigdon

| Date       | Work Completed / Notes                           |
|------------|--------------------------------------------------|
| 2025-09-29 | Implemented extra credit: piping + I/O redirection |
| 2025-09-26 | Bug fixes and testing                            |
| 2025-09-25 | Completed part 9 (built-in commands)             |
| 2025-09-14 | Completed part 5 (external command execution)    |
| 2025-09-14 | Completed part 4 ($PATH search)                  |
| 2025-09-12 | Completed part 3 (tilde expansion)               |
| 2025-09-12 | Completed part 2 (environment variables)         |
| 2025-09-11 | Completed part 1 (prompt)                        |

### Luke Stanton

| Date       | Work Completed / Notes                           |
|------------|--------------------------------------------------|
| 2025-09-24 | Completed part 8 (background processing)         |
| 2025-09-24 | Completed part 7 (piping)                        |
| 2025-09-18 | Extended part 5 functionality                    |
| 2025-09-18 | Completed part 6 (I/O redirection)               |

### Bruno Page

| Date       | Work Completed / Notes                           |
|------------|--------------------------------------------------|
| 2025-09-28 | Implemented extra credit: unlimited piping       |

## Meetings

| Date       | Attendees                              | Topics Discussed                                      | Outcomes / Decisions                                           |
|------------|----------------------------------------|-------------------------------------------------------|----------------------------------------------------------------|
| 2025-09-24 | Luke Stanton, Gabe Rigdon              | Progress review after completing part 8               | Decided to work on extra credit after completing part 9        |

## Bugs & Resolutions
- **Memory Leak in main.c**: The `current_dir` pointer from `getcwd()` was not being freed, causing memory leaks on each prompt display. **Fixed** by adding `free(current_dir)` after the prompt is printed.
- **Command History Not Showing on Exit**: Initially, command history was added before checking for the `exit` command, causing `exit` itself to appear in history. **Fixed** by moving `add_to_history()` call after built-in command checks.
- **Background Job Tracking**: Job completion messages were not displaying correctly when multiple background jobs finished simultaneously. **Fixed** by using `WNOHANG` in `waitpid()` loop in `check_jobs()`.

## Extra Credit

### 1. Unlimited Piping [2 points] - **COMPLETED**
**Implementation**: Extended the pipeline functionality to support an unlimited number of pipes in a single command. The `split_by_pipes()` function uses dynamic memory allocation with a capacity doubling strategy to handle any number of pipe-separated commands.

**How to Test**:
```bash
# Test with 4 pipes (5 commands)
echo "hello world" | tr ' ' '\n' | sort | uniq | wc -l

# Test with more complex pipeline
cat /etc/passwd | grep root | cut -d: -f1 | sort | head -n 5

# Test with long pipeline
ls -la | grep "\.c$" | awk '{print $9}' | sort | uniq | wc -l
```

**Technical Details**: The implementation uses a dynamically resizing array of `tokenlist` pointers that grows as needed when encountering pipe symbols. Initial capacity is 4, doubling when exceeded.

### 2. Piping & I/O Redirection [2 points] - **COMPLETED**
**Implementation**: Modified the pipeline execution logic to allow I/O redirection on individual commands within a pipeline. Input redirection is applied to the first command, output redirection to the last command, and commands in the middle connect via pipes.

**How to Test**:
```bash
# Input redirection with pipes
cat < input.txt | grep "test" | sort

# Output redirection with pipes
ls -la | grep "\.c$" | sort > output.txt

# Both input and output redirection with pipes
cat < input.txt | grep "error" | sort | uniq > results.txt

# Background processing with pipes and redirection
cat < data.txt | sort | uniq > sorted.txt &
```

**Technical Details**: The `take_redirections()` function separates redirection tokens from command arguments, and `apply_io_redirection()` is called appropriately for the first and last commands in the pipeline while maintaining pipe connections for intermediate commands.

### 3. Shell-ception [1 point] - **COMPLETED**
**Implementation**: The shell can execute itself recursively without any special handling required. This works because the shell is just another executable that can be found via PATH search or direct execution.

**How to Test**:
```bash
# From within your shell, run:
./bin/shell

# You'll see a new shell prompt. You can nest multiple levels:
./bin/shell
# Now in second shell instance
./bin/shell
# Now in third shell instance
exit
exit
exit
# Back to original shell
```

**Technical Details**: No special implementation required - the shell's external command execution mechanism handles running itself. Each nested shell maintains its own job table, command history, and process state. Use `exit` to return to the parent shell instance.

## Testing Notes

### Test Environment
All testing was performed on `linprog` to ensure compatibility with the grading environment.

### Edge Cases Tested
- Empty commands and whitespace-only input
- Commands with no arguments vs. multiple arguments
- Invalid paths and non-existent commands
- Directory execution attempts
- Multiple background jobs running simultaneously
- Pipe chains with failing intermediate commands
- I/O redirection with non-existent input files
- Output redirection with permission-restricted directories
- Maximum background job limit (10 concurrent jobs)
- Nested shell execution up to 5 levels deep

### Known Limitations
- Job numbers are never reused (as per specification)
- Maximum of 10 concurrent background jobs (as per specification)
- Does not support glob patterns, regular expressions, or quoted strings (as per specification)
- Does not implement signal handling for background jobs (polling-based approach as specified)

## Considerations

### Design Decisions
- **Modular Architecture**: Separated functionality into distinct modules (lexer, shell, jobs, I/O redirection) for maintainability and clarity.
- **Memory Management**: Implemented custom `str_dup()` to avoid portability issues with `strdup()`.
- **Error Handling**: Comprehensive error checking with descriptive messages for debugging and user feedback.
- **Job Tracking**: Used a fixed-size array with active flags rather than dynamic structures for simplicity and specification compliance.
