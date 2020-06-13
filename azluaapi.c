#include "azluaapi.h"

lua_State *AzLua;

luaL_Reg const Player_Funcs[] = {
    { "getDirection", lAPI_GetPlayerDir },
    { "setDirection", lAPI_SetPlayerDir },
    { "setVelocity", lAPI_SetPlayerVelocity },
    { "setLocation", lAPI_GetPlayerLocation },
    { "setLocation", lAPI_SetPlayerLocation },    
    { NULL, NULL }    
};

luaL_Reg const Core_Funcs[] = {
    { "toggleControls", lAPI_ToggleControls },
    { "showMessage", lAPI_ShowMsgBox },
    { "toggleCursor", lAPI_ToggleCursorVisibility },
    { NULL, NULL }    
};

luaL_Reg const Script_Funcs[] = {
    { "wait", lAPI_ScriptWait },
    { NULL, NULL }
};

luaL_Reg const Map_Funcs[] = {
    { "setTemp", lAPI_SetTemp },
    { NULL, NULL }
};

/* luaL_Reg const Input_Funcs[] = {
    { "IsKeyDown"
    { NULL, NULL }    
};
 */

static void wait(float seconds)
{
    float start = GetTime();
    
    while (GetTime() < (start + seconds)) continue; 
}
 
void InitLua()
{
    AzLua = luaL_newstate();
    
    AzLua_PushLib("core", Core_Funcs);
    AzLua_PushLib("player", Player_Funcs);
    AzLua_PushLib("script", Script_Funcs);
    AzLua_PushLib("map", Map_Funcs);
    
    luaL_openlibs(AzLua);
}

void QuitLua()
{
    lua_close(AzLua);
}

void AzLua_PushLib(const char* libname, const luaL_Reg *lib)
{
    luaL_newlib(AzLua, lib);
    lua_setglobal(AzLua, libname);
}

int lAPI_GetPlayerDir(lua_State *L)
{
    lua_pushstring(L, GetAzPDirectionString(&state.player));
    return 1;
}

int lAPI_SetPlayerDir(lua_State *L)
{
    int dir = lua_tonumber(L, 1);
    
    state.player.direction = dir;
    
    return 0;
}

int lAPI_SetPlayerVelocity(lua_State *L)
{
    int v = lua_tonumber(L, 1);
    
    state.player.velocity = v;
    
    return 0;
}

int lAPI_SetPlayerLocation(lua_State *L)
{
    int x = lua_tonumber(L, 1);
    int y = lua_tonumber(L, 2);
    
    state.player.where.x = x;
    state.player.where.y = y;
    
    return 0;
}

int lAPI_GetPlayerLocation(lua_State *L)
{
    lua_pushnumber(L, state.player.where.x);
    lua_pushnumber(L, state.player.where.y);
    
    return 2;
}

// core funcs

int lAPI_ToggleControls(lua_State *L)
{
    state.controls_locked = !state.controls_locked;
    return 0;
}

int lAPI_ToggleCursorVisibility(lua_State *L)
{
    state.cursor_visible = !state.cursor_visible;
    
    if (state.cursor_visible)
    {
        ShowCursor();
    }
    else
    {
        HideCursor();
    }
    
    return 0;
}

int lAPI_ShowMsgBox(lua_State *L)
{   
    char* sender = lua_tostring(L, 1);
    char* content = lua_tostring(L, 2);
    
    printf("%s, %s\n", sender, content);

    state.textbox.sender = sender;
    state.textbox.content = content;
    state.textbox.shouldDraw = 1;
    
    while(!IsKeyDown(KEY_ENTER))
    {
        continue;
    }
    
    state.textbox.shouldDraw = 0;
}

int lAPI_SetWeatherState(lua_State *l)
{
    int wstate = lua_tointeger(l, 1);

    state.weatherstate = wstate;
}

int lAPI_SetTemp(lua_State *l)
{
    printf("entered\n");
    int temp = lua_tointeger(l, 1);

    printf("new temp: %i\n", temp);

    if (temp > state.temperature)
    {
        while (state.temperature < temp)
        {
            state.temperature++;
            wait(0.5);
        }
    }
    else if (temp < state.temperature) 
    {
        while (state.temperature > temp)
        {
            state.temperature--;
            wait(0.5);
        }
    }
    else
    {
        return 0;
    }

    return 0;
}

// script funcs

int lAPI_ScriptWait(lua_State *l)
{
    float seconds = lua_tointeger(l, 1);

    wait(seconds);
    
    return 0;
}

// C API

void __DOFILE(char* fname)
{
    printf("(LOG) Script started. (%s)\n", fname);
    
    luaL_dofile(AzLua, fname);
    
    printf("(LOG) Script finished. (%s)\n", fname);
    
    pthread_exit(NULL);
}

void __DOSTRING(char* string)
{
    printf("(LOG) Script started.\n");
    
    luaL_dostring(AzLua, string);
    
    printf("(LOG) Script finished.\n");
    
    pthread_exit(NULL);
}

void __PCALL()
{
    lua_pcall(AzLua, 0, 0, 0);
    
    pthread_exit(NULL);   
}

// azhelper  implementations

void AzLua_DoFile(const char* fname)
{    
    AzScriptInstance thread;
    int err;
    
    err = pthread_create(&thread, NULL, __DOFILE, fname);
    
    if (err)
    {
        printf("Script start failed\n");
    }
}

void AzLua_LoadFile(const char* fname)
{
    luaL_loadfile(AzLua, fname);
}

void AzLua_PCall()
{
    AzScriptInstance thread;
    int err;
    
    err = pthread_create(&thread, NULL, __PCALL, NULL);
    
    if (err)
    {
        printf("Script start failed\n");
    }
}

void AzLua_DoString(const char* com)
{
    AzScriptInstance thread;
    int err;
    
    err = pthread_create(&thread, NULL, __DOSTRING, com);
    
    if (err)
    {
        printf("Script start failed\n");
    }
}
