#include "Beamline.h"
#include "Debug.h"

#include <iostream>

namespace RAYX
{
    // push copy of shared pointer to m_objects vector
    void Beamline::addOpticalElement(const std::shared_ptr<OpticalElement> q) {
        m_Objects.push_back(q);
    }

    void Beamline::addOpticalElement(const char* name, const std::vector<double>& inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> OParameters, std::vector<double> EParameters)
    {
        m_Objects.emplace_back(std::make_shared<OpticalElement>(name, inputPoints, inputInMatrix, inputOutMatrix, misalignmentMatrix, inverseMisalignmentMatrix, OParameters, EParameters));
    }

    void Beamline::addOpticalElement(const char* name, std::vector<double>&& inputPoints, std::vector<double>&& inputInMatrix, std::vector<double>&& inputOutMatrix, std::vector<double>&& misalignmentMatrix, std::vector<double>&& inverseMisalignmentMatrix, std::vector<double>&& OParameters, std::vector<double>&& EParameters)
    {
        m_Objects.emplace_back(std::make_shared<OpticalElement>(name, std::move(inputPoints), std::move(inputInMatrix), std::move(inputOutMatrix), std::move(misalignmentMatrix), std::move(inverseMisalignmentMatrix), std::move(OParameters), std::move(EParameters)));
    }

    void Beamline::replaceNthObject(uint32_t index, std::shared_ptr<OpticalElement> newObject)
    {
        assert(m_Objects.size() >= index);
        m_Objects[index] = newObject;
    }

    std::vector<std::shared_ptr<OpticalElement>> Beamline::getObjects() const
    {
        return m_Objects;
    }

} // namespace RAYX