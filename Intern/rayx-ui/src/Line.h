#pragma once

#include <glm/glm.hpp>

#include "Shared/EventType.h"

class Line {
  public:
    Line(/* args */);
    ~Line();

  private:
    glm::vec4 m_start;
    glm::vec4 m_intersection;
    double m_eventType;
};