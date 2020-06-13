#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <raylib.h>
#include "azluaapi.h"
#include "controls.h"

#define GLSL_VERSION 330

#define CLS_COLOR (Color) { 67, 143, 168, 255 }

int sw = 640;
int sh = 480;
int noclip = 0;

AzGameState state;
int showDebugInfo = 0;

Rectangle getBackgroundTile(int col, int row, int w, int h)
{
    return (Rectangle) { col * w, row * h, w, h };
}

static void ResizeForUpdate()
{
    if (IsWindowFullscreen()) sh = GetMonitorHeight(0);
    else sh = GetScreenHeight();

    sw = round((float)(((sh * 4) / 3) + 0.5));    
}

int main(int argc, char* argv[])
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    Initialize(&state);
    InitLua();

    SetExitKey(0);

    state.map = AzLoadMap("resource/map.xml");

    Texture2D azra = LoadTexture("resource/azra.png");
    Texture2D background = LoadTexture(state.map.pathToSheet);
    
    RenderTexture2D RenderTarget = LoadRenderTexture(sw, sh);
    SetTextureFilter(RenderTarget.texture, FILTER_POINT);

    AzAnimationComponent waves = AzCreateAnim("waves", 1, 2, 16, 16, 2, 4);

    state.camera.target = (Vector2) { state.player.frame.x + 7, state.player.frame.y + 9 };
    state.camera.offset = (Vector2) { sw/2, sh/2 };
    state.camera.rotation = 0;
    state.camera.zoom = 1.6f;
    
    int frameCounter = 0;
    
    AzLua_DoFile(state.map.startScript);
    ResizeForUpdate();
       
    while (!WindowShouldClose())
    {        
        frameCounter++;
        int mouseX, mouseY;

        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))
        {
            if (!IsWindowFullscreen()) SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
            else 
            {
                sw = 640;
                sh = 480;
                SetWindowSize(sw, sh);
            }

            ToggleFullscreen();
            ResizeForUpdate();
        }

        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_SEMICOLON)) noclip = !noclip;
        if (IsKeyPressed(KEY_ESCAPE))
        {
            state.paused = !state.paused;
        }
        
        if (IsKeyPressed(KEY_F11))
        {
            showDebugInfo = !showDebugInfo;
        }
    
        mouseX = GetMouseX();
        mouseY = GetMouseY();
        
        AzUpdateControls(&state.controls);

        if (state.paused) 
        {
            if (IsKeyPressed(KEY_Q)) break;
            goto _startdraw;
        }
            
        if (!state.controls_locked) UpdateAzPVel(&state.player, state.controls);
        
        // Update Player Location
        MoveAzP(&state.player, state.controls);
        if (!noclip) DoCollisionsAzP(&state.player, state.map.collisions.list, state.map.collisions.count);
        DoEvCollisionsAzP(&state.player, state.map.eventCollisions.list, state.map.eventCollisions.count);

        // Update Camera2D && Animations
        UpdateAzP(&state.player, &frameCounter, state.player.velocity * 4);
        AzUpdateCamera(&state.player);

        AzUpdateAnim(&waves);

_startdraw:
        BeginDrawing();
            if (state.paused)
            {
                ClearBackground(BLACK);
                DrawTextEx(font2, "PAUSED. \"Q\" TO QUIT.", (Vector2) { GetScreenWidth()/2 - (strlen("PAUSED \"Q\" to quit.") * 5), GetScreenHeight()/2 - 20}, 20, 1, WHITE);
                goto _drawend;
            }

            ClearBackground(BLACK);
                BeginTextureMode(RenderTarget);
                    ClearBackground(CLS_COLOR);
                    BeginMode2D(state.camera);
                        //Background Layer

                        for (int i = -320; i < 640; i += 16)
                        {
                            for (int j = -144; j < 480; j += 16)
                            {
                                Vector2 pos = { i , j };
                                
                                DrawTextureRec(background, getBackgroundTile(0, 0, 16, 16), pos, WHITE);
                            }
                        }

                        for (int i = -320; i < -280; i += 16)
                        {
                            for (int j = -144; j < 480; j += 16)
                            {
                                Vector2 pos = { i , j };
                                
                                DrawProp(background, waves.frame, pos, WHITE);                                    
                            }
                        }
                        
                        // Background Props  

                        DrawProp(background, (Rectangle) { 32, 0, 16, 32}, (Vector2) {0, 0}, WHITE);
                        DrawProp(background, (Rectangle) { 32, 0, 16, 32}, (Vector2) {16, 0}, WHITE);
                        DrawProp(background, (Rectangle) { 32, 0, 16, 32}, (Vector2) {32, 0}, WHITE);
                        
                        DrawProp(background, (Rectangle) { 16, 0, 48, 32 }, (Vector2) { 128, 128 }, WHITE);
                        DrawProp(background, (Rectangle) { 0, 16, 16, 16}, (Vector2) { 0, 96 }, WHITE);
                        
                        // Prop Layer
                        DrawAzP(azra, &state.player);
                        
                        // Prop2 Layer
                        
                        // In-Camera Debugging Stuff
                        if (showDebugInfo)
                        {
                            for (int i = 0; i < state.map.collisions.count; i++)
                            {
                                DrawRectangleLinesEx(state.map.collisions.list[i], 1, BLUE);
                            }
                            
                            for (int i = 0; i < state.map.eventCollisions.count; i++)
                            {
                                DrawRectangleLinesEx(state.map.eventCollisions.list[i].bounds, 1, MAGENTA);
                            }
                        }
                EndMode2D();
                if (state.textbox.shouldDraw)
                {
                    DrawTextBox(state.textbox);
                }

                if (showDebugInfo)
                {
                    DrawTextEx(font1, FormatText("Camera2D\n Target: (%f, %f)\n Zoom: %f\n FPS: %i fps",
                            state.camera.target.x, state.camera.target.y,
                            state.camera.zoom,
                            GetFPS()),
                            (Vector2) { 5,5 },
                        8, 1, BLACK);
                }
                
                DrawText(FormatText("Temp: %i", state.temperature), 10, 470, 10, WHITE);              
            EndTextureMode();

            DrawTexturePro(RenderTarget.texture, (Rectangle){ 0.0f, 0.0f, (float)RenderTarget.texture.width, (float)-RenderTarget.texture.height },
               (Rectangle){ (GetScreenWidth() - ((float)sw))*0.5, 0,
               (float)sw, (float)sh }, (Vector2){ 0, 0 }, 0.0f, WHITE);
_drawend:
        EndDrawing();
    }

    UnloadTexture(background);
    UnloadTexture(azra);
    
    UnloadRenderTexture(RenderTarget);
    
    QuitLua();
    Quit();
    return 0;
}
