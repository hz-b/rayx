#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "Beamline/Beamline.h"
#include "RenderObject.h"
#include "UserInterface/Settings.h"
#include <Tracer/DeviceTracer.h>

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
        std::optional<Texture::TextureInput> textureInput;
    };

    void buildRayCache(UIRayInfo& rayInfo, const RAYX::BundleHistory& rays);
    void buildRaysRObject(const RAYX::Beamline& beamline, UIRayInfo& rayInfo, std::shared_ptr<DescriptorSetLayout> setLayout,
                          std::shared_ptr<DescriptorPool> descriptorPool);
    std::vector<RenderObjectInput> getRObjectInputs(const std::vector<RAYX::DesignElement> elements, const RAYX::BundleHistory& rays) const;
    void buildRObjectsFromInput(std::vector<RenderObjectInput>&& inputs, std::shared_ptr<DescriptorSetLayout> setLayout,
                                std::shared_ptr<DescriptorPool> descriptorPool);

    void resetRayRObject() { m_RayRObjects.clear(); }

  private:
    Device& m_Device;

    std::vector<RenderObject> m_ElementRObjects = {};
    std::vector<RenderObject> m_RayRObjects = {};
    RAYX::BundleHistory m_rayCache = {};
};
