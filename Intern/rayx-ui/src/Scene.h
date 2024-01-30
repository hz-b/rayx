#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "RenderObject.h"
#include "Beamline/Beamline.h"
#include "Tracer/Tracer.h"
#include "UserInterface/Settings.h"

class Scene {
  public:
    Scene(Device& device);
    enum class State { Empty, BuiltRayRObject, BuiltElementRObjs, Complete };

    const std::vector<RenderObject>& getRObjects() const { return m_ElementRObjects; }
    const std::optional<RenderObject>& getRaysRObject() const { return m_RaysRObject; }

    struct RenderObjectInput {
        glm::mat4 modelMatrix;
        std::vector<TextureVertex> vertices;
        std::vector<uint32_t> indices;
        std::optional<Texture::TextureInput> textureInput;
    };

    void buildRayCache(UIRayInfo& rayInfo, const RAYX::BundleHistory& rays);
    void buildRaysRObject(const RAYX::Beamline& beamline, UIRayInfo& rayInfo, std::shared_ptr<DescriptorSetLayout> setLayout,
                          std::shared_ptr<DescriptorPool> descriptorPool);
    std::vector<RenderObjectInput> getRObjectInputs(const std::vector<RAYX::OpticalElement> elements, const RAYX::BundleHistory& rays) const;
    void buildRObjectsFromInput(std::vector<RenderObjectInput>&& inputs, std::shared_ptr<DescriptorSetLayout> setLayout,
                                std::shared_ptr<DescriptorPool> descriptorPool);

    State getState() const { return m_State; }
    void resetRayRObject() { m_RaysRObject.reset(); }

  private:
    Device& m_Device;
    State m_State = State::Empty;

    std::vector<RenderObject> m_ElementRObjects = {};
    std::optional<RenderObject> m_RaysRObject = {};
    BundleHistory m_rayCache = {};
};
