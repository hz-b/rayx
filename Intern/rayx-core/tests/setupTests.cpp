#include "setupTests.h"

// define globally decralred invocation state
ConstState inv;

std::unique_ptr<Tracer> tracer;

// helper functions

double parseDouble(std::string s) {
    double d;
#if defined(WIN32)
    if (sscanf_s(s.c_str(), "%le", &d) != 1) {
#else
    if (sscanf(s.c_str(), "%le", &d) != 1) {
#endif
        RAYX_WARN << "parseDouble failed for string:";
        RAYX_EXIT << s;
        return false;
    }
    return d;
}

void parseRayUiCsvLineAndAppendEvent(Rays& rays, std::string line) {
    std::vector<double> vec;

    if (line.ends_with('\n')) {
        line.pop_back();
    }

    while (true) {
        auto idx = line.find('\t');
        if (idx == std::string::npos) {
            vec.push_back(parseDouble(line));
            break;
        } else {
            vec.push_back(parseDouble(line.substr(0, idx)));
            line = line.substr(idx + 1);
        }
    }

    // order of doubles:
    // RN, RS, RO, position=(OX, OY, OZ), direction=(DX, DY, DZ), energy,
    // path length, stokes=(S0, S1, S2, S3)

    rays.position_x.push_back(vec[3]);
    rays.position_y.push_back(vec[4]);
    rays.position_z.push_back(vec[5]);
    rays.direction_x.push_back(vec[6]);
    rays.direction_y.push_back(vec[7]);
    rays.direction_z.push_back(vec[8]);
    rays.energy.push_back(vec[9]);
    rays.optical_path_length.push_back(vec[10]);
    const auto stokes = glm::dvec4(vec[11], vec[12], vec[13], vec[14]);
    const auto electric_field = stokesToElectricFieldWithBaseConvention(stokes, rays.direction(rays.size() - 1));
    rays.electric_field_x.push_back(electric_field.x);
    rays.electric_field_y.push_back(electric_field.y);
    rays.electric_field_z.push_back(electric_field.z);
}

/// will return the absolute path to the beamline
std::filesystem::path getBeamlineFilepath(std::string filename) {
    return canonicalizeRepositoryPath("Intern/rayx-core/tests/input/" + filename + ".rml");
}

/// will look at Intern/rayx-core/tests/input/<filename>.rml
Beamline loadBeamline(std::string filename) { return importBeamline(getBeamlineFilepath(filename)); }

Rays readCsvUsingFilename(std::string filename) {
    const auto file = canonicalizeRepositoryPath("Intern/rayx-core/tests/input/" + filename + ".csv").string();
    return readCsv(file);
}

/// will write to Intern/rayx-core/tests/output<filename>.csv
void writeCsvUsingFilename(const Rays& rays, std::string filename) {
    const auto f = canonicalizeRepositoryPath("Intern/rayx-core/tests/output/" + filename + ".csv").string();
    writeCsv(f, rays);
}

Rays traceRml(std::string filename, RayAttrMask attrMask, Sequential seq) {
    const auto beamline = loadBeamline(filename);
    return tracer->trace(beamline, seq, ObjectMask::all(), attrMask);
}

std::pair<Beamline, Rays> loadBeamlineAndTrace(std::string filename, RayAttrMask attrMask) {
    auto beamline = loadBeamline(filename);
    auto rays = tracer->trace(beamline, Sequential::No, ObjectMask::all(), attrMask);
    return std::make_pair(std::move(beamline), std::move(rays));
}

/// will look at Intern/rayx-core/tests/input/<filename>.csv
/// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
/// element coordinates of the relevant element!)
Rays loadCsvRayUi(std::string filename) {
    const auto file = canonicalizeRepositoryPath("Intern/rayx-core/tests/input/" + filename + ".csv").string();

    std::ifstream f(file);
    std::string line;

    // discard first two lines
    for (int i = 0; i < 2; i++) {
        std::getline(f, line);
    }

    Rays rays;
    int i = 0;
    while (std::getline(f, line)) {
        // ray-ui only stores the last event of each ray path.
        // we need to fabricate path_id and path_event_id here.
        rays.path_id.push_back(i++);
        rays.path_event_id.push_back(0);

        parseRayUiCsvLineAndAppendEvent(rays, line);
    }
    return rays;
}

void compare(const Rays& a, const Rays& b, const RayAttrMask attrMask, double t) {
    CHECK_EQ(a.size(), b.size());

    if (!contains(a.attrMask(), attrMask)) {
        ADD_FAILURE();
        return;
    }

    if (!contains(b.attrMask(), attrMask)) {
        ADD_FAILURE();
        return;
    }

    const auto n = a.size();
    for (int i = 0; i < n; ++i) {
#define X(type, name, flag) \
    if (contains(attrMask, RayAttrMask::flag)) CHECK_EQ(a.name[i], b.name[i], t);
        RAYX_X_MACRO_RAY_ATTR
#undef X
    }
}

void compareRayUiCompatible(const Rays& a, const Rays& b, double t) { compare(a, b, attrMaskCompatibleWithRayUi, t); }

void compareRayUiCompatible_findAtLeastOnce(const Rays& rayx, const Rays& rayui, double t) {
    const auto numEventsRayui = rayui.size();
    const auto numEventsRayx = rayx.size();
    for (auto i = 0; i < numEventsRayui; ++i) {
        bool found = false;
        for (auto j = 0; j < numEventsRayx; ++j) {
            bool equal = true;

#define X(type, name, flag) \
    if (contains(attrMaskCompatibleWithRayUi, RayAttrMask::flag)) equal = equal && isEqual(rayx.name.at(j), rayui.name.at(i), t);
            RAYX_X_MACRO_RAY_ATTR
#undef X
            if (equal) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << "Could not find ray from rayui (with path_id = " << rayui.path_id.at(i)
                           << " and path_event_id: " << rayui.path_event_id.at(i) << ") in list of rays from rayx.";
    }
}

// returns the rayx rays converted to be ray-UI compatible.
Rays traceRmlAndMakeCompatibleWithRayUi(std::string filename, Sequential seq) {
    const auto beamline = loadBeamline(filename);
    const auto numSources = beamline.numSources();
    const auto numElements = beamline.numElements();
    const auto numObjects = numSources + numElements;

    auto rays =
        tracer
            ->trace(beamline, seq, ObjectMask::all(),
                    attrMaskCompatibleWithRayUi | RayAttrMask::ObjectId | RayAttrMask::PathId | RayAttrMask::PathEventId | RayAttrMask::ElectricField)
            // only events from last element
            .filterByObjectId(numObjects - 1);

    // only last event of each path
    rays = rays.filter([&](int i) { return rays.path_event_id[i] == numObjects - 1; }).sortByPathIdAndPathEventId();

    const auto elements = beamline.compileElements();
    const auto size = rays.size();
    for (int i = 0; i < size; ++i) {
        const auto element_id = rays.object_id[i] - numSources;
        const auto behaviour = elements[element_id].element.m_behaviour;
        if (behaviour.is<Behaviour::ImagePlane>() || behaviour.is<Behaviour::Slit>() || behaviour.is<Behaviour::Foil>()) {
            // these elements have their local z axis along the beam direction, so we need to swap y and z to match Ray-UI
            std::swap(rays.position_y[i], rays.position_z[i]);
            std::swap(rays.direction_y[i], rays.direction_z[i]);
        }
    }

    return rays;
}

void traceRmlAndCompareAgainstRayUi(std::string filename, double tolerance, Sequential seq) {
    auto rayx = traceRmlAndMakeCompatibleWithRayUi(filename, seq);
    auto rayui = loadCsvRayUi(filename);

    writeCsvUsingFilename(rayx, filename + ".rayx");
    writeCsvUsingFilename(rayui, filename + ".rayui");

    compareRayUiCompatible(rayx, rayui, tolerance);
}

void traceRmlAndCompareAgainstRayUi_allowMontecarloRayUi(std::string filename, double tolerance, Sequential seq) {
    auto rayx = traceRmlAndMakeCompatibleWithRayUi(filename, seq);
    auto rayui = loadCsvRayUi(filename);

    writeCsvUsingFilename(rayx, filename + ".rayx");
    writeCsvUsingFilename(rayui, filename + ".rayui");

    compareRayUiCompatible_findAtLeastOnce(rayx, rayui, tolerance);
}

void traceRmlAndCompareAgainstCorrectResults(std::string filename, double tolerance) {
    const auto a = traceRml(filename);
    const auto b = readCsvUsingFilename(filename + ".correct");
    writeCsvUsingFilename(a, filename + ".rayx");
    compare(a, b, RayAttrMask::All, tolerance);
}

MaterialTables createMaterialTables(std::vector<Material> mats_vec) {
    std::array<bool, 92> mats;
    mats.fill(false);
    for (auto m : mats_vec) {
        mats[static_cast<int>(m) - 1] = true;
    }
    return loadMaterialTables(mats);
}
