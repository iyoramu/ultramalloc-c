MIT License

Copyright (c) 2025 IRUTABYOSE Yoramu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

// ======================
//  Modern UI/UX Components
// ======================

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    bool freed;
    time_t alloc_time;
    const char* type;
} AllocationInfo;

typedef struct {
    AllocationInfo* entries;
    size_t size;
    size_t capacity;
    pthread_mutex_t lock;
} AllocationTracker;

AllocationTracker tracker = {0};

void init_tracker() {
    tracker.capacity = 1024;
    tracker.entries = malloc(sizeof(AllocationInfo) * tracker.capacity);
    tracker.size = 0;
    pthread_mutex_init(&tracker.lock, NULL);
}

void add_allocation(void* ptr, size_t size, const char* file, int line, const char* type) {
    pthread_mutex_lock(&tracker.lock);

    if (tracker.size >= tracker.capacity) {
        tracker.capacity *= 2;
        tracker.entries = realloc(tracker.entries, sizeof(AllocationInfo) * tracker.capacity);
    }

    tracker.entries[tracker.size] = (AllocationInfo){
        .ptr = ptr,
        .size = size,
        .file = file,
        .line = line,
        .freed = false,
        .alloc_time = time(NULL),
        .type = type
    };
    tracker.size++;

    pthread_mutex_unlock(&tracker.lock);
}

void mark_freed(void* ptr) {
    pthread_mutex_lock(&tracker.lock);

    for (size_t i = 0; i < tracker.size; i++) {
        if (tracker.entries[i].ptr == ptr && !tracker.entries[i].freed) {
            tracker.entries[i].freed = true;
            break;
        }
    }

    pthread_mutex_unlock(&tracker.lock);
}

void print_memory_report() {
    pthread_mutex_lock(&tracker.lock);

    printf(ANSI_COLOR_CYAN "\n===== MEMORY ALLOCATION REPORT =====\n" ANSI_COLOR_RESET);
    printf("Total allocations tracked: %zu\n", tracker.size);

    size_t active = 0;
    size_t leaked = 0;
    size_t total_allocated = 0;

    for (size_t i = 0; i < tracker.size; i++) {
        if (!tracker.entries[i].freed) {
            leaked += tracker.entries[i].size;
            printf(ANSI_COLOR_RED "LEAK: %zu bytes at %p allocated in %s:%d (%s)\n" ANSI_COLOR_RESET,
                   tracker.entries[i].size, tracker.entries[i].ptr,
                   tracker.entries[i].file, tracker.entries[i].line,
                   tracker.entries[i].type);
        }
        total_allocated += tracker.entries[i].size;
    }

    printf("\nSummary:\n");
    printf("Total memory allocated: %zu bytes\n", total_allocated);
    printf("Potential leaks: %zu bytes\n", leaked);
    printf(ANSI_COLOR_CYAN "===================================\n\n" ANSI_COLOR_RESET);

    pthread_mutex_unlock(&tracker.lock);
}

// ======================
//  Memory Management Core
// ======================

#define MIN_BLOCK_SIZE 16
#define MAX_HEAP_SIZE (1 << 30) // 1GB
#define ALIGNMENT 8

typedef struct Block {
    size_t size;
    struct Block* next;
    bool free;
} Block;

static Block* heap_start = NULL;
static pthread_mutex_t heap_lock = PTHREAD_MUTEX_INITIALIZER;

size_t align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

Block* request_from_os(size_t size) {
    size_t total_size = align_size(size + sizeof(Block));
    void* block = mmap(NULL, total_size,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS,
                      -1, 0);

    if (block == MAP_FAILED) {
        return NULL;
    }

    Block* new_block = (Block*)block;
    new_block->size = total_size - sizeof(Block);
    new_block->next = NULL;
    new_block->free = false;

    return new_block;
}

void split_block(Block* block, size_t size) {
    size_t remaining_size = block->size - size - sizeof(Block);
    if (remaining_size >= MIN_BLOCK_SIZE) {
        Block* new_block = (Block*)((char*)block + sizeof(Block) + size);
        new_block->size = remaining_size;
        new_block->next = block->next;
        new_block->free = true;
        block->next = new_block;
        block->size = size;
    }
}

Block* find_best_fit(size_t size) {
    Block* current = heap_start;
    Block* best_fit = NULL;

    while (current) {
        if (current->free && current->size >= size) {
            if (!best_fit || current->size < best_fit->size) {
                best_fit = current;
                if (best_fit->size == size) {
                    break; // perfect fit
                }
            }
        }
        current = current->next;
    }

    return best_fit;
}

