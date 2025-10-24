#include "Material.h"

#ifdef _WIN32
#include <string.h>
#else
#include <strings.h>
#endif

#include "Debug/Debug.h"
#include "NffTable.h"
#include "PalikTable.h"

namespace rayx {

/**
 * returns the name of the material:
 * EXAMPLE: getMaterialName(Material::Cu) == "CU"
 **/
const char* getMaterialName(Material m) {
    switch (m) {
        case Material::VACUUM:
            return "VACUUM";
        case Material::REFLECTIVE:
            return "REFLECTIVE";

#define X(e, z, a, rho) \
    case Material::e:   \
        return #e;
#include "materials.xmacro"
#undef X
    }
    RAYX_EXIT << "unknown material in getMaterialName()!";
    return nullptr;
}

// std::vector over all materials
std::vector<Material> allNormalMaterials() {
    std::vector<Material> mats;
#define X(e, z, a, rho) mats.push_back(Material::e);
#include "materials.xmacro"
#undef X
    return mats;
}

bool materialFromString(const char* matname, Material* out) {
    for (auto m : allNormalMaterials()) {
        const char* name = getMaterialName(m);
        if (strcasecmp(name, matname) == 0) {
            *out = m;
            return true;
        }
    }
    RAYX_EXIT << "materialFromString(): material \"" << matname << "\" not found";
    return false;
}

MaterialTables loadMaterialTables(std::array<bool, 92> relevantMaterials) {
    MaterialTables out;

    auto mats = allNormalMaterials();
    if (mats.size() != 92) { RAYX_EXIT << "unexpected number of materials. this is a bug."; }

    // add palik table content
    for (size_t i = 0; i < mats.size(); i++) {
        out.indices.push_back(out.materials.size());
        if (relevantMaterials[i]) {
            PalikTable t;

            if (!PalikTable::load(getMaterialName(mats[i]), &t)) { RAYX_EXIT << "could not load PalikTable!"; }

            for (auto x : t.m_Lines) {
                out.materials.push_back(x.m_energy);
                out.materials.push_back(x.m_n);
                out.materials.push_back(x.m_k);
            }
        }
    }

    // add nff table content
    for (size_t i = 0; i < mats.size(); i++) {
        out.indices.push_back(out.materials.size());
        if (relevantMaterials[i]) {
            NffTable t;

            if (!NffTable::load(getMaterialName(mats[i]), &t)) { RAYX_EXIT << "could not load NffTable!"; }

            for (auto x : t.m_Lines) {
                out.materials.push_back(x.m_energy);
                out.materials.push_back(x.m_f1);
                out.materials.push_back(x.m_f2);
            }
        }
    }

    // this extra index simplifies computation on the GPU, as then the table
    // within indices[i]..indices[i+1] can be used without checks.
    out.indices.push_back(out.materials.size());

    // materials can't be empty, because
    // Vulkan does not support empty buffers.
    if (out.materials.empty()) {
        // this number should never be accessed on the GPU.
        out.materials.push_back(0);
    }

    return out;
}

}  // namespace rayx
