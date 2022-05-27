#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core.h"
#include "rapidxml.hpp"

namespace RAYX {
/**
 * @brief This is an abstract parent class for all beamline objects used in
 * Ray-X.
 *
 * Beamline objects are used to define the beamline of a simulation. They are a
 * vital part of our model. Beamline objects can be mirrors, gratings, etc.
 * which represent real world parts of the beamline. Different beamline objects
 * come with different properties, such as reflectivity, transmission, etc.
 */
class RAYX_API BeamlineObject {
  public:
    ~BeamlineObject();

    /// Getter for the ID of the beamline object.
    const std::string& getName() const;

    const int m_ID;

  protected:
    BeamlineObject(const char* name);
    BeamlineObject();

  private:
    const std::string m_name;
    // m_geometry;
    // m_surfacePtr; //(for lightsource??)
};

}  // namespace RAYX