#include "Beamline.h"

#include <iostream>

#include "Debug/Instrumentor.h"
#include "Model/Beamline/Objects/Objects.h"

namespace RAYX {
Beamline::Beamline() {}

Beamline::~Beamline() {}

// push copy of shared pointer to m_objects vector
void Beamline::addOpticalElement(const std::shared_ptr<OpticalElement> q) {
    RAYX_PROFILE_FUNCTION();
    m_OpticalElements.push_back(q);
}

void Beamline::addOpticalElement(const char* name,
                                 const std::array<double, 4 * 4>& inputPoints,
                                 std::array<double, 4 * 4> inputInMatrix,
                                 std::array<double, 4 * 4> inputOutMatrix,
                                 std::array<double, 4 * 4> OParameters,
                                 std::array<double, 4 * 4> EParameters) {
    RAYX_PROFILE_FUNCTION();
    m_OpticalElements.emplace_back(std::make_shared<OpticalElement>(
        name, inputPoints, inputInMatrix, inputOutMatrix, OParameters,
        EParameters));
}

void Beamline::addOpticalElement(const char* name,
                                 std::array<double, 4 * 4>&& inputPoints,
                                 std::array<double, 4 * 4>&& inputInMatrix,
                                 std::array<double, 4 * 4>&& inputOutMatrix,
                                 std::array<double, 4 * 4>&& OParameters,
                                 std::array<double, 4 * 4>&& EParameters) {
    RAYX_PROFILE_FUNCTION();
    m_OpticalElements.emplace_back(std::make_shared<OpticalElement>(
        name, std::move(inputPoints), std::move(inputInMatrix),
        std::move(inputOutMatrix), std::move(OParameters),
        std::move(EParameters)));
}

}  // namespace RAYX