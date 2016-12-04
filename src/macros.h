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

#ifndef _DERMIS_MACROS_H_
#define _DERMIS_MACROS_H_

// some log macros, warming up
#define LOG(mod, ...) fprintf(stderr, "[dermis][" mod "]" __VA_ARGS__)
#define INFO_MODULE(mod, ...) LOG(mod, "[*] " __VA_ARGS__)
#define INFO(...) INFO_MODULE("core",  __VA_ARGS__)
#define ERROR_MODULE(mod, ...) LOG(mod, "[!] " __VA_ARGS__)
#define ERROR(...) ERROR_MODULE("core",  __VA_ARGS__)

// ok, this is starting to get crazier
// GET_MACRO helps choosing the right macro to expand
#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define MERGE_COMMA(a, b) a, b
#define MERGE_SEMIC(a, b) a ; b

// you need to check this point when registering new functions
// type to be used when casting
#define DERMIS_CAST_TYPE_SIZET size_t
#define DERMIS_CAST_TYPE_VOIDP void*
#define DERMIS_CAST_TYPE_FILEP FILE*
#define DERMIS_CAST_TYPE_CHARP char*
#define DERMIS_CAST_TYPE_INT uintptr_t
#define DERMIS_CAST_TYPE_UINT uintptr_t
#define DERMIS_CAST_TYPE(x) DERMIS_CAST_TYPE_ ## x

// you need to check this point when registering new functions
// type to be used by each function type
#define DERMIS_TYPE_SIZET size_t
#define DERMIS_TYPE_VOIDP void*
#define DERMIS_TYPE_FILEP FILE*
#define DERMIS_TYPE_CHARP char*
#define DERMIS_TYPE_INT int
#define DERMIS_TYPE_UINT unsigned int
#define DERMIS_TYPE(x) DERMIS_TYPE_ ## x

// you need to check this point when registering new functions
// push function to be used for each dermis type
#define DERMIS_TYPE_PUSH_SIZET lua_pushinteger
#define DERMIS_TYPE_PUSH_VOIDP lua_pushlightuserdata
#define DERMIS_TYPE_PUSH_FILEP lua_pushlightuserdata
#define DERMIS_TYPE_PUSH_CHARP lua_pushstring
#define DERMIS_TYPE_PUSH_INT lua_pushinteger
#define DERMIS_TYPE_PUSH_UINT lua_pushinteger
#define DERMIS_TYPE_PUSH(x) DERMIS_TYPE_PUSH_ ## x

// you need to check this point when registering new functions
// convert value from lua to dermis type
#define DERMIS_TYPE_TO_SIZET lua_tointeger
#define DERMIS_TYPE_TO_VOIDP lua_topointer
#define DERMIS_TYPE_TO_FILEP lua_topointer
#define DERMIS_TYPE_TO_CHARP lua_tostring
#define DERMIS_TYPE_TO_INT lua_tointeger
#define DERMIS_TYPE_TO_UINT lua_tointeger
#define DERMIS_TYPE_TO(x) DERMIS_TYPE_TO_ ## x

// you need to check this point when registering new functions
// check if lua value correctly corresponds to dermis type
#define DERMIS_TYPE_IS_SIZET lua_isnumber
#define DERMIS_TYPE_IS_VOIDP lua_islightuserdata
#define DERMIS_TYPE_IS_FILEP lua_islightuserdata
#define DERMIS_TYPE_IS_CHARP lua_isstring
#define DERMIS_TYPE_IS_INT lua_isnumber
#define DERMIS_TYPE_IS_UINT lua_isnumber
#define DERMIS_TYPE_IS(x) DERMIS_TYPE_IS_ ## x

// count arguments
#define DERMIS_COUNT(...) GET_MACRO(NULL, ##__VA_ARGS__, 3, 2, 1, 0)

// map function, apply F to every arg
#define DERMIS_MAP_ARGS_1(F, SEP, total, idx, type) F(type, total, idx)
#define DERMIS_MAP_ARGS_2(F, SEP, total, idx, type, ...) SEP(F(type, total, idx), DERMIS_MAP_ARGS_1(F, SEP, total, DERMIS_COUNT(__VA_ARGS__), __VA_ARGS__))
#define DERMIS_MAP_ARGS_3(F, SEP, total, idx, type, ...) SEP(F(type, total, idx), DERMIS_MAP_ARGS_2(F, SEP, total, DERMIS_COUNT(__VA_ARGS__), __VA_ARGS__))
#define DERMIS_MAP_ARGS_4(F, SEP, total, idx, type, ...) SEP(F(type, total, idx), DERMIS_MAP_ARGS_3(F, SEP, total, DERMIS_COUNT(__VA_ARGS__), __VA_ARGS__))
#define DERMIS_MAP_ARGS(F, SEP, ...) \
    GET_MACRO(__VA_ARGS__, DERMIS_MAP_ARGS_4, DERMIS_MAP_ARGS_3, DERMIS_MAP_ARGS_2, DERMIS_MAP_ARGS_1)(F, SEP, DERMIS_COUNT(__VA_ARGS__), DERMIS_COUNT(__VA_ARGS__), __VA_ARGS__)

