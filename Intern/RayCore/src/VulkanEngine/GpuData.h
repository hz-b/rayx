#pragma once

#include <vector>

const uint32_t STAGING_SIZE = 134217728;  // in bytes, equal to 128MB.

// in order to send data to the VulkanEngine, it needs to be converted to raw
// bytes (i.e. chars). the performance of those encode/decode can definitely be
// improved, as a copy is not generally necessary.

namespace RAYX {
struct GpuData {
    // the inner std::vectors are at most STAGING_SIZE large.
    std::vector<std::vector<char>> raw;

    inline uint32_t size() const {
        uint32_t s = 0;
        for (auto& x : raw) {
            s += x.size();
        }
        return s;
    }
};

// TODO optimize those functions!
template <typename T>
inline GpuData encode(std::vector<T> in) {
    uint32_t remaining_bytes = in.size() * sizeof(T);
    char* ptr = (char*)in.data();

    GpuData out;
    while (remaining_bytes > 0) {
        // number of bytes transferred in this for-loop
        int localbytes = std::min(STAGING_SIZE, remaining_bytes);

        std::vector<char> subdata(localbytes);
        memcpy(subdata.data(), ptr, localbytes);
        out.raw.push_back(subdata);

        ptr += localbytes;
        remaining_bytes -= localbytes;
    }
    return out;
}

template <typename T>
inline std::vector<T> decode(GpuData in) {
    std::vector<T> out;

    std::vector<char> tmp;
    while (!in.raw.empty()) {
        std::vector<char> f = in.raw[0];
        tmp.insert(tmp.end(), f.begin(), f.end());
        in.raw.erase(in.raw.begin());
        while (tmp.size() >= sizeof(T)) {
            T t;
            memcpy(&t, tmp.data(), sizeof(T));
            tmp.erase(tmp.begin(), tmp.begin() + sizeof(T));
            out.push_back(t);
        }
    }
    if (!tmp.empty()) {
        RAYX_ERR << "decode unsuccessful! remaining bytes are not enough "
                    "to create another element!";
    }
    return out;
}

}  // namespace RAYX
