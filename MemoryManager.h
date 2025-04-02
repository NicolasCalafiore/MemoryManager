#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <functional>
#include <cstdint>

class MemoryManager {
private:
    struct Block {
        size_t offset;   // In words
        size_t length;   // In words
    };

    unsigned wordSize;
    size_t sizeInWords;
    std::function<int(int, void *)> allocator;
    bool isInitialized;

    uint8_t* memory;

    std::vector<Block> freeBlocks;
    std::vector<Block> allocatedBlocks;

public:
    MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
    ~MemoryManager();
    void initialize(size_t sizeInWords);
    void DEBUG_PRINT();
    void shutdown();
    void *allocate(size_t sizeInBytes);
    void free(void *address);
    void setAllocator(std::function<int(int, void *)> allocator);
    int dumpMemoryMap(char *filename);
    void *getList();
    void *getBitmap();
    unsigned getWordSize();
    void *getMemoryStart();
    unsigned getMemoryLimit();
};

int bestFit(int sizeInWords, void *list);
int worstFit(int sizeInWords, void *list);

#endif