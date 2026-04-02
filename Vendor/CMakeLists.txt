cmake_minimum_required (VERSION 3.10)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(DeclareLibs)
    DeclareGitRepo(bin2cpp https://github.com/SawtoothGabe/bin2cpp)
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