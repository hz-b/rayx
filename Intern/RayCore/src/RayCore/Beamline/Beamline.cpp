#include "Beamline.h"
#include "Debug.h"

#include <iostream>

namespace RAY
{

    void Beamline::addOpticalElement(const std::shared_ptr<OpticalElement> q) {
        addOpticalElement(q->getName(), q->getSurfaceParams(), q->getInMatrix(), q->getOutMatrix(), q->getTempMisalignmentMatrix(), q->getInverseTempMisalignmentMatrix(), q->getObjectParameters(), q->getElementParameters());
    }

    /* Somehow results in wrong values. Should be fixed later
    void Beamline::addQuadric(Quadric newObject)
    {
        m_Objects.push_back(newObject);
    }
    */
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

} // namespace RAY