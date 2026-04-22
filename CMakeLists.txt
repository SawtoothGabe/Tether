cmake_minimum_required (VERSION 3.13)
project ("TetherProject")

include(FetchContent)

# All subdirectories will be compiled with this C++ version
set(CMAKE_CXX_STANDARD 23)

# The relative path to tether from each module directory
# Tether and Tether Modules are both children of the root repo directory,
# so this value shouldn't change.
set(TETHER_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(TETHER_DIR ${TETHER_PROJECT_DIR}/Tether)
set(TETHER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

option(TETHER_BUILD_TESTS "Build tether with tests" OFF)
option(TETHER_BUILD_AS_SHARED "Build tether as a shared library" OFF)
option(TETHER_BUILD_RENDERING "Build Tether.Rendering" OFF)
option(TETHER_HEADLESS "Build Tether without window capabilities (headless)" OFF)

message(STATUS "TETHER_BUILD_TESTS      = ${TETHER_BUILD_TESTS}")
message(STATUS "TETHER_BUILD_AS_SHARED  = ${TETHER_BUILD_AS_SHARED}")
message(STATUS "TETHER_BUILD_RENDERING  = ${TETHER_BUILD_RENDERING}")
message(STATUS "TETHER_HEADLESS  = ${TETHER_HEADLESS}")

set (TETHER_BIN2CPP_DIR "${TETHER_PROJECT_DIR}/Vendor/bin2cpp" CACHE STRING
	"bin2cpp directory")

function(DeclareGitRepo libName gitRepo)
    FetchContent_Declare(
        ${libName}
        GIT_REPOSITORY ${gitRepo}
        SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/${libName}
        BINARY_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-build
        SUBBUILD_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-subbuild
    )
endfunction()

add_subdirectory(Vendor)
add_subdirectory("Tether")

if (TETHER_BUILD_RENDERING)
	add_subdirectory("Tether.Rendering")
endif()