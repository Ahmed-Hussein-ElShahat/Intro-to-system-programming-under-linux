
# Heap Memory Management System

This project implements a heap memory management system with features for allocating, reallocating, and freeing memory, while carefully considering the overhead associated with system calls.

## Features

### Heap Allocation
The heap allocation process follows these steps:

1. **Allocate Memory When None is Free**: If there is no available memory in the heap, the system will allocate new memory.
2. **Reuse Free Memory**: If free memory exists, the system will search through it to find a suitable block for allocation.
3. **Allocate New Memory If Needed**: If no suitable free block is found, new memory is allocated.
4. **Extend Last Free Block**: If possible, extend the last free block by adding the remaining required size to it.
5. **Create New Node**: If the last free block isn't at the end of the heap, a new node with a suitable size is created.
6. **Repeat Process**: The process is repeated after adding the suitable size to ensure optimal memory usage.

### Calloc Functionality
The `calloc` function facilitates memory allocation by performing the following steps:

1. **Receive User Input**: It takes from the user the number of members required and the size of each member.
2. **Calculate Total Size**: It computes the total size needed by multiplying the number of members by the size of each member.
3. **Allocate Memory Block**: It returns a pointer to an available memory block that meets the calculated size requirements.
4. **Initialize Memory**: The allocated memory is typically initialized to zero to ensure no residual data is present.

### HmmAlloc Functionality
The `HmmAlloc` function handles basic memory allocation. It is used when memory needs to be allocated without specific initialization, simply finding a suitable block of memory or creating one if none is available.

### Realloc Functionality
The `realloc` function manages dynamic memory reallocation through these steps:

1. **Check Pointer Status**:
   - **Null Pointer**: If the provided pointer is `NULL`, `realloc` behaves similarly to `HmmAlloc` by allocating new memory of the specified size.
   - **Non-Null Pointer**: If the pointer is not `NULL`, it first attempts to free the existing memory block.
2. **Attempt to Allocate Adjacent Memory**: It tries to allocate additional memory adjacent to the existing block to accommodate the new size.
3. **Preserve Data**: `realloc` ensures that the content from the original memory block is preserved up to the lesser of the old and new sizes.
4. **Return New Pointer**: It returns a pointer to the newly allocated memory block, which may be the same as the original pointer or a new location if relocation was necessary.

### Free Memory
To manage free memory blocks and track their locations, a doubly linked list is used. The system follows these steps to free memory:

1. **Node Struct**:
    - `size`: Represents the size of the node (starting after the size variable).
    - `next`: Stores the address of the next node.
    - `prev`: Stores the address of the previous node.

2. **Steps to Free Memory**:
    1. **Check for Free Blocks**: If no free blocks exist, the new node becomes the head of the list.
    2. **Locate Position for New Node**: The system loops through all nodes to find the correct position for the new node.
    3. **Update Head Node**: If the new node is positioned before the current head, it becomes the new head.
    4. **Insert Node in Proper Place**: Otherwise, the new node is placed in the appropriate position.
    5. **Merge Contiguous Nodes**: The system merges nodes together if they are contiguous in memory.
    6. **Adjust Program End**: The program checks if the heap's end can be lowered to release unused memory.

## Summary
This heap memory management system provides efficient and flexible memory operations by implementing custom allocation (`HmmAlloc`, `calloc`), reallocation (`realloc`), and freeing mechanisms. It optimizes memory usage and reduces system call overhead by intelligently managing memory blocks through a doubly linked list structure and strategic allocation strategies.
