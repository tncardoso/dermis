#!/bin/bash

PWD=`pwd`
DERMIS_LIB="${PWD}/src/libdermis"

if [ "$1" == "sleep" ]
then
    DERMIS="../examples/example.lua" DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES="${DERMIS_LIB}.dylib" LD_PRELOAD="${DERMIS_LIB}.so" "./examples/sleep"
elif [ "$1" == "malloc" ]
then
    DERMIS="../examples/example.lua" DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES="${DERMIS_LIB}.dylib" LD_PRELOAD="${DERMIS_LIB}.so" "./examples/malloc"
elif [ "$1" == "fgets" ]
then
    DERMIS="../examples/example.lua" DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES="${DERMIS_LIB}.dylib" LD_PRELOAD="${DERMIS_LIB}.so" "./examples/fgets"
else
    echo "try one of the following examples:"
    echo "  sleep"
    echo "  fgets"
    echo "  malloc"
fi


