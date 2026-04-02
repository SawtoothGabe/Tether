#include <Tether/Rendering/Vulkan/Instance.hpp>
#include <stdexcept>

#include <set>

namespace Tether::Rendering::Vulkan
{
	static const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	Device::Device(Instance& instance, 
		std::span<const char*> extensions,
		const void* pNext,
		bool createTransferQueue,
		bool createComputeQueue)
		:
		m_Instance(instance),
		m_Iloader(instance.GetLoader())
	{
		PickDevice();
		FindQueueFamilies(m_PhysicalDevice);

		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		
		VkDeviceQueueCreateInfo graphicsQueue{};
		graphicsQueue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueue.queueFamilyIndex = m_Indices.graphicsFamilyIndex;
		graphicsQueue.queueCount = 1;
		graphicsQueue.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(graphicsQueue);

		if (createTransferQueue)
		{
			VkDeviceQueueCreateInfo transfer{};
			transfer.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			transfer.queueFamilyIndex = m_Indices.transferFamilyIndex;
			transfer.queueCount = 1;
			transfer.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(transfer);
		}

		if (createComputeQueue)
		{
			VkDeviceQueueCreateInfo compute{};
			compute.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			compute.queueFamilyIndex = m_Indices.computeFamilyIndex;
			compute.queueCount = 1;
			compute.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(compute);
		}

		VkPhysicalDeviceFeatures features{};

		std::vector<const char*> allExtensions = deviceExtensions;
		allExtensions.reserve(extensions.size());
		allExtensions.insert(allExtensions.end(), extensions.begin(), extensions.end());

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = pNext;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = queueCreateInfos.size();
		createInfo.pEnabledFeatures = &features;
		createInfo.enabledExtensionCount = 
			static_cast<uint32_t>(allExtensions.size());
		createInfo.ppEnabledExtensionNames = allExtensions.data();

		if (instance.IsDebugMode())
		{
			// Enable validation layers

			createInfo.enabledLayerCount =
				static_cast<uint32_t>(instance.validationLayers.size());
			createInfo.ppEnabledLayerNames = instance.validationLayers.data();
		}

		// Create the device
		if (m_Iloader.vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device)
			!= VK_SUCCESS)
			throw std::runtime_error("Device creation failed");

		m_Loader.Load(m_Iloader, m_Device);
	}

	Device::~Device()
	{
		m_Loader.vkDestroyDevice(m_Device, nullptr);
	}

	VkQueue Device::GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex)
	{
		VkQueue queue;
		m_Loader.vkGetDeviceQueue(m_Device, familyIndex, queueIndex, &queue);

		return queue;
	}

	void Device::WaitIdle()
	{
		m_Loader.vkDeviceWaitIdle(m_Device);
	}

	VkPhysicalDeviceProperties Device::GetPhysicalDeviceProperties()
	{
		VkPhysicalDeviceProperties properties{};
		m_Iloader.vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

		return properties;
	}

	VkDevice Device::Get() const
	{
		return m_Device;
	}

	VkPhysicalDevice Device::GetPhysicalDevice() const
{
		return m_PhysicalDevice;
	}

	const DeviceLoader& Device::GetLoader() const
	{
		return m_Loader;
	}

	void Device::PickDevice()
	{
		uint32_t deviceCount = 0;
		m_Iloader.vkEnumeratePhysicalDevices(m_Instance.Get(), &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("No vulkan devices found");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		m_Iloader.vkEnumeratePhysicalDevices(m_Instance.Get(), &deviceCount,
			devices.data());

		for (VkPhysicalDevice device : devices)
			if (IsDeviceSuitable(device))
			{
				m_PhysicalDevice = device;
				return;
			}

		throw std::runtime_error("No suitable device found");
	}

	bool Device::IsDeviceSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		m_Iloader.vkGetPhysicalDeviceProperties(device, &deviceProperties);
		m_Iloader.vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		bool extentionsSupported = CheckDeviceExtentionSupport(device,
			deviceExtensions.data(), deviceExtensions.size());

		// kinda hacky. if this physical device isn't chosen, this function will
		// run again and overwrite m_Indices, so it will still choose the
		// correct queue family indices. 
		// TLDR; it works, it just doesn't look like it does.
		FindQueueFamilies(device);

		return
			deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
			&& m_Indices.hasGraphicsFamily
			&& extentionsSupported;
	}

	bool Device::CheckDeviceExtentionSupport(VkPhysicalDevice device,
		const char* const* deviceExtentions, uint64_t extentionCount)
	{
		// The device requires some extentions (such as the swap chain)
		// We need to check for those here before we can use the device.

		uint32_t count;
		m_Iloader.vkEnumerateDeviceExtensionProperties(device, nullptr, &count,
			nullptr);

		std::vector<VkExtensionProperties> availableExtentions(count);
		m_Iloader.vkEnumerateDeviceExtensionProperties(device, nullptr, &count,
			availableExtentions.data());

		std::vector<std::string> requiredExtentions(extentionCount);
		for (uint64_t i = 0; i < extentionCount; i++)
			requiredExtentions[i] = std::string(deviceExtentions[i]);

		std::set<std::string> requiredExtentionSet(requiredExtentions.begin(),
			requiredExtentions.end());

		for (size_t i = 0; i < availableExtentions.size(); i++)
			requiredExtentionSet.erase(availableExtentions[i].extensionName);

		return requiredExtentionSet.empty();
	}

	void Device::FindQueueFamilies(VkPhysicalDevice device)
	{
		uint32_t familyCount = 0;
		m_Iloader.vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

		if (familyCount == 0)
			return;

		std::vector<VkQueueFamilyProperties> families(familyCount);
		m_Iloader.vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount,
			families.data());

		for (size_t i = 0; i < families.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = families[i];

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
				&& !m_Indices.hasGraphicsFamily)
			{
				m_Indices.hasGraphicsFamily = true;
				m_Indices.graphicsFamilyIndex = static_cast<uint32_t>(i);
			}

			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT
				&& !m_Indices.hasTransferFamily)
			{
				m_Indices.hasTransferFamily = true;
				m_Indices.transferFamilyIndex = static_cast<uint32_t>(i);
			}

			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT
				&& !m_Indices.hasComputeFamily)
			{
				m_Indices.hasComputeFamily = true;
				m_Indices.computeFamilyIndex = static_cast<uint32_t>(i);
			}
		}

		// Try to find a dedicated transfer queue
		for (size_t i = 0; i < families.size(); i++)
		{
			if (!(families[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
				continue;

			if (m_Indices.hasGraphicsFamily && m_Indices.graphicsFamilyIndex == i)
				continue;

			m_Indices.transferFamilyIndex = static_cast<uint32_t>(i);
			break;
		}
	}

	QueueFamilyIndices Device::GetQueueFamilyIndices() const
	{
		return m_Indices;
	}
}
