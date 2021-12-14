#include "Material.h"

#include <assert.h>

#include <iostream>

#include "PalikTable.h"

const char* getMaterialName(Material m) {
    switch (m) {
#define X(e)          \
    case Material::e: \
        return #e;
#include "materials.xmacro"
#undef X
    }
    std::cerr << "unknown material in getMaterialName()!\n";
    assert(false);
    return nullptr;
}

std::vector<Material> allMaterials() {
    std::vector<Material> mats;
#define X(e) mats.push_back(Material::e);
#include "materials.xmacro"
#undef X
    return mats;
}

void fillMaterialTables(std::vector<int>* materialIndex,
                        std::vector<double>* material) {
    *materialIndex = std::vector<int>();
    *material = std::vector<double>();

    auto mats = allMaterials();
    for (uint i = 0; i < mats.size(); i++) {
        PalikTable t;
        assert(PalikTable::load(getMaterialName(mats[i]), &t));
        materialIndex->push_back(material->size());
        for (auto x : t.m_Lines) {
            material->push_back(x.m_energy);
            material->push_back(x.m_n);
            material->push_back(x.m_k);
            material->push_back(0);
        }
    }
}