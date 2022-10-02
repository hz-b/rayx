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
    std::string beamline_file = canonicalizeRepositoryPath("Tests/input/" + filename + ".rml");

    return RAYX::importBeamline(beamline_file);
}

// will write to Tests/output/<filename>.csv
void writeToOutputCSV(std::vector<RAYX::Ray>& rays, std::string filename) {
    std::string f = canonicalizeRepositoryPath("Tests/output/" + filename + ".csv");
    writeCSV(rays, f);
}

// sequentialExtraParam yields the desired extraParam for rays which went the
// sequential route through a beamline with `count` OpticalElements.
// sequentialExtraParam(2) = 21
// sequentialExtraParam(3) = 321 // i.e. first element 1, then 2 and then 3.
// ...
int sequentialExtraParam(int count) {
    int out = 0;
    int fac = 1;
    for (int i = 1; i <= count; i++) {  // i goes from 1 to count
        out += i * fac;
        fac *= 10;
    }
    return out;
}

std::vector<RAYX::Ray> traceRML(std::string filename, Filter filter) {
    auto beamline = loadBeamline(filename);

    // the rays satisfying the weight != 0 test.
    std::vector<RAYX::Ray> wRays;
    {
        auto rays = tracer->trace(beamline);
        wRays.reserve(rays.size());
        for (auto r : rays) {
            if (r.m_weight != 0) {
                wRays.push_back(r);
            }
        }
    }

    if (filter == Filter::OnlySequentialRays) {
        auto extra = sequentialExtraParam(beamline.m_OpticalElements.size());

        // the rays satisfying the extraParam test.
        std::vector<RAYX::Ray> eRays;

        eRays.reserve(wRays.size());
        for (auto r : wRays) {
            if (intclose(r.m_extraParam, extra)) {
                eRays.push_back(r);
            }
        }
        return eRays;
    } else {
        return wRays;
    }
}

// will look at Tests/input/<filename>.csv
// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
// element coordinates of the relevant element!)
std::vector<RAYX::Ray> loadCSVRayUI(std::string filename) {
    std::string file = canonicalizeRepositoryPath("Tests/input/" + filename + ".csv");

    std::ifstream f(file);
    std::string line;

    // discard first two lines
    for (int i = 0; i < 2; i++) {
        std::getline(f, line);
    }

    std::vector<RAYX::Ray> out;

    while (std::getline(f, line)) {
        out.push_back(parseCSVline(line));
    }

    out.shrink_to_fit();

    return out;
}

void compareRayLists(const std::vector<RAYX::Ray>& rayx_list,
                     const std::vector<RAYX::Ray>& rayui_list, double t) {
    CHECK_EQ(rayx_list.size(), rayui_list.size());

    auto itRayX = rayx_list.begin();
    auto itRayXEnd = rayx_list.end();

    auto itRayUI = rayui_list.begin();

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

    auto a = traceRML(filename, Filter::OnlySequentialRays);
    auto b = loadCSVRayUI(filename);

    writeToOutputCSV(a, filename + ".rayx");
    writeToOutputCSV(b, filename + ".rayui");

    compareRayLists(a, b, tolerance);
}

void updateCpuTracerMaterialTables(std::vector<Material> mats_vec) {
    std::array<bool, 92> mats;
    mats.fill(false);
    for (auto m : mats_vec) {
        mats[static_cast<int>(m) - 1] = true;
    }
    auto materialTables = loadMaterialTables(mats);
    CPU_TRACER::mat.data = materialTables.materialTable;
    CPU_TRACER::matIdx.data = materialTables.indexTable;
}
