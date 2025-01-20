#include "Importer.h"

#include <cstring>
#include <memory>
#include <rapidxml.hpp>
#include <sstream>
#include <string>

#include "Beamline/Beamline.h"
#include "Beamline/Objects/Objects.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Design/DesignElement.h"
#include "DesignElementWriter.h"
#include "DesignSourceWriter.h"
#include "Strings.h"

using RAYX::ElementType;

void parseElement(RAYX::xml::Parser parser, RAYX::DesignElement* de) {
    ElementType type = parser.type();

    if (type == ElementType::ImagePlane) {
        getImageplane(parser, de);
    } else if (type == ElementType::ConeMirror) {
        getCone(parser, de);
    } else if (type == ElementType::CylinderMirror) {
        getCylinder(parser, de);
    } else if (type == ElementType::EllipsoidMirror) {
        getEllipsoid(parser, de);
    } else if (type == ElementType::ExpertsMirror) {
        getExpertsOptics(parser, de);
    } else if (type == ElementType::ParaboloidMirror) {
        getParaboloid(parser, de);
    } else if (type == ElementType::PlaneGrating) {
        getPlaneGrating(parser, de);
    } else if (type == ElementType::PlaneMirror) {
        getPlaneMirror(parser, de);
    } else if (type == ElementType::ReflectionZoneplate) {
        getRZP(parser, de);
    } else if (type == ElementType::Slit) {
        getSlit(parser, de);
    } else if (type == ElementType::SphereGrating) {
        getSphereGrating(parser, de);
    } else if (type == ElementType::Sphere) {
        getSphereMirror(parser, de);
    } else if (type == ElementType::SphereMirror) {
        getSphereMirror(parser, de);
    } else if (type == ElementType::ToroidMirror) {
        getToroidMirror(parser, de);
    } else if (type == ElementType::ToroidGrating) {
        getToroidalGrating(parser, de);
    } else {
        RAYX_LOG << "could not classify beamline object with Name: " << parser.name()
                 << "; Type: " << int(parser.type());  // TODO: write type as string not enum id
    }
}

namespace RAYX {

void addBeamlineObjectFromXML(rapidxml::xml_node<>* node, Group& group, std::filesystem::path filename) {
    RAYX::xml::Parser parser(node, filename);
    ElementType type = parser.type();

    DesignSource ds;
    ds.m_elementParameters = Map();

    DesignElement de;
    de.m_elementParameters = Map();

    bool isSource = true;
    switch (type) {
        case ElementType::PointSource:
            setPointSource(parser, &ds);
            break;
        case ElementType::MatrixSource:
            setMatrixSource(parser, &ds);
            break;
        case ElementType::DipoleSource:
        case ElementType::DipoleSrc:
            setDipoleSource(parser, &ds);
            break;
        case ElementType::PixelSource:
            setPixelSource(parser, &ds);
            break;
        case ElementType::CircleSource:
            setCircleSource(parser, &ds);
            break;
        case ElementType::SimpleUndulatorSource:
            setSimpleUndulatorSource(parser, &ds);
            break;
        default:
            isSource = false;
            break;
    }

    // TODO: could likely be made nicer
    if (isSource) {
        group.addChild(ds);
    } else {
        parseElement(parser, &de);
        group.addChild(de);
    }
}

// `collection` is an xml object, over whose children-objects we want to
// iterate in order to add them to the beamline.
// `collection` may either be a <beamline> or a <group>.
// the group-context represents the stack of groups within which we currently are.
// Whenever we look into a group, this group has to be pushed onto the group context stack. And when we are done, it will be popped again.
void handleObjectCollection(rapidxml::xml_node<>* collection, Group& group, const std::filesystem::path& filename) {
    // Iterating through XML objects
    for (rapidxml::xml_node<>* object = collection->first_node(); object; object = object->next_sibling()) {
        if (strcmp(object->name(), "object") == 0) {
            addBeamlineObjectFromXML(object, group, filename);
        } else if (strcmp(object->name(), "group") == 0) {
            Group group;
            auto groupOpt = xml::parseGroup(object);
            if (!groupOpt) {
                RAYX_EXIT << "parseGroup failed!";
            }
            auto g = groupOpt.value();

            // Recursively parse all objects from within the group.
            handleObjectCollection(object, group, filename);

        } else if (strcmp(object->name(), "param") != 0) {
            RAYX_EXIT << "received weird object->name(): " << object->name();
        }
    }
}

// This is the central function to load RML files.
Beamline importBeamline(const std::filesystem::path& filename) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    // first implementation: stringstreams are slow; this might need
    // optimization
    RAYX_VERB << "importBeamline is called with file \"" << filename << "\"";

    // load the RML file to a string.
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string test = buffer.str();

    if (test.empty()) {
        RAYX_EXIT << "importBeamline could not open file! (or it was just empty)";
    }

    // load the RML string into the rapid XML parser library.
    std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr.data());

    RAYX_VERB << "\t Version: " << doc.first_node("lab")->first_node("version")->value();
    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");

    Group root;

    // go through all objects of the file, and parse them.
    // The group context stores the set of group in which the algorithm currently "is".
    // For each group we call handleObjectCollection recursively, and push the group onto the group context stack.
    handleObjectCollection(xml_beamline, root, filename);

    Beamline beamline;
    // TODO
    return beamline;
}

}  // namespace RAYX
