cmake_minimum_required (VERSION 3.10)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(DeclareLibs)
    FetchContent_Declare(
            bin2cpp
            GIT_REPOSITORY https://github.com/SawtoothGabe/bin2cpp
            GIT_TAG main
            SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/bin2cpp
            BINARY_DIR ${CMAKE_BINARY_DIR}/Vendor/bin2cpp-build
            SUBBUILD_DIR ${CMAKE_BINARY_DIR}/Vendor/bin2cpp-subbuild
    )
endfunction()

function(MakeLibsAvailable)
    if (NOT "${TETHER_BIN2CPP_DIR}" STREQUAL "")
        message(STATUS "Making bin2cpp available")
        FetchContent_MakeAvailable(
            bin2cpp
        )
    endif()
endfunction()

DeclareLibs()
MakeLibsAvailable()