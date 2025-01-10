#pragma once

#include <memory>

#include <volk.h>

class Context;

class Instance
{
private:
    std::unique_ptr<VkInstance> m_handle;

public:
    Instance(const Context& cx);

    public:
    inline VkInstance getInstance() const { return *m_handle; }
};