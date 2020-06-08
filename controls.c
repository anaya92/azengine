#include "controls.h"

void AzUpdateControls(AzControls* controls)
{
    controls->up = 0;
    controls->down = 0;
    controls->left = 0;
    controls->right = 0;
    controls->sprint = 0;
    
    
    if (IsKeyDown(' '))
    {
        controls->sprint = 1;
    }
    
    if (IsKeyDown('W'))
    {
        controls->up = 1;
    }
    if (IsKeyDown('S'))
    {
        controls->down = 1;
    }
    if (IsKeyDown('A'))
    {
        controls->left = 1;
    }
    if (IsKeyDown('D'))
    {
        controls->right = 1;
    }
    
    if (IsGamepadAvailable(GAMEPAD_PLAYER1))
    {   
        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
        {
            controls->sprint = 1;
        }
        
        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_UP))
        {
            controls->up = 1;
        }
        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
        {
            controls->down = 1;
        }
        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
        {
            controls->left = 1;
        }
        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
        {
            controls->right = 1;
        }
    }
}