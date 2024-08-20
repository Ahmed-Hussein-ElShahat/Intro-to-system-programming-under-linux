# Heap Memory Management System

This project implements a heap memory management system with two primary features: allocating and freeing memory, while carefully considering the overhead associated with system calls.

## Features

### Heap Allocation
The heap allocation process follows these steps:

1. **Allocate Memory When None is Free**: If there is no available memory in the heap, the system will allocate new memory.
2. **Reuse Free Memory**: If free memory exists, the system will search through it to find a suitable block for allocation.
3. **Allocate New Memory If Needed**: If no suitable free block is found, new memory is allocated.
4. **Extend Last Free Block**: If possible, extend the last free block by adding the remaining required size to it.
5. **Create New Node**: If the last free block isn't at the end of the heap, a new node with a suitable size is created.
6. **Repeat Process**: The process is repeated after adding the suitable size to ensure optimal memory usage.

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
