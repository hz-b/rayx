#include "Beamline.h"

#include <iostream>

#include "Model/Beamline/Objects/Objects.h"

namespace RAYX {
Beamline::Beamline() {}

Beamline::~Beamline() {}

// push copy of shared pointer to m_objects vector
void Beamline::addOpticalElement(const std::shared_ptr<OpticalElement> q) {
    m_OpticalElements.push_back(q);
}

void Beamline::addOpticalElement(const char* name,
                                 const std::vector<double>& inputPoints,
                                 std::vector<double> inputInMatrix,
                                 std::vector<double> inputOutMatrix,
                                 std::vector<double> OParameters,
                                 std::vector<double> EParameters) {
    m_OpticalElements.emplace_back(std::make_shared<OpticalElement>(
        name, inputPoints, inputInMatrix, inputOutMatrix, OParameters,
        EParameters));
}

void Beamline::addOpticalElement(const char* name,
                                 std::vector<double>&& inputPoints,
                                 std::vector<double>&& inputInMatrix,
                                 std::vector<double>&& inputOutMatrix,
                                 std::vector<double>&& OParameters,
                                 std::vector<double>&& EParameters) {
    m_OpticalElements.emplace_back(std::make_shared<OpticalElement>(
        name, std::move(inputPoints), std::move(inputInMatrix),
        std::move(inputOutMatrix), std::move(OParameters),
        std::move(EParameters)));
}

}  // namespace RAYX