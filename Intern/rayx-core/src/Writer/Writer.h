#pragma once

#include "Tracer/Ray.h"

using uint = unsigned int;

// When writing a Ray to CSV / H5, one needs to specify a way in which to format the ray.
// A format consists of multiple components - each component corresponds to a single double of data.
// Example: "Ray-ID|Event-ID". This simple format has two components.
// A ray formatted with this format only stores its ray-id and its event-id.
struct FormatComponent {
    // The name of the component, example: "X-position".
    const char* name;
    // A function pointer expressing how to access this component given an actual RAYX::Ray.
    double (*get_double)(uint ray_id, uint event_id, RAYX::Ray ray);
};

// Again, a format is simply a list of components!
using Format = std::vector<FormatComponent>;

// The default format string. It corresponds to FULL_FORMAT, see below.
std::string RAYX_API defaultFormatString();

// throws an error if it is unable to understand the format.
Format RAYX_API formatFromString(std::string);

// The "full" format, consisting of all components that rays support.
// All other formats are derived by picking a subset of these components, and potentially reordering them.
static Format FULL_FORMAT = {
    FormatComponent{
        .name = "Ray-ID",
        .get_double = [](uint ray_id, uint, RAYX::Ray) { return (double)ray_id; },
    },
    FormatComponent{
        .name = "Event-ID",
        .get_double = [](uint, uint event_id, RAYX::Ray) { return (double)event_id; },
    },
    FormatComponent{
        .name = "X-position",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_position.x; },
    },
    FormatComponent{
        .name = "Y-position",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_position.y; },
    },
    FormatComponent{
        .name = "Z-position",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_position.z; },
    },
    FormatComponent{
        .name = "Weight",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_weight; },
    },
    FormatComponent{
        .name = "X-direction",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_direction.x; },
    },
    FormatComponent{
        .name = "Y-direction",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_direction.y; },
    },
    FormatComponent{
        .name = "Z-direction",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_direction.z; },
    },
    FormatComponent{
        .name = "Energy",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_energy; },
    },
    FormatComponent{
        .name = "Stokes0",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_stokes[0]; },
    },
    FormatComponent{
        .name = "Stokes1",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_stokes[1]; },
    },
    FormatComponent{
        .name = "Stokes2",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_stokes[2]; },
    },
    FormatComponent{
        .name = "Stokes3",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_stokes[3]; },
    },
    FormatComponent{
        .name = "pathLength",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_pathLength; },
    },
    FormatComponent{
        .name = "order",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_order; },
    },
    FormatComponent{
        .name = "lastElement",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_lastElement; },
    },
    FormatComponent{
        .name = "extraParam",
        .get_double = [](uint, uint, RAYX::Ray ray) { return ray.m_extraParam; },
    },
};

// These includes allow the user to just import Writer.h and still access
// both CSVWriter and H5Writer.
#include "CSVWriter.h"
#include "H5Writer.h"
