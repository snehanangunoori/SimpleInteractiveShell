# Simple Interactive SHell (`sish`)

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

```bash
1. pwd
2. touch a1 a2 a3 a4 a5 a6 a7 a8 a9 a10
3. ls -l
4. rm -f a1 a2 a3 a4 a5 a6 a7 a8 a9 a10
5. echo helloooooooooooooooooooooooooooooooooooooooooooooo
6. mkdir test
7. cd test         # should work
8. cd              # should return error
9. history 0       # executes command at offset 0 (pwd)
10. history -c     # clears history
11. history        # should show only "history"
12. exit
```

### History Overflow Check
```bash
./sish < hist100
```

Expected output:
```
0
1
...
99
```

Only the last 100 commands should be preserved.



### Pipe Testing
```bash
1. ls | wc
2. find . -print | more
3. echo hi | cat | cat
4. cat hist100 | grep -v z -i | sort | uniq | wc -l
   # Expected output: 6
```