#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "arena.h"
//TODO: Add Linked List of Blocks




// Initialize an arena with a given capacity
Arena* ArenaInit(u64 capacity) {
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (!arena) return NULL;
    
    arena->memory = (u8*)malloc(capacity);
    if (!arena->memory) {
        free(arena);
        return NULL;
    }
    
    arena->capacity = capacity;
    arena->pos = 0;
    arena->next = NULL;
    return arena;
}

// Clear an arena (reset position to 0)
void ArenaClear(Arena* arena) {
    arena->pos = 0;
}

// Align a number to a power of 2
u64 AlignUp(u64 num, u64 align) {
    return (num + (align - 1)) & ~(align - 1);
}

// Push size bytes onto arena
void* ArenaPush(Arena* arena, u64 size, u64 align) {
    u64 aligned_pos = AlignUp(arena->pos, align);
    if (aligned_pos + size > arena->capacity) {
        return NULL;  // Out of memory
    }
    
    void* result = arena->memory + aligned_pos;
    arena->pos = aligned_pos + size;
    return result;
}

// Push and zero-initialize
void* ArenaPushZero(Arena* arena, u64 size, u64 align) {
    void* result = ArenaPush(arena, size, align);
    if (result) {
        memset(result, 0, size);
    }
    return result;
}


// Temporary arena scope functions
ArenaTemp ArenaTempBegin(Arena* arena) {
    ArenaTemp temp = {arena, arena->pos};
    return temp;
}

void ArenaTempEnd(ArenaTemp temp) {
    temp.arena->pos = temp.pos;
}

// Thread context functions
static ThreadContext* GetThreadContext(void) {
    // TODO: In a real implementation, this would use thread-local storage
    static ThreadContext context;
    return &context;
}

ArenaTemp GetScratch(Arena** conflicts, u64 conflict_count) {
    ThreadContext* ctx = GetThreadContext();
    
    // Initialize scratch arenas if needed
    if (ctx->scratch_count == 0) {
        ctx->scratch_arenas[0] = ArenaInit(1024 * 1024);  // 1MB scratch arena
        ctx->scratch_arenas[1] = ArenaInit(1024 * 1024);  // 1MB scratch arena
        ctx->scratch_count = 2;
    }
    
    // Find a non-conflicting arena
    for (u64 i = 0; i < ctx->scratch_count; i++) {
        bool is_conflict = false;
        for (u64 j = 0; j < conflict_count; j++) {
            if (ctx->scratch_arenas[i] == conflicts[j]) {
                is_conflict = true;
                break;
            }
        }
        if (!is_conflict) {
            return ArenaTempBegin(ctx->scratch_arenas[i]);
        }
    }
    
    // Should never happen with two scratch arenas
    return (ArenaTemp){NULL, 0};
}


void ArenaFree(Arena* arena) {
    if (!arena) return;
    free(arena->memory);
    free(arena);
}