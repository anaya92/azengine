#include "azhelper.h"

Font font1;
Font font2;

Rectangle msgbox = { 10, 10, 620, 80 };
const Rectangle inventoryBorder = { 5, 96, 630, 2 };
Rectangle msgboxtextmargin = { 24, 38, 606, 75 };

void DrawTextBox(TextBoxInfo info)
{
    DrawRectangleRounded(msgbox, 0.1f, 1, (Color) { 230, 138, 119 });
    DrawRectangleRoundedLines(msgbox, 0.2f, 1, 2, RAYWHITE);
    
    DrawTextEx(font1, info.sender, (Vector2) { msgbox.x + 6, msgbox.y + 6 }, 16, 1, WHITE);
    DrawTextRec(font1, info.content, msgboxtextmargin, 14, 1, 1, WHITE);
}

void DrawProp(Texture2D texture, Rectangle rect, Vector2 vec, Color color)
{
    DrawTextureRec(texture, rect, vec, RAYWHITE);
    
    if (showDebugInfo)
    {
        DrawRectangleLines(vec.x, vec.y, rect.width, rect.height, color);
    }   
}

void Initialize(AzGameState* state)
{
    ax_initialize(malloc, free);

    InitWindow(640, 480, "A game for you.");
    SetTargetFPS(60);
    
    font1 = LoadFont("resource/font1.ttf");   
    font2 = LoadFont("resource/font2.ttf");

    state->player.entStr = "Azra";

    state->player.frame.x = 0;
    state->player.frame.y = 0;
    state->player.frame.width = 14;
    state->player.frame.height = 18;

    state->player.where.x = 64;
    state->player.where.y = 0;

    state->player.currentFrame = 0;
    state->player.velocity = 0;
    state->player.direction = FACE_DOWN;

    state->textbox.sender = "";
    state->textbox.content = "";
    state->textbox.shouldDraw = 0;
    
    state->paused = 0;
    state->controls_locked = 0;
    state->cursor_visible = 1;
}

void Quit()
{
    UnloadFont(font1);
    UnloadFont(font2);
    CloseWindow();
}
