cmake_minimum_required (VERSION 3.10)
project ("MultipleWindows")

set (SOURCES "Source/Main.cpp")

if (WIN32 AND ("${CMAKE_BUILD_TYPE}" MATCHES "Release"))
    add_executable(
        ${PROJECT_NAME} 
        WIN32 
        ${SOURCES} 
    )
else()
    add_executable(
        ${PROJECT_NAME} 
        ${SOURCES} 
    )
endif()

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers" 
	"../../Headers"
)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} 
		pthread tether X11
	)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} 
		${CMAKE_DL_LIBS} 
		tether
	)
endif()
	
