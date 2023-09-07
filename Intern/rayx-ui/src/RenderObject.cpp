#include "RenderObject.h"

RenderObjectVec getRenderData(const std::filesystem::path& filename) {
    RenderObjectVec data;
    std::vector<RAYX::OpticalElement> elements;
    std::vector<std::shared_ptr<RAYX::LightSource>> sources;
    {
        auto beamline = RAYX::importBeamline(filename);
        elements = beamline.m_OpticalElements;
        sources = beamline.m_LightSources;
    }
    for (auto element : elements) {
        RenderObject d;
        d.name = element.m_name;
        if (d.name == "ImagePlane") {  // TODO: dirty hack that relies on name, this should be fixed
            d.type = 10;
        } else {
            d.type = 0;
        }
        dmat4 outTrans = element.m_element.m_outTrans;
        d.position = dvec4(outTrans[3][0], outTrans[3][1], outTrans[3][2], 1.0);
        for (int i = 0; i < 16; i++) {
            d.surface[i] = element.m_element.m_surface.m_params[i];
        }
        dmat4 rotation;
        rotation[0] = dvec4(outTrans[0][0], outTrans[0][1], outTrans[0][2], 0.0);
        rotation[1] = dvec4(outTrans[1][0], outTrans[1][1], outTrans[1][2], 0.0);
        rotation[2] = dvec4(outTrans[2][0], outTrans[2][1], outTrans[2][2], 0.0);
        rotation[3] = dvec4(0.0, 0.0, 0.0, 1.0);
        d.orientation = rotation;
        d.cutout = element.m_element.m_cutout;
        data.push_back(d);
    }
    return data;
}