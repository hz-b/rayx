#pragma once

#include "Core.h"
#include "glm.hpp"

#include <vector>

namespace RAY {

    class RAY_API Beamline {

    public:
        // struct Disk {
        //     glm::vec3 position;
        //     glm::vec3 normal;
        //     double radius;
        // };
        struct Quad {
            Quad(glm::vec3 pos, glm::vec3 norm, double sideLen) : position(pos), normal(norm), sideLength(sideLen) { };
            glm::vec3 position;
            glm::vec3 normal;
            double sideLength;
        };

        Beamline();
        ~Beamline();

    private:
        std::vector<Quad*> m_objects;

    };


}