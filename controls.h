#pragma once

#include <raylib.h>

typedef struct controls_t
{
    int up, down, left, right, sprint;
    
    int showInv;
} AzControls;


void AzUpdateControls(AzControls* controls);