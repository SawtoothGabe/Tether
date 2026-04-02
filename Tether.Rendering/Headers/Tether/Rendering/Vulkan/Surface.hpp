#pragma once

#include <Tether/Window.hpp>

#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class Instance;
	class InstanceLoader;
	class TETHER_EXPORT Surface
	{
	public:
		Surface(const GraphicsContext& context); // Creates headless surface
#ifndef TETHER_HEADLESS
		Surface(const GraphicsContext& context, Window& window);
#endif
		~Surface();
		TETHER_NO_COPY(Surface);

		VkSurfaceKHR Get();
	private:
		VkInstance m_Instance = nullptr;
		const InstanceLoader& m_Loader;

		VkSurfaceKHR m_Surface = nullptr;
	};
}
