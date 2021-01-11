#include "Beamline.h"
#include "Debug.h"

#include <iostream>

namespace RAY
{

    Beamline::Beamline()
    {
        DEBUG(std::cout << "Creating Beamline..." << std::endl);
        //std::vector<double> temp(16, 1);
        //BeamLineObject firstQuadric(temp);
        //m_objects.push_back(firstQuadric);
    }

    Beamline::~Beamline()
    {
        DEBUG(std::cout << "Deleting Beamline..." << std::endl);
    }

    /* Somehow results in wrong values. Should be fixed later
    void Beamline::addBeamlineObject(BeamLineObject newObject)
    {
        m_Objects.push_back(newObject);
    }
    */
    void Beamline::addQuadric(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix)
    {
        m_Objects.emplace_back(inputPoints, inputInMatrix, inputOutMatrix);
    }

    void Beamline::replaceNthObject(uint32_t index, Quadric newObject)
    {
        assert(m_Objects.size() >= index);
        m_Objects[index] = newObject;
    }

    std::vector<Quadric> Beamline::getObjects()
    {
        return m_Objects;
    }

} // namespace RAY