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
#include "DesignElementWriter.h"
#include "DesignSourceWriter.h"
#include <DesignElement/DesignElement.h>
#include <Shader/Strings.h>

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
    } else {
        RAYX_LOG << "could not classify beamline object with Name: " << parser.name()
                 << "; Type: " << int(parser.type());  // TODO: write type as string not enum id
    }
}

namespace RAYX {

void addBeamlineObjectFromXML(rapidxml::xml_node<>* node, Beamline* beamline, const std::vector<xml::Group>& group_context,
                              std::filesystem::path filename) {
    // the following three blocks of code are lambda expressions (see
    // https://en.cppreference.com/w/cpp/language/lambda) They define functions
    // to be used in the if-else-chain below to keep it structured and readable.

    // //addLightSource(s, node) is a function adding a light source to the
    // beamline (if it's not nullptr)

    RAYX::xml::Parser parser(node, group_context, filename);
    ElementType type = parser.type();
    DesignSource ds;
    ds.m_elementParameters = Map();

    DesignElement de;
    de.m_elementParameters = Map();

    // Light sources have constructors that accept a const DesignObject& as argument.
    // They use the param* functions declared in <Data/xml.h> to retrieve the relevant information.
    if (type == ElementType::PointSource) {
        setPointSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
    } else if (type == ElementType::MatrixSource) {
        setMatrixSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
    } else if (type == ElementType::DipoleSource) {
        setDipoleSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
    } else if (type == ElementType::DipoleSrc) {
        setDipoleSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
    } else if (type == ElementType::PixelSource) {
        setPixelSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
    } else if (type == ElementType::CircleSource) {
        setCircleSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
    } else if (type == ElementType::SimpleUndulatorSource) {
        setSimpleUndulatorSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
    } else {
        parseElement(parser, &de);
        beamline->m_DesignElements.push_back(de);
    }
}

// `collection` is an xml object, over whose children-objects we want to
// iterate in order to add them to the beamline.
// `collection` may either be a <beamline> or a <group>.
// the group-context represents the stack of groups within which we currently are.
// Whenever we look into a group, this group has to be pushed onto the group context stack. And when we are done, it will be popped again.
void handleObjectCollection(rapidxml::xml_node<>* collection, Beamline* beamline, std::vector<xml::Group>* group_context,
                            const std::filesystem::path& filename) {
    // Iterating through XML objects
    for (rapidxml::xml_node<>* object = collection->first_node(); object; object = object->next_sibling()) {
        if (strcmp(object->name(), "object") == 0) {
            // if it's an object, parse it.
            addBeamlineObjectFromXML(object, beamline, *group_context, filename);
        } else if (strcmp(object->name(), "group") == 0) {
            // if it's a group, add it: parse it and add it to the group context.
            xml::Group g{};
            bool success = xml::parseGroup(object, &g);
            if (success) {
                group_context->push_back(g);
            } else {
                RAYX_ERR << "parseGroup failed!";
            }
            // recursively parse all objects from within the group.
            handleObjectCollection(object, beamline, group_context, filename);

            // "pop" the group stack.
            if (success) {
                group_context->pop_back();
            }
        } else if (strcmp(object->name(), "param") != 0) {
            RAYX_ERR << "received weird object->name(): " << object->name();
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
        RAYX_ERR << "importBeamline could not open file! (or it was just empty)";
    }

    // load the RML string into the rapid XML parser library.
    std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr.data());

    RAYX_VERB << "\t Version: " << doc.first_node("lab")->first_node("version")->value();
    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");

    Beamline beamline;
    std::vector<xml::Group> group_context;

    // go through all objects of the file, and parse them.
    // The group context stores the set of group in which the algorithm currently "is".
    // For each group we call handleObjectCollection recursively, and push the group onto the group context stack.
    handleObjectCollection(xml_beamline, &beamline, &group_context, filename);
    return beamline;
}

}  // namespace RAYX
