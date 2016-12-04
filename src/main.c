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

// Different types covered by dermis. These are the types and arguments
// that are also returned by libc functions.
// You need to check this point when adding new functions to Dermis.
enum dermis_type {
    SIZET,
    VOIDP,
    FILEP,
    CHARP,
    INT,
    UINT
};

// A given argument or return value for libc functions.
struct dermis_arg {
    enum dermis_type type;
    void* value;
};

// Struct that represents a libc function.
struct dermis_func {
    // Pointer to the original function.
    void* original;
    // Detour id as stored in lua RegistryIndex.
    int detour;
    // Return type of this function.
    enum dermis_type retype;
};

// Struct containing all functions covered by Dermis.
// You need to check this point when adding new functions to Dermis.
struct dermis {
    struct dermis_func sleep;
    struct dermis_func fgets;
    struct dermis_func malloc;
};

// Pointers to functions and detours for wrapped functions.
static struct dermis dermis_core;

// Lua virtual machine.
static lua_State *dermis_lua;

// Call wrapper of func with provided args. Returned value is stored in
// ret and non-zero value is returned in case of error.
int call_lua(struct dermis_func *func,
        unsigned int argc, struct dermis_arg* args,
        struct dermis_arg *ret) {

    lua_rawgeti(dermis_lua, LUA_REGISTRYINDEX, func->detour);

    for (int i = 0; i < argc; i++) {
        // Push function parameters to lua.
        // You need to check this point when adding new functions to Dermis.
        switch (args[i].type) {
            DERMIS_PUSH_CASE(SIZET);
            DERMIS_PUSH_CASE(VOIDP);
            DERMIS_PUSH_CASE(FILEP);
            DERMIS_PUSH_CASE(CHARP);
            DERMIS_PUSH_CASE(INT);
            DERMIS_PUSH_CASE(UINT);
        }
    }

    // Call lua function with provided paramenters.
    if (lua_pcall(dermis_lua, argc, 1, 0) != 0) {
        ERROR("could not call function: %s\n", lua_tostring(dermis_lua, -1));
        return 1;
    }

    int error = 0;
    ret->type = func->retype;
    // Pop and check returned value.
    // You need to check this point when adding new functions to Dermis.
    switch (func->retype) {
            DERMIS_POP_CASE(SIZET);
            DERMIS_POP_CASE(VOIDP);
            DERMIS_POP_CASE(FILEP);
            DERMIS_POP_CASE(CHARP);
            DERMIS_POP_CASE(INT);
            DERMIS_POP_CASE(UINT);
    }

    return error;
}

// Wrap libc functions.
// DERMIS_WRAP(function name, return type, arg1, arg2...)
// You need to check this point when adding new functions to Dermis.
DERMIS_WRAP(sleep, UINT, UINT);
DERMIS_WRAP(fgets, CHARP, CHARP, INT, FILEP);
DERMIS_WRAP(malloc, VOIDP, SIZET);

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
    DERMIS_REGISTER(malloc, VOIDP);
    DERMIS_REGISTER(sleep, UINT);
    DERMIS_REGISTER(fgets, CHARP);

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
        if (lua_pcall(dermis_lua, 0, 0, 0)) {
            ERROR("could not execute file '%s'!\n", fname);
        }
    }
}
