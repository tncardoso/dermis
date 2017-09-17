// Copyright (c) 2016, Thiago Cardoso
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//  * Neither the name of  nor the names of its contributors may be
//    used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "macros.h"

// Struct that represents a libc function.
struct dermis_func {
    // Pointer to the original function.
    void* original;
    // Detour id as stored in lua RegistryIndex.
    int detour;
};

// Struct containing all functions covered by Dermis.
struct dermis {
    
    struct dermis_func sleep;
    
    struct dermis_func fgets;
    
    struct dermis_func malloc;
    
};

// Pointers to functions and detours for wrapped functions.
static struct dermis dermis_core;

// Lua virtual machine.
static lua_State *dermis_lua;

// Call original libc function.


unsigned int sleep_original (unsigned int seconds) {
    if (!dermis_core.sleep.original) {
        dermis_core.sleep.original = dlsym(RTLD_NEXT, "sleep");
    }
    unsigned int (*original)(unsigned int) = dermis_core.sleep.original;
    return (*original)(seconds);
}

char* fgets_original (char* str, int num, FILE* stream) {
    if (!dermis_core.fgets.original) {
        dermis_core.fgets.original = dlsym(RTLD_NEXT, "fgets");
    }
    char* (*original)(char*, int, FILE*) = dermis_core.fgets.original;
    return (*original)(str, num, stream);
}

void* malloc_original (size_t size) {
    if (!dermis_core.malloc.original) {
        dermis_core.malloc.original = dlsym(RTLD_NEXT, "malloc");
    }
    void* (*original)(size_t) = dermis_core.malloc.original;
    return (*original)(size);
}


// Functions available from lua.


int sleep_lua(lua_State *L) {
    unsigned int seconds = (unsigned int) lua_tointeger(L, 1); 
    lua_pushinteger(L, sleep_original(seconds));
    return 1;
}

int fgets_lua(lua_State *L) {
    char* str = (char*) lua_tostring(L, 1); int num = (int) lua_tointeger(L, 2); FILE* stream = (FILE*) lua_topointer(L, 3); 
    lua_pushstring(L, fgets_original(str, num, stream));
    return 1;
}

int malloc_lua(lua_State *L) {
    size_t size = (size_t) lua_tointeger(L, 1); 
    lua_pushlightuserdata(L, malloc_original(size));
    return 1;
}


// Wrap libc functions.


unsigned int sleep (unsigned int seconds) {
    if (dermis_core.sleep.detour) {
        lua_rawgeti(dermis_lua, LUA_REGISTRYINDEX, dermis_core.sleep.detour);
        
        lua_pushinteger(dermis_lua, (unsigned int) seconds);

        // Call lua function with provided paramenters.
        if (lua_pcall(dermis_lua, 1, 1, 0) != 0) {
            ERROR("could not call function: %s\n", lua_tostring(dermis_lua, -1));
            return sleep_original(seconds);
        }

        // Pop and check returned value.
        if (!lua_isnumber(dermis_lua, -1)) {
            ERROR("expected return type = %s got = %s\n", "UINT", lua_typename(dermis_lua, lua_type(dermis_lua, -2)));
            return sleep_original(seconds);
        }

        unsigned int ret = (unsigned int) lua_tointeger(dermis_lua, 1);
        lua_pop(dermis_lua, 1);
        return ret;
    } else {
        return sleep_original(seconds);
    }
}

char* fgets (char* str, int num, FILE* stream) {
    if (dermis_core.fgets.detour) {
        lua_rawgeti(dermis_lua, LUA_REGISTRYINDEX, dermis_core.fgets.detour);
        
        lua_pushstring(dermis_lua, (char*) str);
        lua_pushinteger(dermis_lua, (int) num);
        lua_pushlightuserdata(dermis_lua, (FILE*) stream);

        // Call lua function with provided paramenters.
        if (lua_pcall(dermis_lua, 3, 1, 0) != 0) {
            ERROR("could not call function: %s\n", lua_tostring(dermis_lua, -1));
            return fgets_original(str, num, stream);
        }

        // Pop and check returned value.
        if (!lua_isstring(dermis_lua, -1)) {
            ERROR("expected return type = %s got = %s\n", "CHARP", lua_typename(dermis_lua, lua_type(dermis_lua, -2)));
            return fgets_original(str, num, stream);
        }

        char* ret = (char*) lua_tostring(dermis_lua, 1);
        lua_pop(dermis_lua, 1);
        return ret;
    } else {
        return fgets_original(str, num, stream);
    }
}

