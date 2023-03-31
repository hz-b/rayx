#include "Importer.h"

#include <cstring>
#include <memory>
#include <rapidxml.hpp>
#include <sstream>
#include <utility>

#include "Beamline/Beamline.h"
#include "Beamline/Objects/Objects.h"
#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"

namespace RAYX {

void addBeamlineObjectFromXML(rapidxml::xml_node<>* node, Beamline* beamline, const std::vector<xml::Group>& group_context,
                              std::filesystem::path filename) {
    const char* type = node->first_attribute("type")->value();

    // the following three blocks of code are lambda expressions (see
    // https://en.cppreference.com/w/cpp/language/lambda) They define functions
    // to be used in the if-else-chain below to keep it structured and readable.

    // addLightSource(s, node) is a function adding a light source to the
    // beamline (if it's not nullptr)
    const auto addLightSource = [&](const std::shared_ptr<LightSource>& s, rapidxml::xml_node<>* node) {
        if (s) {
            beamline->m_LightSources.push_back(s);
        } else {
            RAYX_ERR << "could not construct LightSource with Name: " << node->first_attribute("name")->value()
                     << "; Type: " << node->first_attribute("type")->value();
        }
    };

    // addOpticalElement(e, node) is a function adding an optical element to the
    // beamline (if it's not nullptr)
    const auto addOpticalElement = [&](const std::shared_ptr<OpticalElement>& e, rapidxml::xml_node<>* node) {
        if (e) {
            beamline->m_OpticalElements.push_back(e);
        } else {
            RAYX_ERR << "could not construct OpticalElement with Name: " << node->first_attribute("name")->value()
                     << "; Type: " << node->first_attribute("type")->value();
        }
    };

    RAYX::xml::Parser parser(node, group_context, std::move(filename));

    // every beamline object has a function createFromXML which constructs the
    // object from a given xml-node if possible (otherwise it will return a
    // nullptr) The createFromXML functions use the param* functions declared in
    // <Data/xml.h>
    if (strcmp(type, "Point Source") == 0) {
        addLightSource(std::make_shared<PointSource>(parser), node);
    } else if (strcmp(type, "Matrix Source") == 0) {
        addLightSource(std::make_shared<MatrixSource>(parser), node);
    } else if (strcmp(type, "ImagePlane") == 0) {
        addOpticalElement(std::make_shared<ImagePlane>(parser), node);
    } else if (strcmp(type, "Plane Mirror") == 0) {
        addOpticalElement(std::make_shared<PlaneMirror>(parser), node);
    } else if (strcmp(type, "Toroid") == 0) {
        addOpticalElement(std::make_shared<ToroidMirror>(parser), node);
    } else if (strcmp(type, "Slit") == 0) {
        addOpticalElement(std::make_shared<Slit>(parser), node);
    } else if (strcmp(type, "Spherical Grating") == 0) {
        addOpticalElement(std::make_shared<SphereGrating>(parser), node);
    } else if (strcmp(type, "Plane Grating") == 0) {
        addOpticalElement(std::make_shared<PlaneGrating>(parser), node);
    } else if (strcmp(type, "Sphere") == 0) {
        addOpticalElement(std::make_shared<SphereMirror>(parser), node);
    } else if (strcmp(type, "Reflection Zoneplate") == 0) {
        addOpticalElement(std::make_shared<ReflectionZonePlate>(parser), node);
    } else if (strcmp(type, "Ellipsoid") == 0) {
        addOpticalElement(std::make_shared<Ellipsoid>(parser), node);
    } else if (strcmp(type, "Cylinder") == 0) {
        addOpticalElement(std::make_shared<Cylinder>(parser), node);
    } else if (strcmp(type, "Cone") == 0) {
        addOpticalElement(std::make_shared<Cone>(parser), node);
    } else {
        RAYX_WARN << "could not classify beamline object with Name: " << node->first_attribute("name")->value()
                  << "; Type: " << node->first_attribute("type")->value();
    }
}

/** `collection` is an xml object, over whose children-objects we want to
 * iterate in order to add them to the beamline.
 * `collection` may either be a <beamline> or a <group>. */
void handleObjectCollection(rapidxml::xml_node<>* collection, Beamline* beamline, std::vector<xml::Group>* group_context,
                            const std::filesystem::path& filename) {
    for (rapidxml::xml_node<>* object = collection->first_node(); object; object = object->next_sibling()) {  // Iterating through objects
        if (strcmp(object->name(), "object") == 0) {
            addBeamlineObjectFromXML(object, beamline, *group_context, filename);
        } else if (strcmp(object->name(), "group") == 0) {
            xml::Group g{};
            bool success = xml::parseGroup(object, &g);
            if (success) {
                group_context->push_back(g);
            } else {
                RAYX_ERR << "parseGroup failed!";
            }
            handleObjectCollection(object, beamline, group_context, filename);
            if (success) {
                group_context->pop_back();
            }
        } else if (strcmp(object->name(), "param") != 0) {
            RAYX_ERR << "received weird object->name(): " << object->name();
        }
    }
}

Beamline importBeamline(const std::filesystem::path& filename) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    // first implementation: stringstreams are slow; this might need
    // optimization
    RAYX_VERB << "importBeamline is called with file \"" << filename << "\"";

    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string test = buffer.str();
    if (test.empty()) {
        RAYX_ERR << "importBeamline could not open file! (or it was just empty)";
    }
    std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr.data());

    RAYX_VERB << "\t Version: " << doc.first_node("lab")->first_node("version")->value();
    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");

    Beamline beamline;
    std::vector<xml::Group> group_context;
    handleObjectCollection(xml_beamline, &beamline, &group_context, filename);
    return beamline;
}

}  // namespace RAYX
