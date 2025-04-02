#include <functional>
#include <string>
#include <utility>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>
#include "MemoryManager.h"

using namespace std;

    MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator) {
        bool isDebug = false;
        //wordSize --> size of 1 WORD in BYTES
        this->wordSize = wordSize;
        this->allocator = std::move(allocator);
        if(isDebug) cout << "MemoryManager created with word size: " << wordSize << endl << endl;
    }
    MemoryManager::~MemoryManager() {
        shutdown();
    }


    void MemoryManager::initialize(size_t sizeInWords) {
        bool isDebug = false;
        //sizeInWords --> size of memory in WORDS
        //sizeInWords * wordSize --> actual size of memory in bytes
        this->sizeInWords = sizeInWords;

        memory = new uint8_t[sizeInWords * wordSize];

        //Do I need this?
        for (int i = 0; i < sizeInWords * wordSize; i++)
            memory[i] = 0;

        freeBlocks.clear();
        allocatedBlocks.clear();
        Block initialBlock = {0, sizeInWords};
        freeBlocks.push_back(initialBlock);
        isInitialized = true;

        if (isDebug){
            cout << "MemoryManager initialized with word size: " << sizeInWords << " words" << endl;
            cout << "MemoryManager initialized with byte size: " << sizeInWords * wordSize << " bytes" << endl;
            cout << "Initial Memory Block: offset:" << initialBlock.offset << " - Length:" << initialBlock.length << endl;
            std::cout << std::endl;
        }
    }

    void MemoryManager::DEBUG_PRINT() {
        cout << "MemoryManager DEBUG_PRINT" << endl;
        cout << "Word Size: " << wordSize << endl;
        cout << "Size in Words: " << sizeInWords << endl;
        cout << "Free Blocks:" << endl;
        for (const auto& block : freeBlocks) {
            cout << "Offset: " << block.offset << ", Length: " << block.length << endl;
        }

        cout << "Memory Blocks (0=free, 1=allocated):" << endl;
        for (size_t i = 0; i < sizeInWords; i++) {
            bool isFree = false;
            for (const auto& block : freeBlocks) {
                if (i >= block.offset && i < block.offset + block.length) {
                    isFree = true;
                    break;
                }
            }
            cout << "Word " << i << ": " << (isFree ? "FREE" : "ALLOCATED") << endl;
        }

        std::cout << std::endl;
    }

    void MemoryManager::shutdown() {
        bool isDebug = false;
        if (isInitialized) {
            delete[] memory;
            memory = nullptr;
            freeBlocks.clear();
            allocatedBlocks.clear();
            isInitialized = false;
            if(isDebug) std::cout << "Memory manager shutdown" << std::endl;
        }
    }
    void *MemoryManager::allocate(size_t sizeInBytes) {
        bool isDebug = false;
        if (!isInitialized || sizeInBytes == 0)
            return nullptr;

        size_t sizeInWords = (sizeInBytes + wordSize - 1) / wordSize;

        if (freeBlocks.empty()) {
            if(isDebug) std::cout << "Allocation failed: No free blocks available" << std::endl;
            return nullptr;
        }

        uint16_t* list = static_cast<uint16_t*>(getList());
        if (list == nullptr)
            return nullptr;

        int wordOffset = allocator(sizeInWords, list);

        if (wordOffset < 0) {
            delete[] list;
            if(isDebug) std::cout << "Allocation failed: No suitable block found" << std::endl;
            return nullptr;
        }

        int blockIndex = -1;
        for (size_t i = 0; i < freeBlocks.size(); i++) {
            if (freeBlocks[i].offset == wordOffset) {
                blockIndex = i;
                break;
            }
        }

        if (blockIndex < 0) {
            delete[] list;
            if(isDebug) std::cout << "Allocation failed: No block found at offset " << wordOffset << std::endl;
            return nullptr;
        }

        Block selectedBlock = freeBlocks[blockIndex];

        if (selectedBlock.length < sizeInWords) {
            delete[] list;
            if(isDebug) std::cout << "Allocation failed: Selected block too small" << std::endl;
            return nullptr;
        }

        void* allocatedMemory = memory + (selectedBlock.offset * wordSize);

        Block newAllocatedBlock = {selectedBlock.offset, sizeInWords};
        allocatedBlocks.push_back(newAllocatedBlock);

        //If block selected is larger than necessary
        if (selectedBlock.length > sizeInWords) {
            //Split block
            freeBlocks[blockIndex].offset += sizeInWords;
            freeBlocks[blockIndex].length -= sizeInWords;
        } else {
            //Block is correct size, erase from freeBlocks.
            freeBlocks.erase(freeBlocks.begin() + blockIndex);
        }

        delete[] list;
        if(isDebug)
            std::cout << "Allocated " << sizeInBytes << " bytes (" << sizeInWords << " words) at offset "
                      << selectedBlock.offset << std::endl << std::endl;

        return allocatedMemory;
    }

    void MemoryManager::free(void *address) {
        bool isDebug = false;
        if (!isInitialized || address == nullptr) {
            if(isDebug) std::cout << "Free failed: Memory manager not initialized or address is null" << std::endl;
            return;
        }


        size_t offsetInBytes = static_cast<uint8_t*>(address) - memory;
        size_t offsetInWords = offsetInBytes / wordSize;

        if (offsetInWords >= sizeInWords) {
            if(isDebug) std::cout << "Free failed: Address outside of managed memory" << std::endl;
            return;
        }

        int blockIndex = -1;

        //Finds index of block that correlates to requested offset --> -1 = Failed
        for (size_t i = 0; i < allocatedBlocks.size(); i++)
            if (allocatedBlocks[i].offset == offsetInWords) {
                blockIndex = static_cast<int>(i);
                break;
            }

        if (blockIndex == -1) {
            if(isDebug) std::cout << "Free failed: No allocated block found at this address" << std::endl;
            return;
        }


        Block blockToFree = allocatedBlocks[blockIndex];
        Block newFreeBlock = {blockToFree.offset, blockToFree.length};
        allocatedBlocks.erase(allocatedBlocks.begin() + blockIndex);
        freeBlocks.push_back(newFreeBlock);

        std::sort(freeBlocks.begin(), freeBlocks.end(),
                  [](const Block& a, const Block& b) { return a.offset < b.offset; });

        //Merges adjacent blocks
        for (size_t i = 0; i < freeBlocks.size() - 1;) {
            if (freeBlocks[i].offset + freeBlocks[i].length == freeBlocks[i + 1].offset) {
                //Adjacent --> MErge
                freeBlocks[i].length += freeBlocks[i + 1].length;
                freeBlocks.erase(freeBlocks.begin() + i + 1);

            } else
                i++;
        }

        if(isDebug)
            std::cout << "Freed memory at word offset " << offsetInWords
                      << " (length: " << blockToFree.length << " words)" << std::endl;
    }

    void MemoryManager::setAllocator(std::function<int(int, void *)> allocator) {
        bool isDebug = false;
        this->allocator = std::move(allocator);
        if(isDebug) std::cout << "Allocator function changed" << std::endl << endl;
    }

    //[START, LENGTH] - [START, LENGTH] … (hole list)
    int MemoryManager::dumpMemoryMap(char *filename) {
        FILE* file = fopen(filename, "w");
        if (!file)
            return -1;

        uint16_t* list = static_cast<uint16_t*>(getList());
        if (!list) {
            fclose(file);
            return -1;
        }

        uint16_t numHoles = list[0];

        if (numHoles > 0) {
            fprintf(file, "[%u, %u]", list[1], list[2]);

            for (uint16_t i = 1; i < numHoles; i++)
                fprintf(file, " - [%u, %u]", list[2*i+1], list[2*i+2]);
    }

        delete[] list;
        fclose(file);
        return 0;
    }

    //[numHoles, offset1, length1, offset2, length2, ...]
    void *MemoryManager::getList() {
        bool isDebug = false;
        if (!isInitialized || freeBlocks.empty())
            return nullptr;

        size_t listSize = 1 + (2 * freeBlocks.size());
        uint16_t* holeList = new uint16_t[listSize];

        holeList[0] = freeBlocks.size();

        for (size_t i = 0; i < freeBlocks.size(); i++) {
            holeList[1 + (i * 2)] = freeBlocks[i].offset;
            holeList[1 + (i * 2) + 1] = freeBlocks[i].length;
        }

        if(isDebug) {
            std::cout << "Hole List: " << holeList[0];
            for (size_t i = 1; i < listSize; i++) {
                std::cout << ", " << holeList[i];
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        return holeList;
    }

    //[0,10]-[12,2]-[20,6] --> [00000011111100110000000000] --> [0x04,0x00,0x00,0xCC,0x0F,0x00]
    void *MemoryManager::getBitmap() {
        bool isDebug = false;
        if (!isInitialized)
            return nullptr;


        size_t bitmapSizeInBytes = (sizeInWords + 7) / 8;
        size_t totalSizeInBytes = 2 + bitmapSizeInBytes;

        uint8_t* bitmap = new uint8_t[totalSizeInBytes];

        bitmap[0] = bitmapSizeInBytes % 256;  // Lower byte
        bitmap[1] = bitmapSizeInBytes / 256;  // Upper byte

        for (size_t i = 2; i < totalSizeInBytes; i++)
            bitmap[i] = 0;

        for (const auto& block : allocatedBlocks) {
            for (size_t wordOffset = block.offset; wordOffset < block.offset + block.length; wordOffset++) {
                if (wordOffset >= sizeInWords) continue;

                size_t byteIndex = wordOffset / 8;
                size_t bitPosition = wordOffset % 8;

                uint8_t bitValues[8] = {1, 2, 4, 8, 16, 32, 64, 128};
                bitmap[2 + byteIndex] = bitmap[2 + byteIndex] + bitValues[bitPosition];
            }
        }

        if (isDebug) {
            std::cout << "Bitmap: [";
            for (size_t i = 0; i < totalSizeInBytes; i++) {
                if (i > 0) std::cout << ", ";
                std::cout << "0x" << std::hex << static_cast<int>(bitmap[i]);
            }
            std::cout << std::dec << "]" << std::endl;
        }

        return bitmap;
    }

    unsigned MemoryManager::getWordSize() {
        if (!isInitialized)
            return 0;

        return wordSize;
    }

    void *MemoryManager::getMemoryStart() {
        if (!isInitialized)
            return nullptr;

        return memory;
    }
    unsigned MemoryManager::getMemoryLimit() {
        if (!isInitialized)
            return 0;

        return sizeInWords * wordSize;
    }



int bestFit(int sizeInWords, void *list) {
    if (list == nullptr || sizeInWords <= 0)
        return -1;


    uint16_t* holeList = static_cast<uint16_t*>(list);
    uint16_t numHoles = holeList[0];

    if (numHoles == 0)
        return -1;


    int bestFitOffset = -1;
    size_t bestFitSize = SIZE_MAX;

    for (uint16_t i = 0; i < numHoles; i++) {
        uint16_t offset = holeList[1 + (i * 2)];
        uint16_t length = holeList[1 + (i * 2) + 1];

        if (length >= sizeInWords) {
            if (length < bestFitSize) {
                bestFitSize = length;
                bestFitOffset = offset;
            }
        }
    }

    return bestFitOffset;
}

int worstFit(int sizeInWords, void *list) {
    if (list == nullptr || sizeInWords <= 0) {
        return -1;
    }

    uint16_t* holeList = static_cast<uint16_t*>(list);
    uint16_t numHoles = holeList[0];

    if (numHoles == 0) {
        return -1;
    }

    int worstFitOffset = -1;
    size_t worstFitSize = 0;

    for (uint16_t i = 0; i < numHoles; i++) {
        uint16_t offset = holeList[1 + (i * 2)];
        uint16_t length = holeList[1 + (i * 2) + 1];

        if (length >= sizeInWords) {
            if (length > worstFitSize) {
                worstFitSize = length;
                worstFitOffset = offset;
            }
        }
    }

    return worstFitOffset;
}
