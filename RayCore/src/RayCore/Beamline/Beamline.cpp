#include "Beamline.h"
#include "Debug.h"

#include <iostream>

namespace RAY
{
    Beamline::Beamline()
    {
        DEBUG(std::cout << "Creating Beamline..." << std::endl);
        //std::vector<double> temp(16, 1);
        //Quadric firstQuadric(temp);
        //m_objects.push_back(firstQuadric);
    }

    Beamline::~Beamline()
    {
        DEBUG(std::cout << "Deleting Beamline..." << std::endl);
    }

    void Beamline::addQuadric(Quadric q) {
        addQuadric(q.getName(), q.getAnchorPoints(), q.getInMatrix(), q.getOutMatrix(), q.getTempMisalignmentMatrix(), q.getInverseTempMisalignmentMatrix(), q.getParameters());
    }

    /* Somehow results in wrong values. Should be fixed later
    void Beamline::addQuadric(Quadric newObject)
    {
        m_Objects.push_back(newObject);
    }
    */
    void Beamline::addQuadric(const char* name, std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> parameters)
    {
        m_Objects.emplace_back(name, inputPoints, inputInMatrix, inputOutMatrix, misalignmentMatrix, inverseMisalignmentMatrix, parameters);
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