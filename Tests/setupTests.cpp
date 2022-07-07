#include "setupTests.h"

#include <fstream>
#include <functional>
#include <sstream>
#include <type_traits>

const int PREC = 17;

void checkEq(std::string filename, int line, std::string l, std::string r,
             std::vector<double> vl, std::vector<double> vr, double tolerance) {
    if (vl.size() != vr.size()) {
        RAYX::Warn(filename, line)
            << l << " != " << r << ": different lengths!";
        ADD_FAILURE();
        return;
    }

    bool success = true;
    for (size_t i = 0; i < vl.size(); i++) {
        if (abs(vl[i] - vr[i]) > tolerance) {
            success = false;
            break;
        }
    }
    if (success) return;

    RAYX::Warn(filename, line) << l << " != " << r << ":";

    int counter = 0;  // stores the number of elements in the stringstream
    std::stringstream s;
    for (size_t i = 0; i < vl.size(); i++) {
        if (counter != 0) {
            s << " ";
        }
        if (abs(vl[i] - vr[i]) <= tolerance) {
            s << std::setprecision(PREC) << vl[i] << "|" << vr[i];
        } else {
            s << "\x1B[36m" << std::setprecision(PREC) << vl[i] << "|" << vr[i]
              << "\x1B[31m";
        }

        counter++;
        if (counter == 4 &&
            vl.size() == 16) {  // 4x4 things should be written in 4 rows
            counter = 0;
            RAYX::Warn(filename, line) << s.str();
            s = std::stringstream();
        }
    }
    if (counter > 0) {
        RAYX::Warn(filename, line) << s.str();
    }
    ADD_FAILURE();
}

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

// will look at Tests/rml_files/test_shader/<filename>.rml
RAYX::Beamline loadBeamline(std::string filename) {
    std::string beamline_file =
        resolvePath("Tests/rml_files/test_shader/" + filename + ".rml");

    return RAYX::importBeamline(beamline_file);
}

// will write to Tests/output-new/<filename>.csv
void writeToOutputCSV(RAYX::RayList& rays, std::string filename) {
    std::string f = resolvePath("Tests/output-new/" + filename + ".csv");
    writeCSV(rays, f);
}

// if convertToElementCoords = true, all rays are converted to element
// coordinates of beamline->back()
RAYX::RayList traceRML(std::string filename, bool convertToElementCoords) {
    auto beamline = loadBeamline(filename);
    auto rays = tracer->trace(beamline);

    if (convertToElementCoords) {
        if (beamline.m_OpticalElements.empty()) {
            ADD_FAILURE() << "no optical elements!";
        }

        rays =
            mapGlobalToElementRayList(rays, beamline.m_OpticalElements.back());
    }

    writeToOutputCSV(rays, filename + ".rayx");

    return rays;
}

// will look at Tests/rml_files/test_shader/<filename>.csv
// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
// element coordinates of the relevant element!)
RAYX::RayList loadCSVRayUI(std::string filename) {
    std::string file =
        resolvePath("Tests/rml_files/test_shader/" + filename + ".csv");

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
    glm::dmat4x4 transform = arrayToGlm16(o->getInMatrix());
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
