#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include "instance.hpp"

#define PFN_DECLARE_VK(funcName) PFN_##funcName funcName

class Device;

class Context
{
  private:
    std::vector<const char *> m_layers;
    std::vector<const char *> m_instanceExtensions;
    std::vector<const char *> m_deviceExtensions;

  public:
    std::unique_ptr<Instance> m_instance;

  public:
    Context();

    inline int getLayerCount() const
    {
        return m_layers.size();
    }
    inline const char *const *getLayers() const
    {
        return m_layers.data();
    }
    inline int getInstanceExtensionCount() const
    {
        return m_instanceExtensions.size();
    }
    inline const char *const *getInstanceExtensions() const
    {
        return m_instanceExtensions.data();
    }
    inline int getDeviceExtensionCount() const
    {
        return m_deviceExtensions.size();
    }
    inline const char *const *getDeviceExtensions() const
    {
        return m_deviceExtensions.data();
    }

    std::vector<VkPhysicalDevice> getAvvailablePhysicalDevices() const;

};