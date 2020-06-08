#pragma once

#include <raylib.h>

#include "azhelper.h"
#include "controls.h"

#include "azmapparser.h"

#define MAX_MOVEMENT_SPEED 1

#define MOVEMENT_MAX_X 320
#define MOVEMENT_MIN_X -320

#define MOVEMENT_MAX_Y 240
#define MOVEMENT_MIN_Y -144

extern int showDebugInfo;
extern AzGameState state;

extern char* GetAzPDirectionString(AzP* player);
extern void UpdateAzP(AzP* player, int* frameCounter, int speed);
extern void MoveAzP(AzP* player, AzControls controls);
extern void DrawAzP(Texture2D texture, AzP* player);

extern void UpdateAzPVel(AzP* player, AzControls controls);

extern void DoCollisionsAzP(AzP* player, Rectangle* Collisions, int collisionsCount);
extern void DoEvCollisionsAzP(AzP* player, AzEventTrigger* EventCollisions, int eventCount);

extern void AzUpdateCamera(AzP* player);

extern AzAnimationComponent AzCreateAnim(char* animName, int originx, int originy, int width, int height, int speed, int frameCount);

extern void AzUpdateAnim(AzAnimationComponent* comp);

