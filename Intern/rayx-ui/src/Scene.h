#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "Beamline/Beamline.h"
#include "RenderObject.h"
#include "Tracer/Tracer.h"
#include "UserInterface/Settings.h"

class Scene {
  public:
    Scene(Device& device);
    enum class State { Empty, BuiltRayRObject, BuiltElementRObjs, Complete };

    const std::vector<RenderObject>& getRObjects() const { return m_ElementRObjects; }
    const std::vector<RenderObject>& getRaysRObject() const { return m_RayRObjects; }

    struct RenderObjectInput {
        glm::mat4 modelMatrix;
        std::vector<TextureVertex> vertices;
        std::vector<uint32_t> indices;
    };

    void buildRayCache(UIRayInfo& rayInfo, const RAYX::BundleHistory& rays);
    void buildRaysRObject(const RAYX::Beamline& beamline, UIRayInfo& rayInfo, std::shared_ptr<DescriptorSetLayout> setLayout,
                          std::shared_ptr<DescriptorPool> descriptorPool);

    std::vector<Scene::RenderObjectInput> getRObjectInputs(const std::vector<RAYX::DesignElement> elements,
                                                           const std::vector<std::vector<RAYX::Ray>> sortedRays, bool buildTexture);

    void buildRObjectsFromInput(std::vector<RenderObjectInput>&& inputs, std::shared_ptr<DescriptorSetLayout> setLayout,
                                std::shared_ptr<DescriptorPool> descriptorPool, bool buildTexture);

    void resetRayRObject() { m_RayRObjects.clear(); }

  private:
    Device& m_Device;

    std::vector<RenderObject> m_ElementRObjects = {};
    std::vector<RenderObject> m_RayRObjects = {};
    BundleHistory m_rayCache = {};
    mutable std::vector<Texture::TextureInput> m_textureInputCache;  ///< Texture cache
};
