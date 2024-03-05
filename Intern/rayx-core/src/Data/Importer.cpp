#include "Importer.h"

#include <cstring>
#include <memory>
#include <rapidxml.hpp>
#include <sstream>

#include "Beamline/Beamline.h"
#include "Beamline/Objects/Objects.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "DesignElementWriter.h"
#include "DesignSourceWriter.h"

void parseElement(xml::Parser parser, DesignElement* de) {
    const char* type = parser.type();
    // TODO add functions for each Element

    if (strcmp(type, "ImagePlane") == 0) {
        getImageplane(parser, de);
    } else if (strcmp(type, "Cone") == 0) {
        getCone(parser, de);
    } else if (strcmp(type, "Cylinder") == 0) {
        getCylinder(parser, de);
    } else if (strcmp(type, "Ellipsoid") == 0) {
        getEllipsoid(parser, de);
    } else if (strcmp(type, "Experts Optics") == 0) {
        getExpertsOptics(parser, de);
    } else if (strcmp(type, "Experts Cubic") == 0) {
        getExpertsCubic(parser, de);
    } else if (strcmp(type, "Paraboloid") == 0) {
        getParaboloid(parser, de);
    } else if (strcmp(type, "Plane Grating") == 0) {
        RAYX_LOG << "import Planegrating";
        getPlaneGrating(parser, de);
    } else if (strcmp(type, "Plane Mirror") == 0) {
        getPlaneMirror(parser, de);
    } else if (strcmp(type, "Reflection Zoneplate") == 0) {
        getRZP(parser, de);
    } else if (strcmp(type, "Slit") == 0) {
        getSlit(parser, de);
    } else if (strcmp(type, "Spherical Grating") == 0) {
        getSphereGrating(parser, de);
    } else if (strcmp(type, "Sphere") == 0) {
        getSphereMirror(parser, de);
    } else if (strcmp(type, "Spherical Mirror") == 0) {
        getSphereMirror(parser, de);
    } else if (strcmp(type, "Toroid") == 0) {
        getToroidMirror(parser, de);
    } else {
        RAYX_WARN << "could not classify beamline object with Name: " << parser.name() << "; Type: " << parser.type();
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
    const char* type = parser.type();
    DesignSource ds;
    ds.v = Map();

    DesignElement de;
    de.v = Map();

    // Light sources have constructors that accept a const DesignObject& as argument.
    // They use the param* functions declared in <Data/xml.h> to retrieve the relevant information.
    if (strcmp(type, "Point Source") == 0) {
        setPointSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
        RAYX_LOG << "in importer PS";
    } else if (strcmp(type, "Matrix Source") == 0) {
        setMatrixSource(parser, &ds);
        beamline->m_DesignSources.push_back(ds);
        RAYX_LOG << "in importer MS";
    } else if (strcmp(type, "Dipole") == 0) {
        //addLightSource(std::make_shared<DipoleSource>(parser), node);
        RAYX_LOG << "import none";
    } else if (strcmp(type, "Dipole Source") == 0) {
        RAYX_LOG << "import none";
        //addLightSource(std::make_shared<DipoleSource>(parser), node);
    } else if (strcmp(type, "Pixel Source") == 0) {
        RAYX_LOG << "import none";
        //addLightSource(std::make_shared<PixelSource>(parser), node);
    } else if (strcmp(type, "Circle Source") == 0) {
        RAYX_LOG << "import none";
        //addLightSource(std::make_shared<CircleSource>(parser), node);
    } else if (strcmp(type, "Simple Undulator") == 0) {
        RAYX_LOG << "import none";
        //addLightSource(std::make_shared<SimpleUndulatorSource>(parser), node);
    } else {
        parseElement(parser, &de);
        RAYX_LOG << "import " << de.getName();
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
