# Shell Project 1: Group 12

## Project Overview

This shell implementation is designed to replicate core functionalities found in a generic bash shell by directly interfacing with the operating system's process control and I/O management mechanisms.

### Core Implemented Features:

**Process Management**: Creates and manages child processes for command execution using `fork()` and `execv()` system calls.

**Command Execution**: Searches the `$PATH` environment variable to locate and run external commands.

**I/O Redirection**: Redirects standard input (`<`) and standard output (`>`) for commands to and from files.

**Piping**: Chains commands by connecting the standard output of one process to the standard input of another, supporting up to two pipes, e.g., `cmd1 | cmd2 | cmd3`.

**Background Processing**: Allows commands to run as background jobs (`&`) and provides basic job control and status notifications.

**Variable and Tilde Expansion**: Expands environment variables (e.g., $USER) and the tilde character (`~`) to their respective values.

**Built-in Commands**: Implements essential internal commands (`cd`, `exit`, `jobs`) that are handled directly by the shell without creating new processes.

## Group Members
- **Gabe Rigdon**: gcr22@fsu.edu
- **Bruno Page**: bap21i@fsu.edu
- **Luke Stanton**: les22@fsu.edu
## Division of Labor

### Part 1: Prompt
- **Responsibilities**: [Description]
- **Assigned to**: Gabe Rigdon

### Part 2: Environment Variables
- **Responsibilities**: [Description]
- **Assigned to**: Gabe Rigdon

### Part 3: Tilde Expansion
- **Responsibilities**: [Description]
- **Assigned to**: Gabe Rigdon

### Part 4: $PATH Search
- **Responsibilities**: [Description]
- **Assigned to**: Gabe Rigdon

### Part 5: External Command Execution
- **Responsibilities**: [Description]
- **Assigned to**: Gabe Rigdon

### Part 6: I/O Redirection
- **Responsibilities**: [Description]
- **Assigned to**: Luke Stanton

### Part 7: Piping
- **Responsibilities**: [Description]
- **Assigned to**: Luke Stanton

### Part 8: Background Processing
- **Responsibilities**: [Description]
- **Assigned to**: Luke Stanton

### Part 9: Internal Command Execution
- **Responsibilities**: [Description]
- **Assigned to**: Gabe Rigdon

### Extra Credit
- **Responsibilities**: [Description]
- **Assigned to**: Luke Stanton & Gabe Rigdon

## File Listing
```
os_group_12/
│
├── src/
│   ├── helper.c
|   ├── io_redirection.c
|   ├── jobs.c
│   ├── lexer.c
│   ├── main.c
│   └── shell.c
│   
│
├── include/
│   ├── helper.h
│   ├── io_redirection.h
│   ├── jobs.h
|   ├── lexer.h 
│   └── shell.h
│
├── obj/        #.o files
│
├── bin/        # "shell" executable
│
├── .gitignore
├── Makefile
└── README.md

```
## How to Compile & Execute

### Requirements
- **Compiler**: `gcc`

### Compilation
```bash
make
```
This will build an executable in `/bin` called `shell`
### Execution
```bash
make run
```
This will run the the `shell` executable. 

### Clean Environment
```bash
make clean
```
This will delete `shell` and all `.o` files. 

## Development Log
Each member records their contributions here.

### Gabe Rigdon

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2025-09-25 | Completed part 9       |
| 2025-09-14 | Completed part 5       |
| 2025-09-14 | Completed part 4       |
| 2025-09-12 | Completed part 3       |
| 2025-09-12 | Completed part 2       |
| 2025-09-11 | Completed part 1       |

### Luke Stanton

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2025-09-18 | Completed Part 6       |
| 2025-09-18 | Extended Part 5        |
| 2025-09-24 | Completed Part 7       |
| 2025-09-24 | Completed Part 8       |


### Bruno Page

| Date       | Work Completed / Notes |
|------------|------------------------|
| YYYY-MM-DD | [Description of task]  |
| YYYY-MM-DD | [Description of task]  |
| YYYY-MM-DD | [Description of task]  |


## Meetings

| Date       | Attendees            | Topics Discussed | Outcomes / Decisions |
|------------|----------------------|------------------|-----------------------|
| 2025-09-24 | Luke Stanton, Gabe Rigdon          | Progress review after completing part 8. of the project   | Decided to work on the extra credit after part 9.  |
| YYYY-MM-DD | [Names]              | [Agenda items]   | [Actions/Next steps]  |





## Bugs
- **Bug 1**: This is bug 1.
- **Bug 2**: This is bug 2.
- **Bug 3**: This is bug 3.

## Extra Credit
- **Extra Credit 1**: In Progress ~ Description of item goes here
- **Extra Credit 2**: In Progress ~ Description of item goes here
- **Extra Credit 3**: In Progress ~ Description of item goes here

## Considerations
[Description]
