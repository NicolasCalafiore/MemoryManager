Memory Manager                                                                                                                    
                                                                                                                                    
  Project Description                                                                                                               
                                                                                                                                    
  A C++ static library for custom memory allocation that manages memory in configurable word-sized units. The library supports      
  pluggable allocation strategies and handles memory fragmentation through block splitting and merging.                             
                                                                                                                                    
  Features:                                                                                                                         
  - Word-Based Allocation - Memory managed in configurable word sizes rather than raw bytes                                         
  - Pluggable Allocators - Swap allocation strategies at runtime via function pointers                                              
  - Fragmentation Management - Automatic block merging on deallocation to reduce fragmentation                                      
  - Memory Inspection - Bitmap view, hole list, and memory dump utilities                                                           
                                                                                                                                    
  Allocation Strategies:                                                                                                            
  - Best Fit - Finds the smallest free block that satisfies the request, minimizing wasted space                                    
  - Worst Fit - Uses the largest available block, preserving larger contiguous regions                                              
  - Custom - Supply your own allocation function                                                                                    
                                                                                                                                    
  Screenshots                                                                                                                       
                                                                                                                                    
  Coming soon                                                                                                                       
                                                                                                                                    
  How to Run It                                                                                                                     
                                                                                                                                    
  Prerequisites                                                                                                                     
                                                                                                                                    
  - g++ with C++20 support                                                                                                          
                                                                                                                                    
  Build                                                                                                                             
                                                                                                                                    
  make                                                                                                                              
                                                                                                                                    
  Or compile manually:                                                                                                              
  g++ -c -o MemoryManager.o MemoryManager.cpp -std=c++2a -Wall -I.                                                                  
  ar rcs libMemoryManager.a MemoryManager.o                                                                                         
                                                                                                                                    
  Usage                                                                                                                             
                                                                                                                                    
  Link the static library into your project:                                                                                        
  #include "MemoryManager.h"                                                                                                        
                                                                                                                                    
  // Create manager with word size and allocator strategy                                                                           
  MemoryManager mm(4, bestFit);                                                                                                     
                                                                                                                                    
  // Initialize with number of words                                                                                                
  mm.initialize(1024);                                                                                                              
                                                                                                                                    
  // Allocate memory (in bytes)                                                                                                     
  void* ptr = mm.allocate(64);                                                                                                      
                                                                                                                                    
  // Free memory                                                                                                                    
  mm.free(ptr);                                                                                                                     
                                                                                                                                    
  // Cleanup                                                                                                                        
  mm.shutdown();                                                                                                                    
                                                                                                                                    
  Technologies Used                                                                                                                 
                                                                                                                                    
  - C++                                                                                                                             
  - C++20 Standard                                                                                                                  
  - Make                                                                                                                            
                                                                                                                                    
  What I Learned                                                                                                                    
                                                                                                                                    
  - Memory Fragmentation - Understanding how memory becomes fragmented over time and implementing block merging strategies to       
  coalesce adjacent free regions                                                                                                    
  - Low-Level Memory Management - Working with raw pointers, byte-level memory operations, and tracking allocated vs. free memory   
  regions manually                                                                                                                  
  - Function Pointers - Using function pointers for dependency injection, allowing allocation strategies to be swapped at runtime   
  without modifying core logic                                                                                                      
                                                                     
