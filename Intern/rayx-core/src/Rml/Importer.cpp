#include "Importer.h"

#include <cstring>
#include <memory>
#include <rapidxml.hpp>
#include <sstream>
#include <string>

#include "Beamline/Beamline.h"
#include "Beamline/Objects/Objects.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Design/DesignElement.h"
#include "DesignElementWriter.h"
#include "DesignSourceWriter.h"
#include "Element/Element.h"
#include "Rml/xml.h"

using RAYX::ElementType;

void parseElement(RAYX::xml::Parser parser, RAYX::DesignElement* de) {
    switch (parser.type()) {
        case ElementType::ImagePlane:
            getImageplane(parser, de);
            break;
        case ElementType::ConeMirror:
            getCone(parser, de);
            break;
        case ElementType::Crystal:
            getCrystal(parser, de);
            break;
        case ElementType::CylinderMirror:
            getCylinder(parser, de);
            break;
        case ElementType::EllipsoidMirror:
            getEllipsoid(parser, de);
            break;
        case ElementType::ExpertsMirror:
            getExpertsOptics(parser, de);
            break;
        case ElementType::ParaboloidMirror:
            getParaboloid(parser, de);
            break;
        case ElementType::PlaneGrating:
            getPlaneGrating(parser, de);
            break;
        case ElementType::PlaneMirror:
            getPlaneMirror(parser, de);
            break;
        case ElementType::ReflectionZoneplate:
            getRZP(parser, de);
            break;
        case ElementType::Slit:
            getSlit(parser, de);
            break;
        case ElementType::SphereGrating:
            getSphereGrating(parser, de);
            break;
        case ElementType::Sphere:
        case ElementType::SphereMirror:
            getSphereMirror(parser, de);
            break;
        case ElementType::ToroidMirror:
            getToroidMirror(parser, de);
            break;
        case ElementType::ToroidGrating:
            getToroidalGrating(parser, de);
            break;
        default:
            RAYX_LOG << "Could not classify beamline object with Name: " << parser.name()
                     << "; Type: " << static_cast<int>(parser.type());  // TODO: write type as string not enum id
            break;
    }
}

namespace RAYX {

void addBeamlineObjectFromXML(rapidxml::xml_node<>* node, Group& group, std::filesystem::path filename) {
    RAYX::xml::Parser parser(node, filename);
    ElementType type = parser.type();

    std::unique_ptr<DesignSource> ds = std::make_unique<DesignSource>();
    ds->m_elementParameters = Map();

    std::unique_ptr<DesignElement> de = std::make_unique<DesignElement>();
    de->m_elementParameters = Map();

    bool isSource = true;
    switch (type) {
        case ElementType::PointSource:
            setPointSource(parser, ds.get());
            break;
        case ElementType::MatrixSource:
            setMatrixSource(parser, ds.get());
            break;
        case ElementType::DipoleSource:
        case ElementType::DipoleSrc:
            setDipoleSource(parser, ds.get());
            break;
        case ElementType::PixelSource:
            setPixelSource(parser, ds.get());
            break;
        case ElementType::CircleSource:
            setCircleSource(parser, ds.get());
            break;
        case ElementType::SimpleUndulatorSource:
            setSimpleUndulatorSource(parser, ds.get());
            break;
        default:
            isSource = false;
            break;
    }

    // TODO: could likely be made nicer
    if (isSource) {
        group.addChild(std::move(ds));
    } else {
        parseElement(parser, de.get());
        group.addChild(std::move(de));
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
            auto groupOpt = xml::parseGroup(object);
            if (!groupOpt) {
                RAYX_EXIT << "parseGroup failed!";
            }
            std::unique_ptr<Group> nestedGroup = std::make_unique<Group>(std::move(*groupOpt));

            // Recursively parse all objects from within the group.
            handleObjectCollection(object, *nestedGroup, filename);
            group.addChild(std::move(nestedGroup));
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

    return root;
}

}  // namespace RAYX
