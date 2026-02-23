# MemoryManager

A custom C++ memory manager that allocates from a contiguous memory pool using pluggable allocation strategies and explicit free-list tracking.

## Project Overview

This project demonstrates low-level systems programming fundamentals through a manually managed allocator. Memory is handled in fixed-size words, with allocation and deallocation operations updating internal block metadata in real time.

## Technical Highlights

- Built a dedicated memory pool with explicit lifecycle control (`initialize` / `shutdown`)
- Implemented word-based allocation with byte-to-word rounding
- Designed free-block and allocated-block tracking using structured metadata
- Added block splitting on allocation and adjacent-hole coalescing on free
- Supported multiple placement policies through strategy injection (best-fit, worst-fit, and custom allocator callbacks)
- Exposed memory-state introspection through hole lists, allocation bitmaps, and memory map dumps
- Implemented a broad command-line test suite that validates allocator behavior across simple and complex allocation patterns

## Core Components

- `MemoryManager.h`: public interface and allocator strategy signatures
- `MemoryManager.cpp`: allocator logic, block management, bitmap/list generation, and strategy implementations
- `CommandLineTest.cpp`: scenario-based validation for allocator correctness and behavior under varied workloads

## Lessons Learned

- **Memory layout modeling:** translating high-level allocation requests into fixed-word offsets and sizes inside a raw byte buffer
- **Pointer arithmetic and boundaries:** safely computing offsets, limits, and address conversions when managing manual allocations
- **Fragmentation handling:** balancing allocation policy choices while controlling fragmentation through split/merge mechanics
- **Data-structure consistency:** maintaining correctness between free and allocated block collections across repeated allocate/free cycles
