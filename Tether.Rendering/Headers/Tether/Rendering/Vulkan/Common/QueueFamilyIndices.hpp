#pragma once

#include <cstdint>

namespace Tether::Rendering::Vulkan
{
    struct QueueFamilyIndices
	{
		bool hasGraphicsFamily = false;
		bool hasComputeFamily = false;
		bool hasTransferFamily = false;
		uint32_t graphicsFamilyIndex = 0;
		uint32_t computeFamilyIndex = 0;
		uint32_t transferFamilyIndex = 0;
	};
}
