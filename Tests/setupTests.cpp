#include "setupTests.h"

std::unique_ptr<RAYX::Tracer> tracer;

// helper functions

double parseDouble(std::string s) {
    double d;
    if (sscanf(s.c_str(), "%le", &d) != 1) {
        RAYX_WARN << "parseDouble failed for string:";
        RAYX_ERR << s;
        return false;
    }
    return d;
}

RAYX::Ray parseCSVline(std::string line) {
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

    RAYX::Ray ray;
    // order of doubles:
    // RN, RS, RO, position=(OX, OY, OZ), direction=(DX, DY, DZ), energy,
    // path length, stokes=(S0, S1, S2, S3)

    ray.m_position = {vec[3], vec[4], vec[5]};
    ray.m_direction = {vec[6], vec[7], vec[8]};
    ray.m_energy = vec[9];
    ray.m_pathLength = vec[10];
    ray.m_stokes = {vec[11], vec[12], vec[13], vec[14]};

    // otherwise uninitialized:
    ray.m_extraParam = -1;
    ray.m_weight = -1;
    ray.m_lastElement = -1;
    ray.m_order = -1;

    return ray;
}

// will look at Tests/input/<filename>.rml
RAYX::Beamline loadBeamline(std::string filename) {
    std::string beamline_file = resolvePath("Tests/input/" + filename + ".rml");

    return RAYX::importBeamline(beamline_file);
}

// will write to Tests/output/<filename>.csv
void writeToOutputCSV(RAYX::RayList& rays, std::string filename) {
    std::string f = resolvePath("Tests/output/" + filename + ".csv");
    writeCSV(rays, f);
}

int intexp(int a, int b) {
    if (b == 0) return 1;
    return a * intexp(a, b - 1);
}

// sequentialExtraParam yields the desired extraParam for rays which went the
// sequential route through a beamline with `count` OpticalElements.
// sequentialExtraParam(2) = 21
// sequentialExtraParam(3) = 321 // i.e. first element 1, then 2 and then 3.
// ...
int sequentialExtraParam(int count) {
    if (count == 1) {
        return 1;
    }
    return sequentialExtraParam(count - 1) + count * intexp(10, count - 1);
}

RAYX::RayList traceRML(std::string filename, Filter filter) {
    auto beamline = loadBeamline(filename);
    auto rays = tracer->trace(beamline);
    rays = rays.filter([](Ray& r) { return r.m_weight != 0; });
    if (filter == Filter::OnlySequentialRays) {
        auto extra = sequentialExtraParam(beamline.m_OpticalElements.size());
        rays = rays.filter([=](Ray& r) { return r.m_extraParam == extra; });
    }

    return rays;
}

// will look at Tests/input/<filename>.csv
// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
// element coordinates of the relevant element!)
RAYX::RayList loadCSVRayUI(std::string filename) {
    std::string file = resolvePath("Tests/input/" + filename + ".csv");

    std::ifstream f(file);
    std::string line;

    // discard first two lines
    for (int i = 0; i < 2; i++) {
        std::getline(f, line);
    }

    RAYX::RayList out;

    while (std::getline(f, line)) {
        out.push(parseCSVline(line));
    }

    return out;
}

void compareRayLists(const RAYX::RayList& rayx_list,
                     const RAYX::RayList& rayui_list, double t) {
    CHECK_EQ(rayx_list.rayAmount(), rayui_list.rayAmount());

    auto itRayX = rayx_list.cbegin();
    auto itRayXEnd = rayx_list.cend();

    auto itRayUI = rayui_list.cbegin();

    while (itRayX != itRayXEnd) {
        auto rayx = *itRayX;
        auto rayui = *itRayUI;
        CHECK_EQ(rayx.m_position, rayui.m_position, t);
        CHECK_EQ(rayx.m_direction, rayui.m_direction, t);
        CHECK_EQ(rayx.m_energy, rayui.m_energy, t);

        ++itRayX;
        ++itRayUI;
    }
}

void compareAgainstRayUI(std::string filename, double tolerance) {
    auto beamline = loadBeamline(filename);
    auto extra = sequentialExtraParam(beamline.m_OpticalElements.size());

    auto a = traceRML(filename).filter(
        [=](Ray& r) { return r.m_extraParam == extra; });
    auto b = loadCSVRayUI(filename);

    writeToOutputCSV(a, filename + ".rayx");
    writeToOutputCSV(b, filename + ".rayui");

    compareRayLists(a, b, tolerance);
}