#pragma once

#include "RenderObject.h"

class TexturedRenderObject : public RenderObject {
  public:
    TexturedRenderObject(std::string name, Device& device, glm::mat4 modelMatrix, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                         std::shared_ptr<DescriptorSetLayout> setLayout, std::shared_ptr<DescriptorPool> descriptorPool);

    void updateTexture(const std::filesystem::path& path);
    void updateTexture(const unsigned char* data, uint32_t width, uint32_t height);

  private:
    void RenderObject::createDescriptorSet();

    Texture m_Texture;

    std::shared_ptr<DescriptorSetLayout> m_setLayout;
    std::shared_ptr<DescriptorPool> m_descriptorPool;

    bool m_isTextured;
    VkDescriptorSet m_descrSet;

    std::vector<Texture> textures;
};
