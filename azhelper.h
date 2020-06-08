#pragma once

#include <raylib.h>
#include "azmapparser.h"
#include "controls.h"

#include <malloc.h>

extern Rectangle msgbox;
extern const Rectangle inventoryBorder;
extern Rectangle msgboxtextmargin;

extern Font font1;
extern Font font2;

extern int showDebugInfo;

enum
{
    FACE_DOWN = 0,
    FACE_LEFT,
    FACE_RIGHT,
    FACE_UP
};

typedef struct animation_t
{
    char entStr[16];
    Rectangle frame;

    int OriginX, OriginY;

    int frameCount;
    int frameCounter;

    int currentFrame;
    int speed;
} AzAnimationComponent;

typedef struct azra_t
{
    const char* entStr;
    
    Rectangle frame;
    Vector2 where;
    
    int currentFrame;
    int direction;
    
    int velocity;
} AzP;

typedef struct TextBoxInfo
{
    char* sender;
    char* content;
    
    int shouldDraw;
} TextBoxInfo;

typedef struct GameState
{
    AzP player; 
    AzControls controls;
    AzMap map;
    TextBoxInfo textbox;

    Camera2D camera;

    int paused;
    int controls_locked;
    int cursor_visible;
} AzGameState;

extern void DrawTextBox(TextBoxInfo info);

extern void DrawProp(Texture2D texture, Rectangle rect, Vector2 vec, Color color);

extern void Initialize(AzGameState* state);

extern void Quit();