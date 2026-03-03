#include "BeamlineBuilder.h"

#include <cstring>
#include <memory>
#include <rapidxml.hpp>
#include <sstream>
#include <string>

#include "Design/Beamline.h"
#include "Design/Element.h"
#include "xml.h"
#include "BeamlineObjectBuilder.h"

using rayx::ElementType;

enum class SurfaceElementType {
    Crystal,
    CylinderMirror,
    ImagePlane,
    ParaboloidMirror,
    PlaneGrating,
    ReflectionZoneplate,
    Slit,
    EllipsoidMirror,
    ConeMirror,
    ExpertsMirror,
    PlaneMirror,
    CylinderMirror,
    ImagePlane,
    Paraboloid,
    RzpSphere,
    SphereMirror,
    SphereGrating,
    ToroidMirror,
    ToroidGrating,
    Foil,
    Aperature,
};



void parseElement(rayx::xml::Parser parser, ElementNode& element) {

    SurfaceElement se;

    SurfaceElementType type = StringToSurfaceElementType(parser.name());

    switch (type) {
        case ImagePlane:
            makeImagePlane(parser, se);
            break;
        case Slit:
            makeSlit(parser, se);
            break;
        case CylinderMirror:
            makeCylinderMirror(parser, se);
            break;
        case ParaboloidMirror:
            makeParaboloidMirror(parser, se);
            break;
        case PlaneGrating:
            makePlaneGrating(parser, se);
            break;
        case ReflectionZoneplate:
            makeReflectionZoneplate(parser, se);
            break;
        case SphereMirror:
            makeSphereMirror(parser, se);
            break;
        case ConeMirror:
            makeConeMirror(parser, se);
            break;
        case ExpertsMirror:
            makeExpertsMirror(parser, se);
            break;
        case PlaneMirror:
            makePlaneMirror(parser, se);
            break;
        case ToroidMirror:
            makeToroidMirror(parser, se);
            break;
        case ToroidGrating:
            makeToroidGrating(parser, se);
            break;
        case EllipsoidMirror:
            makeEllipsoidMirror(parser, se);
            break;
        case Crystal:
            makeCrystal(parser, se);
            break;
        case Foil:
            makeFoil(parser, se);
            break;
        case Aperture:
            makeAperture(parser, se);
            break;
        default:
            RAYX_EXIT << "unknown element type \"" << parser.name() << "\"";
    }

    Area area = makeArea(parser);

    Behavior behavior = makeBehavior(parser);
    
    se.area(area);
    se.behavior(behavior);

    makeOptionalCurvature(parser, se);
    makeOptionalAperture(parser, se);
    makeOptionalSlopeError(parser, se);

}

namespace rayx {

void addBeamlineObjectFromXML(rapidxml::xml_node<>* XmlNode, Node& BlNode, std::filesystem::path filepath) {
    rayx::xml::Parser parser(XmlNode, filepath);
    Object type = parser.type();

    bool isSource = true;
    switch (type) {
        case PointSource:
            SourceNode source;
            setPointSource(parser, source);
            break;
        case MatrixSource:
            SourceNode source;
            setMatrixSource(parser, source);
            break;
        case DipoleSource:
            SourceNode source;
            setDipoleSource(parser, source);
            break;
        case PixelSource:
            SourceNode source;
            setPixelSource(parser, source);
            break;
        case CircleSource:
            SourceNode source;
            setCircleSource(parser, source);
            break;
        case SimpleUndulatorSource:
            SourceNode source;
            setSimpleUndulatorSource(parser, source);
            break;
        case SurfaceElement:
            ElementNode element;
            parseElement(parser, element);
            isSource = false;
            break;
        default:
             RAYX_LOG << "Could not classify beamline object with Name: " << parser.name()
                     << "; Type: " << static_cast<int>(parser.type());  // TODO: write type as string not enum id
            break;
    }

    // TODO: could likely be made nicer
    if (isSource) {
        BlNode.append(std::move(ds));
    } else {
        BlNode.append(std::move(de));
    }
}

// `collection` is an xml object, over whose children-objects we want to
// iterate in order to add them to the beamline.
// `collection` may either be a <beamline> or a <group>.
// the beamline-context represents the stack of beamlines within which we currently are.
// Whenever we look into a beamline, this beamline has to be pushed onto the beamline context stack. And when we are done, it will be popped again.
void handleObjectCollection(rapidxml::xml_node<>* collection, Node& parent, const std::filesystem::path& filepath) {
    // Iterating through XML objects

    for (rapidxml::xml_node<>* object = collection->first_node(); object; object = object->next_sibling()) {
        if (strcmp(object->name(), "object") == 0) {
            makeRotationAndTranslation(collection, parent);

            addBeamlineObjectFromXML(object, parent, filepath);
        } else if (strcmp(object->name(), "group") == 0) {   
            makeRotationAndTranslation(collection, parent);

            std::unique_ptr<Node> nestedNode = std::make_unique<Node>(std::move(*groupOpt));

            // Recursively parse all objects from within the beamline.
            handleObjectCollection(object, *nestedNode, filepath);
            parent.append(nestedNode);
        } else if (strcmp(object->name(), "param") != 0) {
            RAYX_EXIT << "received weird object->name(): " << object->name();
        }
    }
}

// This is the central function to load RML files.
Node importBeamline(const std::filesystem::path& filepath) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    // first implementation: stringstreams are slow; this might need
    // optimization
    RAYX_VERB << "import beamline file: " << filepath << "";

    // load the RML file to a string.
    std::ifstream t(filepath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string test = buffer.str();

    if (test.empty()) { RAYX_EXIT << "error: could not open beamline file! (or it was just empty)"; }

    // load the RML string into the rapid XML parser library.
    std::vector<char> cstr(test.c_str(), test.c_str() + test.size() + 1);
    rapidxml::xml_document<> doc;
    doc.parse<0>(cstr.data());

    RAYX_VERB << "RML Version: " << doc.first_node("lab")->first_node("version")->value();
    rapidxml::xml_node<>* xml_beamline = doc.first_node("lab")->first_node("beamline");
    
    auto name = xml_beamline->first_node("name")->value();
    
    Node root(name);

    // go through all objects of the file, and parse them.
    // The beamline context stores the set of beamline in which the algorithm currently "is".
    // For each beamline we call handleObjectCollection recursively, and push the beamline onto the beamline context stack.
    handleObjectCollection(xml_beamline, root, filepath);
    return root;

}

}  // namespace rayx