void* malloc (size_t size) {
    if (dermis_core.malloc.detour) {
        lua_rawgeti(dermis_lua, LUA_REGISTRYINDEX, dermis_core.malloc.detour);
        
        lua_pushinteger(dermis_lua, (size_t) size);

        // Call lua function with provided paramenters.
        if (lua_pcall(dermis_lua, 1, 1, 0) != 0) {
            ERROR("could not call function: %s\n", lua_tostring(dermis_lua, -1));
            return malloc_original(size);
        }

        // Pop and check returned value.
        if (!lua_islightuserdata(dermis_lua, -1)) {
            ERROR("expected return type = %s got = %s\n", "VOIDP", lua_typename(dermis_lua, lua_type(dermis_lua, -2)));
            return malloc_original(size);
        }

        void* ret = (void*) lua_topointer(dermis_lua, 1);
        lua_pop(dermis_lua, 1);
        return ret;
    } else {
        return malloc_original(size);
    }
}


// Register callback from lua


static int l_register_sleep (lua_State *L) {
    if (!lua_isfunction(L, -1)) { \
        ERROR("could not register sleep: second parameter is not a function\n");
        return 0;
    }

    if (dermis_core.sleep.detour) {
        luaL_unref(dermis_lua, LUA_REGISTRYINDEX, dermis_core.sleep.detour);
    }

    dermis_core.sleep.detour = luaL_ref(dermis_lua, LUA_REGISTRYINDEX);
    lua_pushboolean(L, 1);
    return 0;
}

static int l_register_fgets (lua_State *L) {
    if (!lua_isfunction(L, -1)) { \
        ERROR("could not register fgets: second parameter is not a function\n");
        return 0;
    }

    if (dermis_core.fgets.detour) {
        luaL_unref(dermis_lua, LUA_REGISTRYINDEX, dermis_core.fgets.detour);
    }

    dermis_core.fgets.detour = luaL_ref(dermis_lua, LUA_REGISTRYINDEX);
    lua_pushboolean(L, 1);
    return 0;
}

static int l_register_malloc (lua_State *L) {
    if (!lua_isfunction(L, -1)) { \
        ERROR("could not register malloc: second parameter is not a function\n");
        return 0;
    }

    if (dermis_core.malloc.detour) {
        luaL_unref(dermis_lua, LUA_REGISTRYINDEX, dermis_core.malloc.detour);
    }

    dermis_core.malloc.detour = luaL_ref(dermis_lua, LUA_REGISTRYINDEX);
    lua_pushboolean(L, 1);
    return 0;
}



// Function executed when this module is loaded via LD_PRELOAD or
// similar technique. The lua state is initialized and functions are
// registered.
static void dermis_init() __attribute__((constructor));
static void dermis_init() {
    INFO("loaded dermis\n");

    INFO("initing lua state\n");
    dermis_lua = luaL_newstate();
    luaL_openlibs(dermis_lua);

    // Register available functions in lua. These functions include
    // wrapper functions and dermis' API.
    // You need to check this point when adding new functions to Dermis.
    INFO("registering functions in lua\n");
    
    // Register sleep
    lua_pushcfunction(dermis_lua, sleep_lua);
    lua_setglobal(dermis_lua, "dermis_sleep");
    lua_pushcfunction(dermis_lua, l_register_sleep);
    lua_setglobal(dermis_lua, "dermis_register_sleep");

    
    // Register fgets
    lua_pushcfunction(dermis_lua, fgets_lua);
    lua_setglobal(dermis_lua, "dermis_fgets");
    lua_pushcfunction(dermis_lua, l_register_fgets);
    lua_setglobal(dermis_lua, "dermis_register_fgets");

    
    // Register malloc
    lua_pushcfunction(dermis_lua, malloc_lua);
    lua_setglobal(dermis_lua, "dermis_malloc");
    lua_pushcfunction(dermis_lua, l_register_malloc);
    lua_setglobal(dermis_lua, "dermis_register_malloc");

    

    // The lua script that will be executed should be passed via DERMIS
    // environment variable.
    char *fname = getenv("DERMIS");
    if (!fname) {
        ERROR("DERMIS env not set with lua script, aborting!\n");
        return;
    }

    if (luaL_loadfile(dermis_lua, fname)) {
        ERROR("could not load file '%s', aborting!\n", fname);
    } else {
        int callret = lua_pcall(dermis_lua, 0, 0, 0);
        if (callret) {
            ERROR("could not execute file '%s' error= %d!\n", fname, callret);
        }
    }
}
