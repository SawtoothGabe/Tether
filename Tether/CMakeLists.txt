cmake_minimum_required (VERSION 3.10)
project ("tether")

if (TETHER_BUILD_TESTS)
	add_subdirectory("Tests")
endif()

file(GLOB_RECURSE SOURCES "Source/*.cpp" "Headers/*.hpp")
add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} ${SOURCES})

if (TETHER_BUILD_AS_SHARED)
	set(TETHER_LIBRARY_MODE SHARED)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _TETHER_BUILD_DLL)
else()
	set(TETHER_LIBRARY_MODE STATIC)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _TETHER_STATIC _TETHER_BUILD_STATIC)
endif()

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
        PRIVATE
            "/wd4251;" # Disable the dll-interface warning thingy
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "Headers")

if (NOT TETHER_HEADLESS)
	if (WIN32)
		file(GLOB_RECURSE WIN_SOURCES "Platform/Windows/*.cpp" "Platform/Windows/*.hpp")
		target_sources(${PROJECT_NAME} PRIVATE ${WIN_SOURCES})
		target_include_directories(${PROJECT_NAME} PRIVATE "Platform/Windows")
	elseif (UNIX)
		file(GLOB_RECURSE LINUX_SOURCES "Platform/Linux/*.cpp" "Platform/Linux/*.hpp")
		target_sources(${PROJECT_NAME} PRIVATE ${LINUX_SOURCES})
		target_include_directories(${PROJECT_NAME} PRIVATE "Platform/Linux")
		target_link_libraries(${PROJECT_NAME} PUBLIC X11 Xrandr Xi)
	endif()
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC TETHER_HEADLESS)
endif()
