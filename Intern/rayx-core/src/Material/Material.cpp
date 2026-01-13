#include "Material.h"

#ifdef _WIN32
#include <string.h>
#else
#include <strings.h>
#endif

#include "Debug/Debug.h"
#include "NffTable.h"
#include "PalikTable.h"
#include "CromerTable.h"
#include "MolecTable.h"

namespace RAYX {

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

int getMaterialAtomicNumber(Material m) {
    switch (m) {
        case Material::VACUUM:
            return -1;
        case Material::REFLECTIVE:
            return -2;
#define X(e, z, a, rho) \
    case Material::e:   \
        return z;
#include "materials.xmacro"
#undef X
    }
    RAYX_EXIT << "unknown material in getMaterialAtomicNumber()!";
    return -1;
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

MaterialTables loadMaterialTables(std::array<bool, 133> relevantMaterials) {
    MaterialTables out;

    auto mats = allNormalMaterials();
    if (mats.size() != 133) {
        RAYX_EXIT << "unexpected number of materials. this is a bug.";
    }

    // add palik table content
    for (size_t i = 0; i < mats.size(); i++) {
        out.indices.push_back(out.materials.size());
        if (relevantMaterials[i]) {
            PalikTable t;

            if (!PalikTable::load(getMaterialName(mats[i]), &t)) {
                RAYX_VERB << "could not load PalikTable!";
            }

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
        Material mat = mats[i];
        if (relevantMaterials[i]) {
            NffTable t;

            if (!NffTable::load(getMaterialName(mat), &t)) {
                RAYX_VERB << "could not load NffTable!";
            }

            glm::dvec2 massAndRho = getAtomicMassAndRho(getMaterialAtomicNumber(mat));
            double mass = massAndRho.x;
            double rho = massAndRho.y;

            for (auto x : t.m_Lines) {
                double en = x.m_energy;
                double n = 1 - (415.252 * rho * x.m_f1) / (en * en * mass);
                double k = (415.252 * rho * x.m_f2) / (en * en * mass);
                NKEntry nk;
                nk.m_energy = en;
                nk.m_n = n;
                nk.m_k = k;
                out.materials.push_back(nk.m_energy);
                out.materials.push_back(nk.m_n);
                out.materials.push_back(nk.m_k);
            }
        }
    }

    // add cromer table content
    for (size_t i = 0; i < mats.size(); i++) {
        out.indices.push_back(out.materials.size());
        if (relevantMaterials[i]) {
            CromerTable t;

            if (!CromerTable::load(getMaterialName(mats[i]), &t)) {
                RAYX_VERB << "could not load CromerTable!";
                continue;
            }

            glm::dvec2 massAndRho = getAtomicMassAndRho(i);
            double mass = massAndRho.x;
            double rho = massAndRho.y;

            for (auto x : t.m_Lines) {
                double en = x.m_energy;
                double n = 1 - (415.252 * rho * x.m_f1) / (en * en * mass);
                double k = (415.252 * rho * x.m_f2) / (en * en * mass);
                NKEntry nk;
                nk.m_energy = en;
                nk.m_n = n;
                nk.m_k = k;
                out.materials.push_back(nk.m_energy);
                out.materials.push_back(nk.m_n);
                out.materials.push_back(nk.m_k);
            }
        }
    }
    
    
    // add molec table content now
    for (size_t i = 0; i < mats.size(); i++) {
        out.indices.push_back(out.materials.size());
        if(relevantMaterials[i]) {   
            MolecTable t;
            if (!MolecTable::load(getMaterialName(mats[i]), &t)) {
                RAYX_VERB << "could not load MolecTable!";
                continue;
            }

            for (auto x : t.m_Lines) {
                out.materials.push_back(x.m_energy);
                out.materials.push_back(x.m_n);
                out.materials.push_back(x.m_k);
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

// returns dvec2(atomic mass, density) extracted from materials.xmacro
glm::dvec2 getAtomicMassAndRho(int material) {
    // This is an "X-Macro", see https://en.wikipedia.org/wiki/X_macro
    // It allows us to generate a `case` for each material in the materials.xmacro file.
    // The `case` matches upon the atomic number of the element, and returns the atomic mass and density as specified in the materials.xmacro file.
    switch (material) {
#define X(e, z, a, rho) \
    case z:             \
        return glm::dvec2(a, rho);
#include "../Material/materials.xmacro"
#undef X
    }
    RAYX_VERB << "invalid material index " << material;
    _throw("invalid material in getAtomicMassAndRho");
    return glm::dvec2(0.0, 0.0);
}

}  // namespace RAYX