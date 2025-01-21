#include <iostream>
#include <set>
#include <vector>

#include "context.hpp"
#include "surface.hpp"

#include "device.hpp"

Device::Device(const Context &cx, VkPhysicalDevice base, const Surface *surface) : cx(cx), surface(surface)
{
    physicalHandle = base;
    graphicsFamilyIndex = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
    if (this->surface)
        presentFamilyIndex = findPresentQueueFamilyIndex();
}

Device::~Device()
{
    vkDestroyCommandPool(*handle, commandPool, nullptr);
    vkDestroyCommandPool(*handle, commandPoolTransient, nullptr);

    vkDestroyDevice(*handle, nullptr);
}

void Device::initLogicalDevice()
{
    std::set<uint32_t> uniqueQueueFamilies;

    if (graphicsFamilyIndex.has_value())
        uniqueQueueFamilies.insert(graphicsFamilyIndex.value());
    if (presentFamilyIndex.has_value())
        uniqueQueueFamilies.insert(presentFamilyIndex.value());

    float queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                   .queueFamilyIndex = queueFamily,
                                                   .queueCount = 1,
                                                   .pQueuePriorities = &queuePriority};
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = static_cast<uint32_t>(cx.getLayerCount()),
        .ppEnabledLayerNames = cx.getLayers(),
        .enabledExtensionCount = static_cast<uint32_t>(cx.getDeviceExtensionCount()),
        .ppEnabledExtensionNames = cx.getDeviceExtensions(),
    };

    // create device
    VkDevice handle;
    if (vkCreateDevice(physicalHandle, &createInfo, nullptr, &handle) != VK_SUCCESS)
    {
        std::cerr << "Failed to create logical device" << std::endl;
        return;
    }
    else
    {
        this->handle = std::make_unique<VkDevice>(handle);
    }

    // queue

    vkGetDeviceQueue(handle, graphicsFamilyIndex.value(), 0, &graphicsQueue);
    if (surface)
        vkGetDeviceQueue(handle, presentFamilyIndex.value(), 0, &presentQueue);

    // command pools

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphicsFamilyIndex.value(),
    };
    VkResult res = vkCreateCommandPool(handle, &commandPoolCreateInfo, nullptr, &commandPool);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to create command pool : " << res << std::endl;
        return;
    }

    VkCommandPoolCreateInfo commandPoolTransientCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = graphicsFamilyIndex.value(),
    };
    res = vkCreateCommandPool(handle, &commandPoolTransientCreateInfo, nullptr, &commandPoolTransient);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to create transient command pool : " << res << std::endl;
        return;
    }
}

std::optional<uint32_t> Device::findMemoryTypeIndex(VkMemoryRequirements requirements,
                                                    VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProp;
    vkGetPhysicalDeviceMemoryProperties(physicalHandle, &memProp);

    for (uint32_t i = 0; i < memProp.memoryTypeCount; ++i)
    {
        bool rightType = requirements.memoryTypeBits & (1 << i);
        bool rightFlag = (memProp.memoryTypes[i].propertyFlags & properties) == properties;
        if (rightType && rightFlag)
            return std::optional<uint32_t>(i);
    }

    std::cerr << "Failed to find suitable memory type" << std::endl;
    return std::optional<uint32_t>();
}

std::vector<VkQueueFamilyProperties> Device::getQueueFamilyProperties() const
{
    uint32_t queueFamilyPropertiesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalHandle, &queueFamilyPropertiesCount, nullptr);
    std::vector<VkQueueFamilyProperties> out(queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalHandle, &queueFamilyPropertiesCount, out.data());
    return out;
}

std::optional<uint32_t> Device::findQueueFamilyIndex(const VkQueueFlags &capabilities) const
{
    auto props = getQueueFamilyProperties();
    for (uint32_t i = 0; i < props.size(); ++i)
    {
        if (props[i].queueFlags & capabilities)
            return std::optional<uint32_t>(i);
    }
    return std::optional<uint32_t>();
}
std::optional<uint32_t> Device::findPresentQueueFamilyIndex() const
{
    if (!surface)
        return std::optional<uint32_t>();

    auto props = getQueueFamilyProperties();
    for (uint32_t i = 0; i < props.size(); ++i)
    {
        VkBool32 supported;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalHandle, i, surface->getHandle(), &supported);
        if (supported)
            return std::optional<uint32_t>(i);
    }
    return std::optional<uint32_t>();
}