void merge_free_blocks() {
    Block* current = heap_start;

    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// ======================
//  Public Interface
// ======================

void* custom_malloc(size_t size, const char* file, int line) {
    if (size == 0) {
        return NULL;
    }

    pthread_mutex_lock(&heap_lock);

    size_t aligned_size = align_size(size);

    if (heap_start == NULL) {
        heap_start = request_from_os(aligned_size);
        if (heap_start == NULL) {
            pthread_mutex_unlock(&heap_lock);
            return NULL;
        }
    }

    Block* block = find_best_fit(aligned_size);

    if (block) {
        block->free = false;
        split_block(block, aligned_size);
    } else {
        block = request_from_os(aligned_size);
        if (!block) {
            pthread_mutex_unlock(&heap_lock);
            return NULL;
        }

        // Add to the end of the list
        Block* last = heap_start;
        while (last->next) {
            last = last->next;
        }
        last->next = block;
    }

    pthread_mutex_unlock(&heap_lock);

    void* ptr = (void*)((char*)block + sizeof(Block));
    add_allocation(ptr, size, file, line, "malloc");
    return ptr;
}

void* custom_calloc(size_t num, size_t size, const char* file, int line) {
    size_t total_size = num * size;
    void* ptr = custom_malloc(total_size, file, line);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void* custom_realloc(void* ptr, size_t size, const char* file, int line) {
    if (!ptr) {
        return custom_malloc(size, file, line);
    }

    if (size == 0) {
        custom_free(ptr, file, line);
        return NULL;
    }

    pthread_mutex_lock(&heap_lock);

    Block* block = (Block*)((char*)ptr - sizeof(Block));
    size_t aligned_size = align_size(size);

    // Check if we can just expand in place
    if (block->size >= aligned_size) {
        split_block(block, aligned_size);
        pthread_mutex_unlock(&heap_lock);
        return ptr;
    }

    // Check if next block is free and combining gives enough space
    if (block->next && block->next->free &&
        (block->size + sizeof(Block) + block->next->size) >= aligned_size) {
        block->size += sizeof(Block) + block->next->size;
        block->next = block->next->next;
        split_block(block, aligned_size);
        pthread_mutex_unlock(&heap_lock);
        return ptr;
    }

    pthread_mutex_unlock(&heap_lock);

    // Need to allocate new space and copy
    void* new_ptr = custom_malloc(size, file, line);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size < size ? block->size : size);
        custom_free(ptr, file, line);
    }

    return new_ptr;
}

void custom_free(void* ptr, const char* file, int line) {
    if (!ptr) {
        return;
    }

    pthread_mutex_lock(&heap_lock);

    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->free = true;

    merge_free_blocks();

    pthread_mutex_unlock(&heap_lock);

    mark_freed(ptr);
}

// ======================
//  Advanced Features
// ======================

void memory_stats() {
    pthread_mutex_lock(&heap_lock);

    size_t total_memory = 0;
    size_t free_memory = 0;
    size_t used_memory = 0;
    size_t block_count = 0;
    size_t free_blocks = 0;

    Block* current = heap_start;
    while (current) {
        total_memory += current->size + sizeof(Block);
        if (current->free) {
            free_memory += current->size;
            free_blocks++;
        } else {
            used_memory += current->size;
        }
        block_count++;
        current = current->next;
    }

    printf(ANSI_COLOR_YELLOW "\n===== MEMORY STATISTICS =====\n" ANSI_COLOR_RESET);
    printf("Total heap memory: %zu bytes\n", total_memory);
    printf("Used memory: %zu bytes\n", used_memory);
    printf("Free memory: %zu bytes\n", free_memory);
    printf("Total blocks: %zu\n", block_count);
    printf("Free blocks: %zu\n", free_blocks);
    printf("Fragmentation: %.2f%%\n",
           block_count > 0 ? (float)free_blocks/block_count * 100 : 0);
    printf(ANSI_COLOR_YELLOW "=============================\n\n" ANSI_COLOR_RESET);

    pthread_mutex_unlock(&heap_lock);
}

void memory_defrag() {
    pthread_mutex_lock(&heap_lock);

    printf(ANSI_COLOR_MAGENTA "Running memory defragmentation...\n" ANSI_COLOR_RESET);

    // Simple defragmentation by merging adjacent free blocks
    merge_free_blocks();

    // More advanced defrag would involve moving blocks around
    // but that's complex without proper memory handles

    pthread_mutex_unlock(&heap_lock);

    memory_stats();
}

// ======================
//  Initialization/Cleanup
// ======================

__attribute__((constructor))
void init_memory_manager() {
    init_tracker();
    printf(ANSI_COLOR_GREEN "Custom Memory Manager Initialized\n" ANSI_COLOR_RESET);
}

__attribute__((destructor))
void cleanup_memory_manager() {
    print_memory_report();

    // Free all allocated blocks
    pthread_mutex_lock(&heap_lock);
    Block* current = heap_start;
    while (current) {
        Block* next = current->next;
        munmap(current, current->size + sizeof(Block));
        current = next;
    }
    heap_start = NULL;
    pthread_mutex_unlock(&heap_lock);

    // Free tracker
    free(tracker.entries);
    pthread_mutex_destroy(&tracker.lock);
    pthread_mutex_destroy(&heap_lock);

    printf(ANSI_COLOR_GREEN "Custom Memory Manager Cleaned Up\n" ANSI_COLOR_RESET);
}

// ======================
//  Macros for Easy Use
// ======================

#define malloc(size) custom_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) custom_calloc(num, size, __FILE__, __LINE__)
#define realloc(ptr, size) custom_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) custom_free(ptr, __FILE__, __LINE__)
