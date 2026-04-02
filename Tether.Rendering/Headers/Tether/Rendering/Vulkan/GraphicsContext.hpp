#pragma once

#include <Tether/Rendering/Vulkan/AllocatorManager.hpp>

#include <Tether/Rendering/GraphicsContext.hpp>
#include <Tether/Rendering/RenderTarget.hpp>
#include <Tether/Rendering/Vulkan/VertexBuffer.hpp>

#include <Tether/Rendering/Vulkan/Device.hpp>

#include <optional>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class ContextCreator;
	class TETHER_EXPORT GraphicsContext : public Rendering::GraphicsContext
	{
	public:
		/// <summary>
		/// yes
		/// </summary>
		GraphicsContext(
			PFN_vkGetInstanceProcAddr GetInstanceProcAddr,

			uint32_t framesInFlight,
			const InstanceLoader& instanceLoader, // Must have had Load called
			const DeviceLoader& deviceLoader, // Must have had Load called
			VkInstance instance,
			VkDevice device,
			VkQueue queue,
			VkPhysicalDevice physicalDevice,
			VkCommandPool commandPool,
			VmaAllocator allocator // One will be created automatically if it is nullptr
		);
		explicit GraphicsContext(const ContextCreator& vulkanContext);
		~GraphicsContext() override;
		TETHER_NO_COPY(GraphicsContext);

		Scope<RenderTarget> CreateWindowRenderTarget(Window& window) override;

		Scope<Resources::BufferedImage> CreateBufferedImage(
			const Resources::BufferedImageInfo& info) override;
		Scope<Resources::Font> CreateFont(const std::string& fontPath) override;
		
		VertexBuffer& GetSquareBuffer();
		[[nodiscard]] const uint32_t GetFramesInFlight() const;
		[[nodiscard]] const InstanceLoader& GetInstanceLoader() const;
		[[nodiscard]] const DeviceLoader& GetDeviceLoader() const;
		[[nodiscard]] VkInstance GetInstance() const;
		[[nodiscard]] VkDevice GetDevice() const;
		[[nodiscard]] VkQueue GetQueue() const;
		[[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const;
		[[nodiscard]] VkCommandPool GetCommandPool() const;
		[[nodiscard]] VmaAllocator GetAllocator() const;

		[[nodiscard]] VkDescriptorSetLayout GetTexturedPipelineLayout() const;
		[[nodiscard]] VkDescriptorSetLayout GetTextPipelineLayout() const;
	private:
		void Init();

		void CreateDescriptorSetLayouts();
		void CreateVertexBuffers();
		void CreateSampler();

		PFN_vkGetInstanceProcAddr m_GetInstanceProcAddr;

		uint32_t m_FramesInFlight = 2;
		InstanceLoader m_InstanceLoader;
		DeviceLoader m_DeviceLoader;
		VkInstance m_Instance = nullptr;
		VkDevice m_Device = nullptr;
		VkQueue m_Queue = nullptr;
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkCommandPool m_CommandPool = nullptr;
		VmaAllocator m_Allocator = nullptr;

		VkDescriptorSetLayout m_TexturedPipelineSetLayout = nullptr;
		VkDescriptorSetLayout m_TextPipelineLayout = nullptr;

		std::optional<AllocatorManager> m_AllocatorManager;

		std::optional<VertexBuffer> square;
		VkSampler sampler;
	};
}