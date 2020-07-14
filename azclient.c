#include "azclient.h"


char* GetAzPDirectionString(AzP* player)
{
    switch (player->direction)
    {
        case FACE_UP:
            return "FACE_UP";
            break;
        case FACE_DOWN:
            return "FACE_DOWN";
            break;
        case FACE_LEFT:
            return "FACE_LEFT";
            break;
        case FACE_RIGHT:
            return "FACE_RIGHT";
            break;
        default:
            return "NULL";
            break;
    }
}

void UpdateAzP(AzP* player, int* frameCounter, int speed)
{
    if (player->velocity != 0)
    {
        if (*frameCounter >= (60/speed))
        {
            *frameCounter = 0;
            player->currentFrame++;
            
            if (player->currentFrame > 3) player->currentFrame = 0;
        }
    }
    else
    {
        player->currentFrame = 0;
    }
    
    player->frame.x = player->frame.width * player->currentFrame;
    player->frame.y = player->frame.height * player->direction;
}

void MoveAzP(AzP* player, AzControls controls)
{
    switch (player->direction)
    {
        case FACE_UP:
            player->where.y -= player->velocity;
            break;
        case FACE_DOWN:
            player->where.y += player->velocity;
            break;
        case FACE_LEFT:
            player->where.x -= player->velocity;
            break;
        case FACE_RIGHT:
            player->where.x += player->velocity;
            break;
    }
}

void DrawAzP(Texture2D texture, AzP* player)
{
    DrawProp(texture, player->frame, player->where, BLUE);
    
    if (showDebugInfo)
    {
        DrawTextEx(font1, FormatText("AzP: %s (%f, %f)\n Direction: %s\n Velocity: %i\n Current Frame: %i",
            player->entStr, player->where.x, player->where.y,
            GetAzPDirectionString(player), player->velocity, player->currentFrame), 
            (Vector2) { player->where.x + 15, player->where.y - 5 },
            4 / (state.camera.zoom/2), 0, BLACK);               
    }
}

void UpdateAzPVel(AzP* player, AzControls controls) 
{
    int velocitySALT = 0;

    if (controls.sprint)
    {
        velocitySALT = 1;
    }
    else
    {
        velocitySALT = 0;
    }
    
    if (controls.up)
    {
        player->direction = FACE_UP;
        player->velocity = MAX_MOVEMENT_SPEED + velocitySALT;
    }
    else if (controls.down)
    {
        player->direction = FACE_DOWN;
        player->velocity = MAX_MOVEMENT_SPEED + velocitySALT;
    }
    else if (controls.left)
    {
        player->direction = FACE_LEFT;
        player->velocity = MAX_MOVEMENT_SPEED + velocitySALT;
    }
    else if (controls.right)
    {
        player->direction = FACE_RIGHT;
        player->velocity = MAX_MOVEMENT_SPEED + velocitySALT;
    }
    else
    {
        player->velocity = 0;
    }
}

void DoCollisionsAzP(AzP* player, Rectangle* Collisions, int collisionsCount)
{
    Rectangle temp = { player->where.x, player->where.y, player->frame.width, player->frame.height };
    
    for (int i = 0; i < collisionsCount; i++) 
    {
        Rectangle inter = GetCollisionRec(temp, Collisions[i]);

        if ((inter.x + inter.y + inter.width + inter.height != 0))
        {
            switch (player->direction)
            {
                case FACE_UP:
                    player->where.y += inter.height; 
                    break;                
                case FACE_DOWN:
                    player->where.y -= inter.height;
                    break;
                case FACE_LEFT:
                    player->where.x += inter.width;
                    break;
                case FACE_RIGHT:
                    player->where.x -= inter.width;
                    break;
            }
            
            player->velocity = 0;
        }
    }
}

void DoEvCollisionsAzP(AzP* player, AzEventTrigger* EventCollisions, int eventCount)
{
    Rectangle temp = { player->where.x, player->where.y, player->frame.width, player->frame.height };

    for (int i = 0; i < eventCount; i++)
    {
        Rectangle inter = GetCollisionRec(temp, EventCollisions[i].bounds);
        
        if (inter.width == player->frame.width && inter.height == player->frame.height && !EventCollisions[i].hasBeenTriggered)
        {
            printf("Event Triggered: \'%s\'\n", EventCollisions[i].name);
            AzLua_DoString(EventCollisions[i].script);
            
            
            EventCollisions[i].hasBeenTriggered = 1;
        }
    }    
}

void DoInteractCollisionsAzP(AzP* player, AzInteractableCollider* InteractCollisions, int interactCount)
{
    Rectangle temp = { player->where.x, player->where.y, player->frame.width, player->frame.height };

    for (int i = 0; i < interactCount; i++)
    {
        Rectangle inter = GetCollisionRec(temp, InteractCollisions[i].bounds);
        
        if (inter.width == player->frame.width && inter.height == player->frame.height && (InteractCollisions[i].req_face == player->direction))
        {
            DrawTextEx(font2, "press e to interact.", (Vector2) { 10, 10 }, 8, 1, BLACK);

            state.interactPrompt = 1;

            if (IsKeyPressed(KEY_E))
            {
                printf("Event Triggered: \'%s\'\n", InteractCollisions[i].name);
                AzLua_DoString(InteractCollisions[i].script);
            }
        }
        else
        {
            state.interactPrompt = 0;
        }        
    }    
}

void AzUpdateCamera(AzP* player)
{
    state.camera.target = (Vector2) { player->where.x + 7, player->where.y + 9 };
}

AzAnimationComponent AzCreateAnim(char* animName, int originx, int originy, int width, int height, int speed, int frameCount)
{
    AzAnimationComponent returnval;

    returnval.speed = speed;

    returnval.OriginX = originx;
    returnval.OriginY = originy;    

    returnval.frame.height = height;
    returnval.frame.width = width;

    returnval.frame.x =  returnval.OriginX * width;
    returnval.frame.y =  returnval.OriginY * height;

    returnval.frameCount = frameCount;

    strcpy(returnval.entStr, animName);

    return returnval;
}

void AzUpdateAnim(AzAnimationComponent* comp)
{
    comp->frameCounter++;

    if (comp->frameCounter >= (60/comp->speed))
    {
        comp->frameCounter = 0;
        comp->currentFrame++;

        if (comp->currentFrame > (comp->frameCount - 1)) comp->currentFrame = 0;
    }

    comp->frame.x = (comp->OriginX + comp->currentFrame) * comp->frame.width;
}