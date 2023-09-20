#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	window = newWindow;

	try {
		createInstance();
		createSurface();	
		getPhysicalDevice();
		createLogicalDevice();
	}
	catch (const std::runtime_error& e)
	{
		printf("ERROR %s\n ", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::cleanup()
{
	vkDestroySurfaceKHR(vkInstance, surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
}

void VulkanRenderer::createInstance()
{
	//Create info for application
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Course";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "QK engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	//Create vk instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	//Get GLFW required extensions
	uint32_t glfwExtensionsCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	for (int i = 0; i < glfwExtensionsCount; i++)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}
	if (!checkInstanceExtensionSupported(&instanceExtensions))
	{
		throw std::runtime_error("Not support require vk extensions");
	}
	//Create info for vk instance
	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = nullptr;

	//Create vk instance
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vkInstance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vk instance");
	}
}

void VulkanRenderer::createLogicalDevice()
{
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	//
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily }; 

	for (int queueFamilyIndex : queueFamilyIndices)
	{
		//Queue the logical device needs to create
		VkDeviceQueueCreateInfo queueInfo = {};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = queueFamilyIndex;
		queueInfo.queueCount = 1;
		float priority = 1.0f;
		queueInfo.pQueuePriorities = &priority;

		queueCreateInfos.push_back(queueInfo);
	}



	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.queueCreateInfoCount = static_cast<int32_t>(queueCreateInfos.size());
	deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceInfo.enabledExtensionCount = static_cast<int32_t>(deviceExtensions.size());
	deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
	//deviceInfo.enabledLayerCount = 0;
	//deviceInfo.ppEnabledLayerNames = nullptr;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceInfo.pEnabledFeatures = &deviceFeatures;

	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceInfo, nullptr, &mainDevice.logicalDevice);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}

	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);
}

void VulkanRenderer::createSurface()
{
	//create Surface
	VkResult result = glfwCreateWindowSurface(vkInstance, window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create surface");
	}
}

void VulkanRenderer::getPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Cant find GPU supported vulkan");
	}

	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, deviceList.data());

	for (const auto& device : deviceList)
	{
		if (checkDeviceSuitable(device))
		{
			mainDevice.physicalDevice = device;
			break;
		}
	}
}

bool VulkanRenderer::checkInstanceExtensionSupported(std::vector<const char*>* extensions)
{
	uint32_t extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

	if (extensionsCount == 0)
	{
		return false;
	}

	std::vector<VkExtensionProperties> extensionProperties(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensionProperties.data());

	for (const auto& extension : *extensions)
	{
		bool hasExtension = false;
		for (const auto& extensionProperty : extensionProperties)
		{
			if (strcmp(extension, extensionProperty.extensionName))
			{
				hasExtension = true;
				break;
			}
		}
		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceExtensionSupported(VkPhysicalDevice device)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	if (extensionCount == 0)
	{
		return false;
	}

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

	for (const auto& deviceExtension : deviceExtensions)
	{
		bool hasExtensions = false;
		for (const auto& extension : extensions)
		{
			if (strcmp(deviceExtension, extension.extensionName) == 0)
			{
				hasExtensions = true;
				break;
			}
		}
		if (!hasExtensions)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{

	////Device info (id, name, type, vendor, etc)
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//
	////Device features (geo shader, tess shader, wide lines, etc
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	QueueFamilyIndices indices = getQueueFamilies(device);

	bool extensionSupported = checkDeviceExtensionSupported(device);

	bool swapchainValid = false;
	if (extensionSupported)
	{
		SwapchainDetails swapchainDetails = getSwapchainDetails(device);
		swapchainValid = !swapchainDetails.presentationModes.empty() && !swapchainDetails.surfaceFormats.empty();
	}

	return indices.isValid() && extensionSupported && swapchainValid;
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());
	int i = 0;
	//Go through each family queue and check if it has at least 1 required queue type
	for (const auto& queueFamily : queueFamilyList)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;

		}

		VkBool32 isPresentationSupported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &isPresentationSupported);
		
		if (queueFamily.queueCount > 0 && isPresentationSupported)
		{
			indices.presentationFamily = i;
		}

		if (indices.isValid())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapchainDetails VulkanRenderer::getSwapchainDetails(VkPhysicalDevice device)
{
	SwapchainDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.surfaceCapabilities);

	uint32_t formatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatsCount, nullptr);
	if (formatsCount != 0)
	{
		details.surfaceFormats.resize(formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatsCount, details.surfaceFormats.data());
	}

	uint32_t presentationModesCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModesCount, nullptr);
	if (formatsCount != 0)
	{
		details.presentationModes.resize(presentationModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModesCount, details.presentationModes.data());
	}


	return details;
}
