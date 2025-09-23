# README – Mini-Shell Practice (Operating Systems)

## Overview

This project is a **practical exercise** for the course _Sistemas Operacionais_ (Operating Systems).
It consists of implementing a **mini-shell**, a simplified command interpreter for Unix/Linux systems.
The goal is to consolidate knowledge about **process creation, execution, and management** using system calls such as `fork()`, `exec()` and `wait()`.

---

## Objectives

By completing this project, students will be able to:

- Understand and apply process-related system calls: `fork()`, `exec()`, and `wait()`.
- Implement process creation and synchronization.
- Manage processes in **foreground** and **background**.
- Prevent zombie processes through proper child handling.
- Parse and interpret user commands in a shell-like environment.

---

## Structure

### Part 1 – Warm-Up (Example Programs)

Before coding the mini-shell, students should analyze and run example programs:

- `fork.c` – basic process creation.
- `fork-print.c` – memory isolation between processes.
- `fork-execve.c` – process image replacement.

**Compilation examples:**

```bash
gcc -Wall fork.c -o fork
gcc -Wall fork-print.c -o fork-print
gcc -Wall fork-execve.c -o fork-execve
```

**Reflection questions:**

- How do parent and child PIDs differ?
- Why do variable values differ in `fork-print.c`?
- What happens to the child process after `execve()`?

---

### Part 2 – Mini-Shell Implementation

The mini-shell follows the classical shell cycle:

1. Display a prompt (`minishell> `).
2. Read user input.
3. Parse the input into command + arguments.
4. Check if it is an **internal** or **external** command.
5. Execute the command.
6. Wait for termination (if foreground).
7. Repeat the cycle.

#### Required Features

- Interactive prompt: `minishell> `.
- Execution of external commands (e.g., `ls`, `date`, `whoami`).
- Internal command:
  - `exit` → terminate shell.
  - `pid` → show shell PID and last child PID.

- Error handling for system calls.
- Support for multiple arguments.
- Well-structured and documented code.

#### Optional Features

- **Background execution** with `&`.
- `jobs` command → list active background jobs.
- `wait` command → wait for all background jobs to finish.

A **template C program** with parsing and execution placeholders is provided in the document.

---

### Part 3 – Testing

**Basic tests:**

```bash
minishell> ls
minishell> date
minishell> pid
minishell> exit
```

**Advanced tests:**

```bash
minishell> sleep 10 &
minishell> jobs
minishell> sleep 5 &
minishell> jobs
minishell> wait
minishell> jobs
```

---

## Concepts Covered

- `fork()` → process creation.
- `execvp()` / `execve()` → process image replacement.
- `wait()` / `waitpid()` → process synchronization.
- PIDs and process identifiers.
- Zombie process prevention.
- Signal handling basics.

---

## Implementation Tips

- Use `strtok()` for parsing commands.
- Always check return values of system calls and use `perror()` for errors.
- Prefer incremental development (one feature at a time).
- Test frequently.

---

## Deliverables

1. **Source code** → `mini_shell.c`.
2. **Technical report** including:
   - Implementation challenges and solutions.
   - Test cases and results.
   - Analysis of applied OS concepts.

---

## execvp vs execve

- **`execvp`** → easier, searches PATH, inherits environment (recommended).
- **`execve`** → more control, requires full path and explicit environment variables.

---

## Background Process Management

- Detect `&` at the end of a command.
- Run child process without waiting.
- Keep a list of background PIDs.
- Implement `jobs` and `wait` to manage them.
- Use `waitpid(-1, &status, WNOHANG)` to clean up finished processes and avoid zombies.

---

## License

This project is an **academic exercise** for educational purposes in Operating Systems courses.
