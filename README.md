# Simple Interactive Shell (`sish`)

## 👩‍💻 Overview

This project is a basic Linux shell implementation called **`sish`**. It supports:

- Executing system commands
- Built-in commands: `cd`, `exit`, `history`
- A command history buffer (last 100 commands)
- Multi-process Unix-style **pipes (`|`)**


## 📁 Files

- `sish.c`: Main source code
- `hist100`: Input file to test if only 100 commands are stored in history (from stdin redirection)



## ⚙️ How to Compile and Run

### Compile (on a Linux machine):
```bash
gcc sish.c -o sish -Wall -Werror -std=gnu99
```

## 🔨 Features

### Command Execution

- Prompts with: `sish> `
- Tokenizes input and executes basic Linux commands (e.g., `ls`, `pwd`, `echo`)
- Uses `fork()` + `execvp()`


### Built-in Commands

| Command          | Description |
|------------------|-------------|
| `exit`           | Exit the shell |
| `cd [dir]`       | Change directory using `chdir()` |
| `history`        | Show the last 100 commands |
| `history -c`     | Clear all command history |
| `history [n]`    | Execute command at offset `n` |


### Pipe Support

- Executes commands with pipes like `ls | wc`
- Supports multi-level pipes like `cat hist100 | grep cat | wc`


## 🧪 Test Cases

Please refer to the file **`testcases.txt`** for all test case inputs and expected behaviors.

Ensure you test:

- Basic command execution (`ls`, `pwd`, `echo`, etc.)
- Built-in commands (`cd`, `exit`, `history`)
- History overflow and offset execution
- Piped commands (`ls | wc`, `cat hist100 | grep ... | wc`)

You can also run:
```bash
./sish < hist100
