cmake_minimum_required (VERSION 3.10)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(DeclareLibs)
    DeclareGitRepo(stb https://github.com/nothings/stb/)
    DeclareGitRepo(VMA https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
    DeclareGitRepo(freetype https://github.com/freetype/freetype)
endfunction()

function(MakeLibsAvailable)
    FetchContent_MakeAvailable(
        stb
        VMA
        freetype
    )
endfunction()

function(CreateStbImageInterface)
	FetchContent_GetProperties(stb SOURCE_DIR stb_SOURCE_DIR)

	add_library(stb INTERFACE)
	target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endfunction()

DeclareLibs()
MakeLibsAvailable()

CreateStbImageInterface()