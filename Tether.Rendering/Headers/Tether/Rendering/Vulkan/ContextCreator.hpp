#pragma once

#include <Tether/Common/Library.hpp>

#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <Tether/Rendering/Vulkan/Instance.hpp>
#include <Tether/Rendering/Vulkan/Device.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ContextCreator
	{
	public:
		class VulkanLibrary : public Library
		{
		public:
			VulkanLibrary();

			TETHER_VULKAN_FUNC_VAR(GetInstanceProcAddr);
			TETHER_VULKAN_FUNC_VAR(CreateInstance);
			TETHER_VULKAN_FUNC_VAR(EnumerateInstanceExtensionProperties);
			TETHER_VULKAN_FUNC_VAR(EnumerateInstanceLayerProperties);
		};

		struct Info
		{
#ifdef NDEBUG
			bool enableValidationLayers = false;
#else
			bool enableValidationLayers = true;
#endif
			std::string applicationName = "Tether";
			std::string engineName = "Tether";
			std::span<const char*> deviceExtensions = {};
			const void* devicePNext = nullptr;
			bool createTransferQueue = false;
			bool createComputeQueue = false;
			uint32_t framesInFlight = 2;
		};

		explicit ContextCreator(const Info& contextInfo);
		ContextCreator();
		~ContextCreator();
		TETHER_NO_COPY(ContextCreator);

		void AddDebugMessenger(DebugCallback* debugCallback);
		void RemoveDebugMessenger(DebugCallback* debugCallback);

		uint32_t GetFramesInFlight() const;
		const InstanceLoader& GetInstanceLoader() const;
		const DeviceLoader& GetDeviceLoader() const;
		const VulkanLibrary& GetVulkanLibrary() const;
		VkInstance GetInstance() const;
		VkDevice GetDevice() const;
		QueueFamilyIndices GetQueueFamilyIndices() const;
		VkQueue GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex);
		VkQueue GetQueue() const;
		VkPhysicalDevice GetPhysicalDevice() const;
		VkCommandPool GetCommandPool() const;
	private:
		void CreateCommandPool(uint32_t graphicsFamilyIndex);

		VulkanLibrary m_VulkanLibrary;

		std::optional<Instance> m_Instance;
		std::optional<Device> m_Device;

		VkQueue m_Queue = nullptr;

		VkCommandPool m_CommandPool = nullptr;

		uint32_t m_framesInFlight = 0;
	};
}