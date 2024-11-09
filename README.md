# Mini Shell

## Overview
This project implements a simple shell in C that supports basic commands and piping. Additionally, it includes a daemon process that logs system information at specified intervals.

## Features
- Supports basic commands like `ls`, `echo`, `cat`, `grep`, `head`, `tail`, `wc`, `sort`, etc.
- Supports piping between commands, e.g., `ps -aux | sort -nr -k 4 | head -10`.
- Includes a daemon that logs system information to syslog.

## Compilation
To compile the shell, run `make` in the project directory:
```bash
$ make
```

If you do not have make, you can compile using gcc:
```bash
$ gcc -Wall -o mishell p1.c
$ ./mishell
```
## Usage

### Running the Shell

Start the shell by executing the compiled binary:

```bash
$ ./mishell
```

### Supported Commands
You can run simple commands like:

```bash
mishell:$ ls
mishell:$ echo "Hello, World!"
mishell:$ cat file.txt
```

### Piping Commands
You can also pipe commands:

```bash
mishell:$ ps -aux | sort -nr -k 4 | head -10
mishell:$ cat texto.txt | wc -w
```

### Daemon
To create a daemon that logs system information every t seconds for a total of p seconds, use the following command:

```bash
mishell:$ daemon <t> <p>
```

To stop the daemon, use the kill command with the PID provided by the daemon:

```bash
$ kill <PID>
```

### Cleaning Up
To clean up the compiled files, run:

```bash
$ make clean
```