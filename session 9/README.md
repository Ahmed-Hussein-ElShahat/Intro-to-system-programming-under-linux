# fdisk

`fdisk` is a command-line utility used to parse the partition table of a disk and retrieve all the partition entries. It supports both the MBR (Master Boot Record) and GPT (GUID Partition Table) partitioning schemes.

## Overview

`fdisk` reads the partition table in the MBR to gather information about the disk's partitions. If extended partitions are present, `fdisk` follows the chain to list all partitions, also follows the GPT partitions.

## Usage

To use `fdisk`, simply provide the path to the disk device as an argument:

```bash
./fdisk /dev/sda
```

This command will display all partition entries for the specified disk (`/dev/sda` in this example).

## Features

- **MBR Partitioning**: Parses the MBR partition table to list primary and extended partitions.
- **GPT Partitioning**: Supports GPT partition tables, following the partition entries to retrieve all details.
- **Extended Partitions**: Tracks extended partitions in the MBR, listing all logical partitions within.