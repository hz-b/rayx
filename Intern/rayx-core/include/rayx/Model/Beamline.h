#pragma once

#include <string>
#include <vector>

#include "Element.h"
#include "Source.h"

namespace rayx::detail::model {

struct BeamlineSource {
    std::string name;
    int objectId;
    glm::dmat4 inMat;
    glm::dmat4 outMat;

    model::Source source;
};

struct BeamlineElement {
    std::string name;
    int objectId;
    glm::dmat4 inMat;
    glm::dmat4 outMat;

    model::SurfaceElement element;
};

struct Beamline {
    std::string name;
    std::vector<model::BeamlineSource> sources;
    std::vector<model::BeamlineElement> elements;
};

}  // namespace rayx::detail::model
