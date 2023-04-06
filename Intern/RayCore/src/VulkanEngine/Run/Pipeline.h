#ifndef NO_VULKAN

#pragma once

namespace RAYX {
class RAYX_API Pipeline {
    Pipeline();
    ~Pipeline();

  private:
    void virtual bind();
};

class RAYX_API ComputePipeline : public Pipeline {
    ComputePipeline();
    ~ComputePipeline();
};
}  // namespace RAYX
#endif