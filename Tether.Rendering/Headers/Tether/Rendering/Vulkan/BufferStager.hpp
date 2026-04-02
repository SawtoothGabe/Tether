#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Vulkan/DeviceLoader.hpp>

#include <string>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class GraphicsContext;
	class TETHER_EXPORT BufferStager
	{
	public:
		BufferStager(
			GraphicsContext& context,
			VkBuffer buffer,
			size_t bufferSize
		);
		BufferStager(
			GraphicsContext& context,
			VkBuffer buffer,
			size_t bufferSize,
			VkQueue queue,
			VkCommandPool pool
		);
		~BufferStager();
		TETHER_NO_COPY(BufferStager);

		void UploadData(void* data);
		void UploadDataAsync(const void* data) const;

		// Wait for data to finish uploading.
		void Wait() const;
	private:
		void CreateCommandBuffer();
		void CreateFence();

		void CreateStagingBuffer();
		void RecordCommandBuffer() const;

		VmaAllocationInfo m_StagingInfo;

		VkBuffer m_StagingBuffer = nullptr;
		VmaAllocation m_StagingAllocation = nullptr;
		VkCommandBuffer m_CommandBuffer = nullptr;
		VkFence m_CompletedFence = nullptr;
		VkQueue m_Queue = nullptr;
		VkCommandPool m_CommandPool = nullptr;

		GraphicsContext& m_Context;

		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;
		
		VkBuffer m_Buffer = nullptr;
		size_t m_BufferSize = 0;
	};
}