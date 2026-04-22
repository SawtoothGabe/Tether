cmake_minimum_required (VERSION 3.10)
project ("tether")

if (TETHER_BUILD_TESTS)
	add_subdirectory("Tests")
endif()

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} "Source/Application.cpp" "Source/Common/HashedString.cpp"  "Source/Common/Stopwatch.cpp" "Source/Common/StringTools.cpp" "Source/Controls/Control.cpp" "Source/Devices/Monitor.cpp"  "Source/Events/WindowMoveEvent.cpp" "Source/Events/WindowResizeEvent.cpp" "Source/Input/KeyCharInfo.cpp" "Source/Input/KeyInfo.cpp" "Source/Input/MouseMoveInfo.cpp" "Source/Input/RawMouseMoveInfo.cpp" "Source/Module.cpp"  "Source/Window.cpp" "Source/Common/Stopwatch.cpp"  "Source/Input/MouseClickInfo.cpp")

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

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
)

if (NOT TETHER_HEADLESS)
	if (WIN32)
		target_sources(${PROJECT_NAME} PRIVATE "Source/Platform/Win32Application.cpp" "Source/Platform/Win32Window.cpp" )
	elseif (UNIX)
		target_sources(${PROJECT_NAME} PRIVATE "Source/Platform/X11Application.cpp" "Source/Platform/X11Window.cpp")

		target_link_libraries(${PROJECT_NAME} PUBLIC
			X11 Xrandr Xi
		)
	endif()

	target_sources(${PROJECT_NAME} PRIVATE "Source/Platform/CreateWindow.cpp")
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC TETHER_HEADLESS)
endif()

if (WIN32)
	target_sources(${PROJECT_NAME} PRIVATE "Source/Platform/Win32_Library.cpp")
elseif (UNIX)
	target_sources(${PROJECT_NAME} PRIVATE "Source/Platform/Linux_Library.cpp")
endif()
