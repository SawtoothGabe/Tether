cmake_minimum_required (VERSION 3.10)
project ("tetherRendering")

option(TETHER_RENDERING_VULKAN_API "Build tether with Vulkan capabilities" ON)
option(TETHER_RENDERING_OPENGL_API "Build tether with OpenGL capabilities" OFF)
message(STATUS "TETHER_RENDERING_VULKAN_API = ${TETHER_RENDERING_VULKAN_API}")
message(STATUS "TETHER_RENDERING_OPENGL_API = ${TETHER_RENDERING_OPENGL_API}")

add_subdirectory(Assets)
add_subdirectory(Vendor)

if (TETHER_BUILD_TESTS)
	add_subdirectory("Tests")
endif()

set_property(TARGET freetype PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${TETHER_LIBRARY_MODE} "Source/WindowUIManager.cpp" "Source/Resources/BufferedImage.cpp" "Source/Resources/Resource.cpp" "Source/ImageLoader.cpp" "Source/StbImageImpl.cpp" "Source/FreetypeLibrary.cpp" "Source/Resources/Font.cpp" "Source/Elements/Element.cpp" "Source/Elements/Division.cpp"  "Source/Elements/Button.cpp" "Source/Elements/ProgressBar.cpp" "Source/Elements/Common/BorderedElement.cpp" "Source/Elements/Text.cpp" "Source/Elements/ImageView.cpp")

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
        PRIVATE
            "/wd4251;/w44265" # Disable the dll-interface warning thingy
    )
else()
	target_compile_options(${PROJECT_NAME}
		PRIVATE
			-Wnon-virtual-dtor
	)
endif()

if (TETHER_RENDERING_VULKAN_API)
	find_package(Vulkan REQUIRED)
	target_sources(${PROJECT_NAME} PRIVATE "Source/Vulkan/GraphicsContext.cpp" "Source/Vulkan/Device.cpp" "Source/Vulkan/DeviceLoader.cpp" "Source/Vulkan/Instance.cpp" "Source/Vulkan/InstanceLoader.cpp" "Source/Vulkan/Surface.cpp" "Source/Vulkan/Pipeline.cpp" "Source/Vulkan/Swapchain.cpp" "Source/Vulkan/VertexBuffer.cpp" "Source/Vulkan/BufferStager.cpp" "Source/Vulkan/VmaImplementation.cpp"    "Source/Vulkan/ShaderModule.cpp"  "Source/Vulkan/DescriptorPool.cpp" "Source/Vulkan/UniformBuffer.cpp" "Source/Vulkan/Resources/BufferedImage.cpp"  "Source/Vulkan/SingleUseCommandBuffer.cpp"   "Source/Vulkan/DescriptorSet.cpp" "Source/Vulkan/DescriptorSetWritable.cpp" "Source/Vulkan/Resources/Font.cpp"   "Source/Vulkan/ImageStager.cpp" "Source/Vulkan/AllocatorManager.cpp" "Source/Vulkan/ContextCreator.cpp" "Source/Vulkan/WindowRenderTargetVk.cpp"  "Source/Vulkan/CommandBufferState.cpp" "Source/Vulkan/VulkanRenderer.cpp")
endif()

if (TETHER_HEADLESS)
	add_definitions(-DTETHER_HEADLESS)
endif()

add_dependencies(${PROJECT_NAME} tether compileTetherRenderingAssets freetype)

target_include_directories(${PROJECT_NAME} 
PUBLIC 
	"Headers"
	${Vulkan_INCLUDE_DIR}
PRIVATE
	${TETHER_BINARY_DIR}
	${TETHER_DIR}/Headers

)

target_link_libraries(${PROJECT_NAME}
	tether freetype stb VulkanMemoryAllocator
)
