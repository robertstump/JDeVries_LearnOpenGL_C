#ifndef m_SCRATCH_H
#define m_SCRATCH_H

typedef struct {
    char* base;
    size_t size;
    size_t offset;
    size_t previous;
} ScratchArena;

ScratchArena createArena(size_t arena_size) {
    ScratchArena arena;
    arena.base = malloc(arena_size);
    if(!arena.base) {
        fprintf(stderr, "Arena allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    arena.size = arena_size;
    arena.offset = 0;
    return arena;
}

void* arenaAlloc(ScratchArena* arena, size_t alloc_size, size_t alignment) {
    uintptr_t current = (uintptr_t)(arena->base + arena->offset);
    size_t alignment_offset = (alignment - (current % alignment)) % alignment;

    if(arena->offset + alignment_offset + alloc_size > arena->size) {
        fprintf(stderr, "ERROR: Arena overflow!\n");
        return NULL;
    }

    void* ptr = arena->base + arena->offset + alignment_offset;
    arena->offset += alignment_offset + alloc_size;
    return ptr;
}

void resetArena(ScratchArena* arena) {
    arena->offset = 0;
}

void arenaPush(ScratchArena* arena) {
    arena->previous = arena->offset;
}

void arenaPop(ScratchArena* arena) {
    arena->offset = arena->previous;
}

void destroyArena(ScratchArena* arena) {
    free(arena->base);
    arena->base = NULL;
    arena->size = 0;
    arena->offset = 0;
}

#endif
