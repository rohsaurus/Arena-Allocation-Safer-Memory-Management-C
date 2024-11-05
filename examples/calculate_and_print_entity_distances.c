#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../arena.h"

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    int id;
    char name[32];
    Vector3 position;
    float health;
} Entity;

int main() {
    // Initialize permanent arena (1MB)
    Arena* permanent = ArenaInit(1024*1024);
    if (!permanent) {
        printf("Failed to initialize permanent arena\n");
        return 1;
    }

    // Allocate an array of entities permanently
    int entity_count = 100;
    Entity* entities = ArenaPushZero(permanent, sizeof(Entity) * entity_count, 8);
    if (!entities) {
        printf("Failed to allocate entities\n");
        ArenaFree(permanent);
        return 1;
    }

    // Initialize some entities
    for (int i = 0; i < entity_count; i++) {
        entities[i].id = i;
        snprintf(entities[i].name, 32, "Entity_%d", i);
        entities[i].position = (Vector3){i * 1.0f, 0, 0};
        entities[i].health = 100.0f;
    }

    // Use scratch arena for temporary calculations
    ArenaTemp scratch = GetScratch(&permanent, 1);
    if (!scratch.arena) {
        printf("Failed to get scratch arena\n");
        ArenaFree(permanent);
        return 1;
    }

    // Temporary array for distance calculations
    float* distances = ArenaPush(scratch.arena, sizeof(float) * entity_count, 8);
    if (!distances) {
        printf("Failed to allocate distances\n");
        ArenaFree(permanent);
        return 1;
    }

    // Calculate distances from origin
    for (int i = 0; i < entity_count; i++) {
        Vector3 pos = entities[i].position;
        distances[i] = sqrtf(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
    }

    // Print some results
    printf("First 5 entities and their distances:\n");
    for (int i = 0; i < 5; i++) {
        printf("Entity %d ('%s') at position (%.1f, %.1f, %.1f) - Distance: %.1f\n",
            entities[i].id,
            entities[i].name,
            entities[i].position.x,
            entities[i].position.y,
            entities[i].position.z,
            distances[i]);
    }

    // Release temporary memory
    ArenaTempEnd(scratch);

    // Clean up permanent arena
    ArenaFree(permanent);
    return 0;
}