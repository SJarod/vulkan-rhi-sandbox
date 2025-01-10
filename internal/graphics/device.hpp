#pragma once

#include <optional>
#include <vector>

#include <volk.h>

class Context;
class Surface;

class Device
{
  private:
    const Context &cx;

  public:
    // physical device
    VkPhysicalDevice physicalHandle;

    // logical device
    std::unique_ptr<VkDevice> handle;

    std::optional<uint32_t> graphicsFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;

  public:
    Device() = delete;
    Device(const Context &cx, VkPhysicalDevice base, const Surface *surface = nullptr);
    ~Device();

    std::vector<VkQueueFamilyProperties> getQueueFamilyProperties() const;

    std::optional<uint32_t> findQueueFamilyIndex(const VkQueueFlags &capabilities) const;
    std::optional<uint32_t> findPresentQueueFamilyIndex(const Surface *surface) const;

    void initLogicalDevice();
};