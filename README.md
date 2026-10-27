xv6 Virtual Memory and Page Replacement Assignment
This repository contains the implementation of advanced virtual memory extensions, lazy memory allocation, page fault handling, and page replacement algorithm simulations built on top of the xv6 operating system (x86 architecture).

Features & Implementation Overview
1. Page Table Inspection (vmprint)
System Call (vmprint): Traverses the calling process's two-level page directory below KERNBASE. It prints valid virtual-to-physical address mappings along with protection flags (P, W, and U).

Summary Metrics: Outputs the total count of valid user pages and page-table pages used by the process.

Test Program (pttest.c): Validates memory layout changes by calling vmprint() at startup and immediately after expanding the heap using sbrk(4 * 4096).

2. Lazy Page Allocation & Page Fault Handling
Lazy sbrk: Modified sys_sbrk() so that positive allocations only increment myproc()->sz without eagerly allocating physical memory frames.

Trap Interception (T_PGFLT): Handled page faults in the kernel using rcr2(). If the faulting address is within valid process bounds, the kernel dynamically allocates a zeroed frame via kalloc() and memset(), maps it using mappages() with PTE_W | PTE_U permissions, and increments a process fault counter.

Test Program (faulttest.c): Allocates memory for 32 pages lazily, writes to 5 distinct pages, verifies that the getfaults() system call returns 5, and demonstrates that out-of-bounds accesses safely terminate only the offending process.

3. Page Replacement Algorithms (pagerepl)
Simulation Framework: A user-space program that models physical memory frames to evaluate First-In, First-Out (FIFO) and Least Recently Used (LRU) page replacement policies.

Analysis & Output: Displays frame contents and hit/fault status for every reference in an input string, and outputs a summary comparison table of page faults and hit ratios across 3, 4, and 5 frames.

Environment Prerequisites
To build and run this project, ensure you have a Linux environment (such as WSL on Windows) installed with the required xv6 toolchain packages:

Bash
sudo apt update
sudo apt install -y build-essential qemu-system-x86 gcc-multilib make gdb
Building and Running xv6
Clone or open your xv6 project directory in your terminal.

Compile and launch xv6 using QEMU:

Bash
make clean
make qemu
To exit the QEMU emulator, press Ctrl + A, then press x.

Running the Test Programs
Once the xv6 shell boots up (init: starting sh), you can run the implemented test programs directly:

Test Page Table Printing:

Bash
$ pttest
Test Lazy Allocation & Page Faults:

Bash
$ faulttest
Run Page Replacement Simulation:

Bash
$ pagerepl
(You can also pass custom inputs, e.g., pagerepl 3 7,0,1,2,0,3)

File Modifications Summary
vm.c: Implemented vmprint(), and adjusted deallocuvm() and copyuvm() to gracefully skip unallocated pages.

sysproc.c: Updated sys_sbrk() for lazy allocation and added sys_getfaults().

trap.c: Added the T_PGFLT page fault handler.

proc.h: Added a fault counter (numfaults) field to struct proc.

syscall.h / syscall.c / user.h / usys.S / defs.h: Registered system calls (vmprint, getfaults).

Makefile: Added _pttest, _faulttest, and _pagerepl to UPROGS.
