
# ls Command Utility

## Introduction
The `ls` command is a Unix/Linux command-line utility that displays the contents of a directory and/or shows details about the files and directories, it can take more than one argument(if no arguments given, the default is the current directory). It lists files and directories in alphabetical order by default, but it can also be customized to display additional information and change the order based on user preferences.

## Features
The `ls` command provides several options to customize its behavior:

- Files and directories are displayed in alphabetical order by default, with color-coded file types for easier identification.
  
### Available Commands
- **`-l`**: Displays contents in long format, including detailed file information (permissions, owner, size, modification date).
- **`-a`**: Displays hidden files (files that start with a dot `.`).
- **`-f`**: Lists the directories in the same order as in the directory table, without sorting.
- **`-i`**: Displays the inode number of each file.
- **`-d`**: Displays information about the directory itself, rather than its contents.
- **`-t`**: Sorts files by modification time, showing the most recently modified files first.
- **`-u`**: Sorts files by access time.
- **`-c`**: Sorts files by change time.
- **`-1`**: Displays one file per line for a cleaner, minimal display.

## Usage Examples
Here are some common examples of how to use the `ls` command:

- Display contents in long format:
  ```bash
  ls -l
  ```
  
- Show hidden files:
  ```bash
  ls -a
  ```
  
- Display inode numbers:
  ```bash
  ls -i
  ```
  
- Sort by modification time:
  ```bash
  ls -t
  ```
