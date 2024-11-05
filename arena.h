#include <stdint.h>
// Type definitions
typedef uint8_t  u8;
typedef uint64_t u64;
typedef size_t   usize;

// Basic arena structure
typedef struct Arena {
    u8* memory;
    u64 capacity;
    u64 pos;
    struct Arena* next; // For linked list of blocks if needed
} Arena;

// Temporary arena scope
typedef struct ArenaTemp {
    Arena* arena;
    u64 pos;
} ArenaTemp;

// Thread scratch arena context
typedef struct ThreadContext {
    Arena* scratch_arenas[2];  // Two scratch arenas per thread
    u64 scratch_count;
} ThreadContext;

// Initialize an arena with a given capacity
Arena* ArenaInit(u64 capacity);

// Clear an arena (reset position to 0)
void ArenaClear(Arena* arena);

// Align a number to a power of 2
u64 AlignUp(u64 num, u64 align);

// Push size bytes onto arena
void* ArenaPush(Arena* arena, u64 size, u64 align);

// Push and zero-initialize
void* ArenaPushZero(Arena* arena, u64 size, u64 align);

// Convenience macro for pushing structs
#define PushStruct(arena, type) (type*)ArenaPush(arena, sizeof(type), _Alignof(type))
#define PushStructZero(arena, type) (type*)ArenaPushZero(arena, sizeof(type), _Alignof(type))

// Temporary arena scope functions
ArenaTemp ArenaTempBegin(Arena* arena);
void ArenaTempEnd(ArenaTemp temp);

// Thread context functions
static ThreadContext* GetThreadContext(void);
ArenaTemp GetScratch(Arena** permanent, u64 index);

#define ReleaseScratch(temp) ArenaTempEnd(temp)

// Free an arena and all of its memory
void ArenaFree(Arena* arena);