#pragma once

#define AZMAP_PARSER

#include "ext/asm-xml.h"
#include <stdio.h>
#include <raylib.h>
#include <string.h>
#include <malloc.h>

#include <stdlib.h>

typedef struct eventtrigger_t
{
    char* name[16];

    Rectangle bounds;
    int hasBeenTriggered;
    char script[1024];
} AzEventTrigger;

typedef struct collidelist_t
{
    int count;
    Rectangle list[128];
} AzColliderList;

typedef struct evcollidelist_t
{
    int count;
    AzEventTrigger list[64];
} AzEventColliderList;

typedef struct interact_collide_t
{
    char* name[16];

    Rectangle bounds;
    
    int req_face;
    char script[1024];

} AzInteractableCollider;

typedef struct interact_collidelist_t
{
    int count;
    AzInteractableCollider list[64];
} AzInteractableColliderList;

typedef struct map_t
{
    char name[32];
    char author[32];

    char pathToSheet[64];
    char startScript[64];

    AzColliderList collisions;
    AzEventColliderList eventCollisions;
    AzInteractableColliderList interactCollisions;
} AzMap;

AzMap AzLoadMap(char* filename);
void AzUnloadMap(AzMap* map);

const char* AzXMLGetErrorString(int responseCode);