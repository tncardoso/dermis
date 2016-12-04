#!/bin/bash

PWD=`pwd`
DERMIS_LIB="${PWD}/src/libdermis"

if [ "$1" == "sleep" ]
then
    DERMIS="../examples/example.lua" DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES="${DERMIS_LIB}.dylib" LD_PRELOAD="${DERMIS_LIB}.so" "./examples/sleep"
else
    echo "try one of the following examples:"
    echo "  sleep"
    echo "  fgets"
    echo "  malloc"
fi


