#include <fstream>

#include "CanonicalizePath.h"
#include "IO/Debug.h"

namespace rayx {

std::optional<std::vector<uint8_t>> readFile(const std::string& filename, const uint32_t count) {
    std::vector<uint8_t> data;

    std::ifstream file;

    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) { return {}; }

    uint64_t read_count = count;
    if (count == 0) {
        file.seekg(0, std::ios::end);
        read_count = static_cast<uint64_t>(file.tellg());
        file.seekg(0, std::ios::beg);
    }

    data.resize(static_cast<size_t>(read_count));
    file.read(reinterpret_cast<char*>(data.data()), read_count);
    file.close();

    return data;
}

// Read file into array of bytes, and cast to uint32_t*, then return.
// The data has been padded, so that it fits into an array uint32_t.
std::optional<std::vector<uint32_t>> readFileAlign32(const std::string& filename, const uint32_t count) {
    std::vector<uint32_t> data;

    std::ifstream file;

    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) { return {}; }

    uint64_t read_count = count;
    if (count == 0) {
        file.seekg(0, std::ios::end);
        read_count = static_cast<uint64_t>(file.tellg());
        file.seekg(0, std::ios::beg);
    }

    // Check, if it's even uint32_t alignable!
    if (read_count % sizeof(uint32_t) != 0) { return {}; }

    data.resize(static_cast<size_t>(read_count) / sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(data.data()), read_count);
    file.close();

    return data;
}

void writeFile(const std::vector<uint8_t>& data, const std::string& filename, const uint32_t count) {
    std::ofstream file;

    file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open()) { RAYX_EXIT << "Failed to open file: " << filename; }

    uint64_t write_count = count;
    if (count == 0) { write_count = data.size(); }

    file.write(reinterpret_cast<const char*>(data.data()), write_count);
    file.close();
}


void parseElement(rayx::xml::Parser parser, rayx::DesignElement* de) {
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
        case ElementType::Foil:
            getFoil(parser, de);
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


void addBeamlineObjectFromXML(rapidxml::xml_node<>* node, Group& group, std::filesystem::path filepath) {
    rayx::xml::Parser parser(node, filepath);
    ElementType type = parser.type();

    std::unique_ptr<DesignSource> ds = std::make_unique<DesignSource>();
    ds->m_elementParameters          = Map();

    std::unique_ptr<DesignElement> de = std::make_unique<DesignElement>();
    de->m_elementParameters           = Map();

    bool isSource = true;
    switch (type) {
        case ElementType::PointSource:
            setPointSource(parser, ds.get());
            break;
        case ElementType::MatrixSource:
            setMatrixSource(parser, ds.get());
            break;
        case ElementType::DipoleSource:
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
void handleObjectCollection(rapidxml::xml_node<>* collection, Group& group, const std::filesystem::path& filepath) {
    // Iterating through XML objects
    for (rapidxml::xml_node<>* object = collection->first_node(); object; object = object->next_sibling()) {
        if (strcmp(object->name(), "object") == 0) {
            addBeamlineObjectFromXML(object, group, filepath);
        } else if (strcmp(object->name(), "group") == 0) {
            auto groupOpt = xml::parseGroup(object);
            if (!groupOpt) { RAYX_EXIT << "parseGroup failed!"; }
            std::unique_ptr<Group> nestedGroup = std::make_unique<Group>(std::move(*groupOpt));

            // Recursively parse all objects from within the group.
            handleObjectCollection(object, *nestedGroup, filepath);
            group.addChild(std::move(nestedGroup));
        } else if (strcmp(object->name(), "param") != 0) {
            RAYX_EXIT << "received weird object->name(): " << object->name();
        }
    }
}

}  // namespace rayx
