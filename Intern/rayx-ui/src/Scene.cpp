#include "Scene.h"

#include <algorithm>
#include <numeric>
#include <unordered_set>

#include "Debug/Instrumentor.h"
#include "Plotting.h"
#include "RayProcessing.h"
#include "RenderObject.h"
#include "Triangulation/GeometryUtils.h"
#include "Triangulation/Triangulate.h"
#include "Vertex.h"

Scene::Scene(Device& device) : m_Device(device) {}

void Scene::buildRayCache(UIRayInfo& rayInfo, const RAYX::BundleHistory& rays) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    rayInfo.maxAmountOfRays = (int)rays.size();
    if (rayInfo.renderAllRays) {
        m_rayCache = rays;
        return;
    } else {
        m_rayCache.clear();
    }
    const size_t m = getMaxEvents(rays);

    std::vector<size_t> indices(rays.size());
    std::iota(indices.begin(), indices.end(), 0);  // Filling indices with 0, 1, 2, ..., n-1

    // Randomly shuffling the indices
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::shuffle(indices.begin(), indices.end(), engine);

    std::unordered_set<size_t> selectedIndices;

    // Selecting rays for each event index
    for (size_t eventIdx = 0; eventIdx < m; ++eventIdx) {
        size_t count = 0;
        for (size_t rayIdx : indices) {
            if (count >= MAX_RAYS) break;
            if (rays[rayIdx].size() > eventIdx) {
                selectedIndices.insert(rayIdx);
                count++;
            }
        }
    }

    // Now selectedIndices contains unique indices of rays
    // Creating rayCache object from selected indices
    for (size_t idx : selectedIndices) {
        m_rayCache.push_back(rays[idx]);
    }

    rayInfo.maxAmountOfRays = m_rayCache.size();
}

void Scene::buildRaysRObject(const RAYX::Beamline& beamline, UIRayInfo& rayInfo, std::shared_ptr<DescriptorSetLayout> setLayout,
                             std::shared_ptr<DescriptorPool> descriptorPool) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    std::vector<Line> rays;
    if (!rayInfo.renderAllRays) {
        rays = getRays(m_rayCache, beamline, kMeansFilter, (uint32_t)rayInfo.amountOfRays, rayInfo.startEventID);
    } else {
        rays = getRays(m_rayCache, beamline, noFilter, (uint32_t)rayInfo.maxAmountOfRays, rayInfo.startEventID);
    }
    if (!rays.empty()) {
        // Temporarily aggregate all vertices, then create a single RenderObject
        std::vector<VertexVariant> rayVertices(rays.size() * 2);
        std::vector<uint32_t> rayIndices(rays.size() * 2);
        for (uint32_t i = 0; i < rays.size(); ++i) {
            rayVertices[i * 2] = VertexVariant(rays[i].v1);
            rayVertices[i * 2 + 1] = VertexVariant(rays[i].v2);
            rayIndices[i * 2] = i * 2;
            rayIndices[i * 2 + 1] = i * 2 + 1;
        }

        if (!m_RayRObjects.empty()) {
            m_RayRObjects[0].rebuild(rayVertices, rayIndices);
        } else {
            m_RayRObjects.emplace_back(m_Device, glm::mat4(1.0f), rayVertices, rayIndices, Texture(m_Device), setLayout, descriptorPool);
        }
    }
}

std::vector<Scene::RenderObjectInput> Scene::getRObjectInputs(const std::vector<RAYX::DesignElement> elements,
                                                              const RAYX::BundleHistory& rays) const {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<std::vector<RAYX::Ray>> sortedRays;
    sortRaysByElement(rays, sortedRays, elements.size());

    std::vector<RenderObjectInput> rObjectsInput;
    for (uint32_t i = 0; i < elements.size(); i++) {
        std::vector<TextureVertex> vertices;
        std::vector<uint32_t> indices;

        try {
            triangulateObject(elements[i], vertices, indices);
        } catch (const std::exception& ex) {
            RAYX_WARN << ex.what() << ". Object \"" << elements[i].getName()
                      << "\" can't be rendered due to triangulation issues. Make sure it is defined correctly in the RML file.";
            continue;  // Input is not generated --> Object won't be built/rendered
        }

        glm::mat4 modelMatrix = elements[i].compile().m_outTrans;

        if (vertices.size() == 4) {
            auto [width, height] = getRectangularDimensions(elements[i].compile().m_cutout);

            std::vector<std::vector<uint32_t>> footprint =
                makeFootprint(sortedRays[i], -width / 2, width / 2, -height / 2, height / 2, (uint32_t)(width * 10), (uint32_t)(height * 10));

            uint32_t footprintWidth, footprintHeight;
            std::unique_ptr<unsigned char[]> data = footprintAsImage(footprint, footprintWidth, footprintHeight);

            RenderObjectInput inputObject(modelMatrix, vertices, indices, Texture::TextureInput{std::move(data), footprintWidth, footprintHeight});
            rObjectsInput.emplace_back(std::move(inputObject));

        } else {
            RenderObjectInput inputObject(modelMatrix, vertices, indices, std::nullopt);
            rObjectsInput.emplace_back(std::move(inputObject));
        }
    }
    return rObjectsInput;
}

void Scene::buildRObjectsFromInput(std::vector<RenderObjectInput>&& inputs, std::shared_ptr<DescriptorSetLayout> setLayout,
                                   std::shared_ptr<DescriptorPool> descriptorPool) {
    assert(setLayout != nullptr && "Descriptor set layout is null");
    assert(descriptorPool != nullptr && "Descriptor pool is null");
    RAYX_PROFILE_FUNCTION_STDOUT();
    m_ElementRObjects.clear();
    for (const auto& input : inputs) {
        std::vector<VertexVariant> convertedVertices(input.vertices.begin(), input.vertices.end());
        if (input.textureInput.has_value()) {
            Texture texture(m_Device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                            VK_IMAGE_ASPECT_COLOR_BIT, {input.textureInput->width, input.textureInput->height});
            texture.updateFromData(input.textureInput->data.get(), input.textureInput->width, input.textureInput->height);
            m_ElementRObjects.emplace_back(m_Device, input.modelMatrix, convertedVertices, input.indices, std::move(texture), setLayout,
                                           descriptorPool);
        } else {
            m_ElementRObjects.emplace_back(m_Device, input.modelMatrix, convertedVertices, input.indices, Texture(m_Device), setLayout,
                                           descriptorPool);
        }
    }
}