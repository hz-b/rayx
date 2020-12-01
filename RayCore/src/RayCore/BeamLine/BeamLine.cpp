#include "Beamline.h"

#include <iostream>

namespace RAY {

    Beamline::Beamline() {
        Quad* quad = new Quad(glm::vec3(0,0,0), glm::vec3(1,1,1),1.0);
        m_objects.push_back(quad);
        std::cout << "Creating Beamline..." << std::endl;
    }

    Beamline::~Beamline() {
        std::cout << "Creating Beamline..." << std::endl;
    }
}