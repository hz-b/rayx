#include "Beamline.h"
#include "Debug.h"

#include <iostream>
#include <memory>

namespace RAYX
{
    Beamline::Beamline()
    {
    }

    Beamline::~Beamline()
    {

    }

    // push copy of shared pointer to m_objects vector
    void Beamline::addOpticalElement(const std::shared_ptr<OpticalElement> q) {
        m_Objects.push_back(std::static_pointer_cast<BeamlineObject>(q));
    }

    void Beamline::addOpticalElement(const char* name, const std::vector<double>& inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> OParameters, std::vector<double> EParameters)
    {
        m_Objects.emplace_back(std::static_pointer_cast<BeamlineObject>(std::make_shared<OpticalElement>(
            name, inputPoints, inputInMatrix, inputOutMatrix, misalignmentMatrix, inverseMisalignmentMatrix, OParameters, EParameters
        )));
    }

    void Beamline::addOpticalElement(const char* name, std::vector<double>&& inputPoints, std::vector<double>&& inputInMatrix, std::vector<double>&& inputOutMatrix, std::vector<double>&& misalignmentMatrix, std::vector<double>&& inverseMisalignmentMatrix, std::vector<double>&& OParameters, std::vector<double>&& EParameters)
    {
        m_Objects.emplace_back(std::static_pointer_cast<BeamlineObject>(std::make_shared<OpticalElement>(
            name, std::move(inputPoints), std::move(inputInMatrix), std::move(inputOutMatrix), std::move(misalignmentMatrix), std::move(inverseMisalignmentMatrix), std::move(OParameters), std::move(EParameters)
        )));
    }

    // TODO(rudi): this still returns OpticalElement to not disrupt the rest of the system.
    // it should soon return BeamlineObject!
    std::vector<std::shared_ptr<OpticalElement>> Beamline::getObjects() const
    {
        std::vector<std::shared_ptr<OpticalElement>> elems;
        for (size_t i = 0; i < m_Objects.size(); i++) {
            std::shared_ptr<OpticalElement> e = std::dynamic_pointer_cast<OpticalElement>(m_Objects[i]);
            if (e) { elems.push_back(e); }
        }
        return elems;
    }

    int Beamline::size() const {
        return m_Objects.size();
    }

} // namespace RAYX