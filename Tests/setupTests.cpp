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

RAYX::RayList traceRML(std::string filename) {
    auto beamline = loadBeamline(filename);
    auto rays = tracer->trace(beamline);

    writeToOutputCSV(rays, filename + ".rayx");

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

    writeToOutputCSV(out, filename + ".rayui");

    return out;
}

void compareRayLists(RAYX::RayList& rayx, RAYX::RayList& rayui, double t) {
    CHECK_EQ(rayx.rayAmount(), rayui.rayAmount());

    auto itRayX = rayx.begin();
    auto itRayXEnd = rayx.end();

    auto itRayUI = rayui.begin();
    auto itRayUIEnd = rayui.end();

    while (itRayX != itRayXEnd) {
        auto rayx = *itRayX;
        auto rayui = *itRayUI;

        CHECK_EQ(rayx.size(), rayui.size());

        for (unsigned int i = 0; i < rayui.size(); i++) {
            CHECK_EQ(rayx[i].m_position.x, rayui[i].m_position.x, t);
            CHECK_EQ(rayx[i].m_position.y, rayui[i].m_position.y, t);
            CHECK_EQ(rayx[i].m_position.z, rayui[i].m_position.z, t);

            CHECK_EQ(rayx[i].m_direction.x, rayui[i].m_direction.x, t);
            CHECK_EQ(rayx[i].m_direction.y, rayui[i].m_direction.y, t);
            CHECK_EQ(rayx[i].m_direction.z, rayui[i].m_direction.z, t);

            CHECK_EQ(rayx[i].m_energy, rayui[i].m_energy, t);
            // CHECK_EQ(rayx.m_pathLength, rayui[i].m_pathLength, t);
            // TODO: also compare pathLength

            CHECK_EQ(rayx[i].m_stokes.x, rayui[i].m_stokes.x, t);
            CHECK_EQ(rayx[i].m_stokes.y, rayui[i].m_stokes.y, t);
            CHECK_EQ(rayx[i].m_stokes.z, rayui[i].m_stokes.z, t);
            CHECK_EQ(rayx[i].m_stokes.w, rayui[i].m_stokes.w, t);
        }

        itRayX++;
        itRayUI++;

        bool endX = itRayX == itRayXEnd;
        bool endUI = itRayUI == itRayUIEnd;
        CHECK_EQ((int)endX, (int)endUI);
    }
}

void compareAgainstRayUI(std::string filename) {
    auto a = traceRML(filename);
    auto b = loadCSVRayUI(filename);
    compareRayLists(a, b);
}

// converts global coordinates to element coordinates.
// to be used in conjunction with runTracerRaw
std::vector<RAYX::Ray> mapGlobalToElement(
    std::vector<RAYX::Ray> global, std::shared_ptr<RAYX::OpticalElement> o) {
    glm::dmat4x4 transform = o->getInMatrix();
    std::vector<RAYX::Ray> out;

    for (auto r : global) {
        auto globalpos =
            arrayToGlm4({r.m_position.x, r.m_position.y, r.m_position.z, 1});
        auto globaldir =
            arrayToGlm4({r.m_direction.x, r.m_direction.y, r.m_direction.z, 0});

        auto elementpos = transform * globalpos;
        auto elementdir = transform * globaldir;

        auto r_element = r;
        r_element.m_position = {elementpos.x, elementpos.y, elementpos.z};
        r_element.m_direction = {elementdir.x, elementdir.y, elementdir.z};

        out.push_back(r_element);
    }
    return out;
}

RAYX::RayList mapGlobalToElementRayList(
    RAYX::RayList& global, std::shared_ptr<RAYX::OpticalElement> o) {
    RAYX::RayList out;
    for (auto l : global) {
        auto element = mapGlobalToElement(l, o);
        out.insertVector(element);
    }
    return out;
}
