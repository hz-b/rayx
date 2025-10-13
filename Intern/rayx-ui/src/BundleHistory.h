#pragma once

#include <vector>

#include "Rays.h"

// TODO: this object oriented api to Ray is to be reworked
struct Ray {
    glm::dvec3 m_position;
    RAYX::EventType m_eventType;
    glm::dvec3 m_direction;
    double m_energy;
    RAYX::ElectricField m_field;
    double m_pathLength;
    int m_order;
    int m_lastElement;
    int m_sourceID;
};
static_assert(std::is_trivially_copyable_v<Ray>);
static_assert(std::is_default_constructible_v<Ray>);

using RayHistory = std::vector<Ray>;
using BundleHistory = std::vector<RayHistory>;

BundleHistory convertRaysToBundleHistory(RAYX::Rays rays, const int numSources);
