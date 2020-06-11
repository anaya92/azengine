#pragma once

#include <lua.h>
#include <lauxlib.h>

#include "azhelper.h"
#include "azclient.h"

#include <unistd.h>
#include <pthread.h>

#include <time.h>

typedef pthread_t AzScriptInstance;

extern AzGameState state;
extern lua_State *AzLua;

extern void InitLua();
extern void QuitLua();

extern void AzLua_DoString(const char* com);
extern void AzLua_LoadFile(const char* fname);
extern void AzLua_DoFile(const char* fname);
extern void AzLua_PCall();

extern void AzLua_PushLib(const char* libname, const luaL_Reg *lib);

// azclient func wrappers

extern int lAPI_ToggleControls(lua_State *L);
extern int lAPI_ToggleCursorVisibility(lua_State *L);

extern int lAPI_GetPlayerDir(lua_State *L);
extern int lAPI_SetPlayerDir(lua_State *L);

extern int lAPI_SetPlayerVelocity(lua_State *L);

extern int lAPI_SetPlayerLocation(lua_State *L);
extern int lAPI_GetPlayerLocation(lua_State *L);

// Script Funcs

extern int lAPI_ScriptWait(lua_State *l);

// azhelper func wrappers

extern int lAPI_ShowMsgBox(lua_State *L);
extern int lAPI_SetWeatherState(lua_State *l);
extern int lAPI_SetTemp(lua_State *l);


extern luaL_Reg const Player_Funcs[];
extern luaL_Reg const Core_Funcs[];
extern luaL_Reg const Script_Funcs[];
extern luaL_Reg const Input_Funcs[];