#define DERMIS_ARGNAME(num) arg ## num
#define DERMIS_FUNC_PARAMS(type, total, num) DERMIS_TYPE(type) DERMIS_ARGNAME(num)
#define DERMIS_FUNC_PARAM_TYPES(type, total, num) DERMIS_TYPE(type)
#define DERMIS_FUNC_PARAM_NAMES(type, total, num) DERMIS_ARGNAME(num)
#define DERMIS_FUNC_LUA_GET(type, total, num) DERMIS_TYPE(type) DERMIS_ARGNAME(num) = (DERMIS_TYPE(type)) DERMIS_TYPE_TO(type)(L, (total-num+1));
// fixme: change this cast for something more safe / portable
#define DERMIS_FUNC_LUA_ARGS(tp, total, num) { .type = tp, .value = (void*) (intptr_t) DERMIS_ARGNAME(num) }

#define DERMIS_PARAMS(...) DERMIS_MAP_ARGS(DERMIS_FUNC_PARAMS, MERGE_COMMA, __VA_ARGS__)
#define DERMIS_PARAM_TYPES(...) DERMIS_MAP_ARGS(DERMIS_FUNC_PARAM_TYPES, MERGE_COMMA, __VA_ARGS__)
#define DERMIS_PARAM_NAMES(...) DERMIS_MAP_ARGS(DERMIS_FUNC_PARAM_NAMES, MERGE_COMMA, __VA_ARGS__)
#define DERMIS_LUA_GET(...) DERMIS_MAP_ARGS(DERMIS_FUNC_LUA_GET, MERGE_SEMIC, __VA_ARGS__)
#define DERMIS_LUA_ARGS(...) DERMIS_MAP_ARGS(DERMIS_FUNC_LUA_ARGS, MERGE_COMMA, __VA_ARGS__)

// function responsible for calling original libc address
#define DERMIS_ORIGINAL(name, ret, ...) \
    DERMIS_TYPE(ret) name ## _original (DERMIS_PARAMS(__VA_ARGS__)) { \
        if (!dermis_core.name.original) { \
            dermis_core.name.original = dlsym(RTLD_NEXT, #name); \
        } \
        DERMIS_TYPE(ret) (*original)(DERMIS_PARAM_TYPES(__VA_ARGS__)) = dermis_core.name.original; \
        return (*original)(DERMIS_PARAM_NAMES(__VA_ARGS__)); \
    }

// function that will be available from lua
#define DERMIS_LUA(name, ret, ...) \
    int name ## _lua(lua_State *L) { \
        DERMIS_LUA_GET(__VA_ARGS__) ; \
        DERMIS_TYPE_PUSH(ret) (L, name ## _original(DERMIS_PARAM_NAMES(__VA_ARGS__))); \
        return 1; \
    }

// wrap lib function
#define DERMIS_LIBC_WRAP(name, ret, ...) \
    DERMIS_TYPE(ret) name (DERMIS_PARAMS(__VA_ARGS__)) { \
        struct dermis_arg retarg; \
        if (dermis_core.name.detour) { \
            struct dermis_arg args[] = { \
                DERMIS_LUA_ARGS(__VA_ARGS__) \
            }; \
            int errn = call_lua(&dermis_core.name, DERMIS_COUNT(__VA_ARGS__), args, &retarg); \
            if (errn) { \
                ERROR("could not call lua detour: %s\n", #name); \
                return name ## _original(DERMIS_PARAM_NAMES(__VA_ARGS__)); \
            } else { \
                return (DERMIS_CAST_TYPE(ret)) retarg.value; \
            } \
        } else { \
            return name ## _original(DERMIS_PARAM_NAMES(__VA_ARGS__)); \
        } \
    }

// register provided functions in lua state
// push in the end is used to ensure a correct stack
#define DERMIS_LUA_REGISTER(name) \
    static int l_register_ ## name (lua_State *L) { \
        if (!lua_isfunction(L, -1)) { \
            ERROR("could not register " #name ": second parameter is not a function\n"); \
            return 0; \
        } \
        if (dermis_core.name.detour) { \
            luaL_unref(dermis_lua, LUA_REGISTRYINDEX, dermis_core.name.detour); \
        } \
        dermis_core.name.detour = luaL_ref(dermis_lua, LUA_REGISTRYINDEX); \
        lua_pushboolean(L, 1); \
        return 0; \
    }

// wrap given function doing necessary work
#define DERMIS_WRAP(name, ret, ...) \
    DERMIS_ORIGINAL(name, ret, __VA_ARGS__) \
    DERMIS_LUA(name, ret, __VA_ARGS__) \
    DERMIS_LIBC_WRAP(name, ret, __VA_ARGS__) \
    DERMIS_LUA_REGISTER(name) \
    void anonfunc()

// register dermis functions in lua state
#define DERMIS_REGISTER(fname, ret) do { \
    dermis_core.fname.retype = ret; \
    lua_pushcfunction(dermis_lua, fname ## _lua); \
    lua_setglobal(dermis_lua, "dermis_" #fname); \
    lua_pushcfunction(dermis_lua, l_register_ ## fname); \
    lua_setglobal(dermis_lua, "dermis_register_" #fname); \
    } while(0)

#define DERMIS_PUSH_CASE(type) \
    case type: DERMIS_TYPE_PUSH(type) (dermis_lua, (DERMIS_CAST_TYPE(type)) args[i].value); break

#define DERMIS_POP_CASE(type) \
    case type: \
        if (!DERMIS_TYPE_IS(type) (dermis_lua, -1)) { \
            ERROR("expected return type = %s got = %s\n", #type, lua_typename(dermis_lua, lua_type(dermis_lua, -2))); \
            error = 1; \
        } else { \
            ret->value = (void*) DERMIS_TYPE_TO(type) (dermis_lua, -1); \
            lua_pop(dermis_lua, 1); \
        } \
        break

#endif
