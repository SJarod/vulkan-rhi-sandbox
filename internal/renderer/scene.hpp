#pragma once

#include <memory>
#include <vector>

class Mesh;

class Scene
{
  public:
    std::vector<std::unique_ptr<Mesh>> objects;

  public:
    Scene(const std::shared_ptr<Device> device);
    ~Scene();
};