#include "Material.h"

#include <Debug.h>
#include <strings.h>

#include <iostream>

#include "NffTable.h"

/**
 * returns the name of the material:
 * EXAMPLE: getMaterialName(Material::CU) == "CU"
 **/
const char* getMaterialName(Material m) {
    switch (m) {
        case Material::VACUUM:
            return "VACUUM";
        case Material::REFLECTIVE:
            return "REFLECTIVE";

#define X(e)          \
    case Material::e: \
        return #e;
#include "materials.xmacro"
#undef X
    }
    RAYX_ERR << "unknown material in getMaterialName()!";
    return nullptr;
}

// std::vector over all materials
std::vector<Material> allNormalMaterials() {
    std::vector<Material> mats;
#define X(e) mats.push_back(Material::e);
#include "materials.xmacro"
#undef X
    return mats;
}

// these tables are private to this file, only read-access is granted to others
// by the functions below
static std::vector<double> MATERIAL_TABLE;
static std::vector<int> MATERIAL_INDEX_TABLE;

// fills the tables above
void fillMaterialTables() {
    auto mats = allNormalMaterials();
    for (size_t i = 0; i < mats.size(); i++) {
        NffTable t;

        if (!NffTable::load(getMaterialName(mats[i]), &t)) {
            RAYX_ERR << "could not load NffTable!";
        }

        MATERIAL_INDEX_TABLE.push_back(MATERIAL_TABLE.size() /
                                       4);  // 4 doubles per Nff Entry
        for (auto x : t.m_Lines) {
            MATERIAL_TABLE.push_back(x.m_energy);
            MATERIAL_TABLE.push_back(x.m_f1);
            MATERIAL_TABLE.push_back(x.m_f2);
            MATERIAL_TABLE.push_back(0);
        }
    }
}

bool materialFromString(const char* matname, Material* out) {
    for (auto m : allNormalMaterials()) {
        const char* name = getMaterialName(m);
        if (strcasecmp(name, matname) != 0) {
            *out = m;
            return true;
        }
    }
    RAYX_ERR << "materialFromString(): material \"" << matname
             << "\" not found";
    return false;
}

const std::vector<double>* getMaterialTable() {
    if (MATERIAL_TABLE.empty()) {
        fillMaterialTables();
    }
    return &MATERIAL_TABLE;
}

const std::vector<int>* getMaterialIndexTable() {
    if (MATERIAL_TABLE.empty()) {
        fillMaterialTables();
    }
    return &MATERIAL_INDEX_TABLE;
}
