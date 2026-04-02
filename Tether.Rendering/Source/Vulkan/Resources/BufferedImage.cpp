#include <Tether/Rendering/Vulkan/Resources/BufferedImage.hpp>
#include <Tether/Rendering/Vulkan/ImageStager.hpp>
#include <stdexcept>

#include <cstring>

namespace Tether::Rendering::Vulkan
{
	BufferedImage::BufferedImage(
		GraphicsContext& context,
		VkSampler sampler,
		VkDescriptorSetLayout pipelineSetLayout,
		const Resources::BufferedImageInfo& info,
		VkFormat imageFormat,
		bool isDepthImage
	)
		:
		Resources::BufferedImage(info),
		m_Device(context.GetDevice()),
		m_Dloader(context.GetDeviceLoader()),
		m_Sampler(sampler),
		m_Allocator(context.GetAllocator()),
        m_ImageFormat(imageFormat)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = info.width;
		imageInfo.extent.height = info.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = m_ImageFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.usage =
		    isDepthImage ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	                     : VK_IMAGE_USAGE_TRANSFER_DST_BIT
	                       | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(m_Allocator, &imageInfo, &allocInfo,
			&m_Image, &m_ImageAllocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image");

		CreateImageView(isDepthImage);

		uint32_t framesInFlight = context.GetFramesInFlight();

	    if (isDepthImage)
	        return;

	    UploadImageData(context, info);

		VkDescriptorPoolSize samplerSize{};
		samplerSize.descriptorCount = framesInFlight;
		samplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		m_Pool.emplace(context, framesInFlight, 1, &samplerSize);
		m_Set.emplace(*m_Pool, pipelineSetLayout, framesInFlight);
		m_Set->UpdateSets(this, 0);
	}

	BufferedImage::~BufferedImage()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);

		m_Dloader.vkDestroyImageView(m_Device, m_ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_Image, m_ImageAllocation);
	}

	void BufferedImage::UploadImageData(GraphicsContext& context, 
		const Resources::BufferedImageInfo& info)
	{
		ImageStager stager(
			context, m_Image,
			info.width, info.height, 4, info.pixelData, m_ImageFormat
		);

		stager.Upload();
	}

	void BufferedImage::CreateImageView(bool isDepthImage)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_ImageFormat;
		viewInfo.subresourceRange.aspectMask =
		    isDepthImage ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (m_Dloader.vkCreateImageView(m_Device, &viewInfo, nullptr,
			&m_ImageView) != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture image view");
	}

	VkDescriptorSet* BufferedImage::GetSetAtIndex(uint32_t index)
	{
		return m_Set->GetSetAtIndex(index);
	}

    VkImage BufferedImage::Get() const
	{
	    return m_Image;
	}

    VkImageView BufferedImage::GetImageView() const
	{
	    return m_ImageView;
	}

	VkDescriptorType BufferedImage::GetDescriptorType()
	{
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	}

	VkDescriptorImageInfo BufferedImage::GetImageInfo(uint32_t setIndex)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_ImageView;
		imageInfo.sampler = m_Sampler;

		return imageInfo;
	}
}