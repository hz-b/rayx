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

    const auto addOpticalElement = [&](Element e, rapidxml::xml_node<>* node) {
        OpticalElement e2 = {
            .m_element = e,
            .m_name = node->first_attribute("name")->value(),
        };

        beamline->m_OpticalElements.push_back(e2);
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
    } else if (strcmp(type, "Dipole") == 0) {
        addLightSource(std::make_shared<DipoleSource>(parser), node);
    } else if (strcmp(type, "Dipole Source") == 0) {
        addLightSource(std::make_shared<DipoleSource>(parser), node);
    } else if (strcmp(type, "ImagePlane") == 0) {
        addOpticalElement(makeImagePlane(parser), node);
    } else if (strcmp(type, "Plane Mirror") == 0) {
        addOpticalElement(makePlaneMirror(parser), node);
    } else if (strcmp(type, "Toroid") == 0) {
        addOpticalElement(makeToroidMirror(parser), node);
    } else if (strcmp(type, "Slit") == 0) {
        addOpticalElement(makeSlit(parser), node);
    } else if (strcmp(type, "Spherical Grating") == 0) {
        addOpticalElement(makeSphereGrating(parser), node);
    } else if (strcmp(type, "Plane Grating") == 0) {
        addOpticalElement(makePlaneGrating(parser), node);
    } else if (strcmp(type, "Sphere") == 0) {
        addOpticalElement(makeSphereMirror(parser), node);
    } else if (strcmp(type, "Reflection Zoneplate") == 0) {
        addOpticalElement(makeReflectionZonePlate(parser), node);
    } else if (strcmp(type, "Ellipsoid") == 0) {
        addOpticalElement(makeEllipsoid(parser), node);
    } else if (strcmp(type, "Cylinder") == 0) {
        addOpticalElement(makeCylinder(parser), node);
    } else if (strcmp(type, "Cone") == 0) {
        addOpticalElement(makeCone(parser), node);
    } else {
        RAYX_WARN << "could not classify beamline object with Name: " << node->first_attribute("name")->value()
                  << "; Type: " << node->first_attribute("type")->value();
    }
}

RenderObjectVec getRenderData(const std::filesystem::path& filename) {
    RenderObjectVec data;

    // TODO(Jannis): This (creating the doc) is duplicated code from importBeamline. If this stays importer should be refactored. Keep in
    // mind: cstr is state that needs to stay in scope during parsing.
    std::ifstream fileStream(filename);
    if (!fileStream) RAYX_ERR << "importBeamline could not open file!";

    std::stringstream buffer;
    buffer << fileStream.rdbuf();

    std::string fileContents = buffer.str();
    if (fileContents.empty()) RAYX_ERR << "importBeamline file is empty!";

    std::vector<char> fileContentsCstr(fileContents.c_str(), fileContents.c_str() + fileContents.size() + 1);
    rapidxml::xml_document<> doc;
    doc.parse<0>(fileContentsCstr.data());

    RAYX_VERB << "\t Version: " << doc.first_node("lab")->first_node("version")->value();
    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");

    for (rapidxml::xml_node<>* object = xml_beamline->first_node(); object; object = object->next_sibling()) {  // Iterating through objects
        const char* type = object->first_attribute("type")->value();

        // Skip lightsources
        if (strcmp(type, "Point Source") == 0 ||
            strcmp(type, "Matrix Source") == 0) {  // TODO(Jannis): The should be a better check for this. Adding a new lightsource would break this.
            continue;
        } else {
            RAYX::xml::Parser parser(object, std::vector<xml::Group>(), std::move(filename));

            RenderObject d;
            d.name = parser.name();
            d.position = parser.parsePosition();
            d.orientation = parser.parseOrientation();
            d.cutout = parser.parseCutout();

            data.push_back(d);
        }
    }

    return data;
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
