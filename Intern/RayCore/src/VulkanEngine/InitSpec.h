#pragma once

namespace RAYX {

template <typename T>
using dict = std::map<std::string, T>;

struct BufferSpec {
    uint32_t binding;
    bool in;
    bool out;
};

class InitSpec {
  public:
    InitSpec() = default;

    inline InitSpec buffer(const char* buffername, BufferSpec spec) {
        bufferSpecs[buffername] = spec;
        return *this;
    }

    inline InitSpec shader(const char* filename) {
        shaderfile = filename;
        return *this;
    }

    const char* shaderfile;
    dict<BufferSpec> bufferSpecs;
};

}  // namespace RAYX
