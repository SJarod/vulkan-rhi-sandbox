#include "device.hpp"
#include <iostream>
#include <set>

#include "context.hpp"

void Context::finishCreateContext()
{
    m_instance = std::make_unique<Instance>(*this);
}

void Context::addLayer(const char *layer)
{
    m_layers.push_back(layer);
}

void Context::addInstanceExtension(const char *extension)
{
    m_instanceExtensions.push_back(extension);
}

void Context::addDeviceExtension(const char *extension)
{
    m_deviceExtensions.push_back(extension);
}

std::vector<VkPhysicalDevice> Context::getAvailablePhysicalDevices() const
{
    uint32_t count;
    vkEnumeratePhysicalDevices(m_instance->getHandle(), &count, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(count);
    vkEnumeratePhysicalDevices(m_instance->getHandle(), &count, physicalDevices.data());
    return physicalDevices;
}