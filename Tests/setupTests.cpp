#include "setupTests.h"

std::unique_ptr<RAYX::Tracer> tracer;

// helper functions

double parseDouble(std::string s) {
    double d;
#if defined(WIN32)
    if (sscanf_s(s.c_str(), "%le", &d) != 1) {
#else
    if (sscanf(s.c_str(), "%le", &d) != 1) {
#endif
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

/// will look at Tests/input/<filename>.rml
RAYX::Beamline loadBeamline(std::string filename) {
    std::string beamline_file = canonicalizeRepositoryPath("Tests/input/" + filename + ".rml").string();

    return RAYX::importBeamline(beamline_file);
}

/// will write to Tests/output/<filename>.csv
void writeToOutputCSV(const RAYX::Rays& rays, std::string filename) {
    std::string f = canonicalizeRepositoryPath("Tests/output/" + filename + ".csv").string();
    writeCSV(rays, f);
}

/// sequentialExtraParam yields the desired extraParam for rays which went the
/// sequential route through a beamline with `count` OpticalElements.
/// sequentialExtraParam(2) = 21
/// sequentialExtraParam(3) = 321 // i.e. first element 1, then 2 and then 3.
/// ...
int sequentialExtraParam(int count) {
    int out = 0;
    int fac = 1;
    for (int i = 1; i <= count; i++) {  // i goes from 1 to count
        out += i * fac;
        fac *= 10;
    }
    return out;
}

RAYX::Rays traceRML(std::string filename) {
    auto beamline = loadBeamline(filename);
    return tracer->trace(beamline);
}

std::vector<RAYX::Ray> extractLastHit(const RAYX::Rays& rays) {
    std::vector<RAYX::Ray> outs;
    for (auto rr : rays) {
        Ray out;
        out.m_weight = W_UNINIT;
        for (auto r : rr) {
            if (r.m_weight == W_JUST_HIT_ELEM) {
                out = r;
            }
        }
        if (out.m_weight != W_UNINIT) {
            outs.push_back(out);
        }
    }
    return outs;
}

/// will look at Tests/input/<filename>.csv
/// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
/// element coordinates of the relevant element!)
std::vector<RAYX::Ray> loadCSVRayUI(std::string filename) {
    std::string file = canonicalizeRepositoryPath("Tests/input/" + filename + ".csv").string();

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

void compareRays(const RAYX::Rays& r1, const RAYX::Rays& r2, double t) {
    CHECK_EQ(r1.size(), r2.size());

    auto it1 = r1.begin();
    auto it1end = r1.end();

    auto it2 = r2.begin();

    while (it1 != it1end) {
        auto& rr1 = *it1;
        auto& rr2 = *it2;

        CHECK_EQ(rr1.size(), rr2.size());

        auto itr1 = rr1.begin();
        auto itr1end = rr1.end();

        auto itr2 = rr2.begin();

        while (itr1 != itr1end) {
            auto ray1 = *itr1;
            auto ray2 = *itr2;

            CHECK_EQ(ray1, ray2);

            ++itr1;
            ++itr2;
        }

        ++it1;
        ++it2;
    }
}

// returns the Ray-X rays converted to be ray-UI compatible.
std::vector<RAYX::Ray> rayUiCompat(std::string filename) {
    auto beamline = loadBeamline(filename);
    auto rays = tracer->trace(beamline);

    std::vector<RAYX::Ray> out;

    for (auto ray_hist : rays) {
        std::vector<RAYX::Ray> ray_hist_only_hits;
        for (auto r : ray_hist) {
            if (r.m_weight == W_JUST_HIT_ELEM) {
                ray_hist_only_hits.push_back(r);
            }
        }

        // valid == ray_hist hits every element in the correct order.
        bool valid = ray_hist_only_hits.size() == beamline.m_OpticalElements.size();
        if (valid) {
            for (int i = 0; i < ray_hist_only_hits.size(); i++) {
                if (ray_hist_only_hits[i].m_lastElement - 1 != i) { // the -1 happens, as there is a +1 whenever lastElement gets set.
                    valid = false;
                    break;
                }
            }
        }

        if (valid) {
            out.push_back(ray_hist_only_hits.back());
        }
    }

    return out;
    //     auto beamline = loadBeamline(filename);
    // auto rays = tracer->trace(beamline);

    // int seq = sequentialExtraParam(beamline.m_OpticalElements.size());

    // std::vector<RAYX::Ray> out;

    // for (auto rr : rays) {
    //     for (auto r : rr) {
    //         // The ray has to be sequential (and it must finally end up at the last element of beamline)
    //         if (!intclose(r.m_extraParam, seq)) {
    //             continue;
    //         }

    //         // The ray has to have weight != W_FLY_OFF
    //         if (r.m_weight != W_JUST_HIT_ELEM) {
    //             continue;
    //         }

    //         out.push_back(r);
    //     }
    // }

    // return out;
}

void compareLastAgainstRayUI(std::string filename, double t) {
    auto rayx_list = rayUiCompat(filename);
    auto rayui_list = loadCSVRayUI(filename);

    writeToOutputCSV(convertToRays(rayx_list), filename + ".rayx");
    writeToOutputCSV(convertToRays(rayui_list), filename + ".rayui");

    CHECK_EQ(rayx_list.size(), rayui_list.size());

    auto itRayX = rayx_list.begin();
    auto itRayXEnd = rayx_list.end();

    auto itRayUI = rayui_list.begin();

    while (itRayX != itRayXEnd) {
        auto rayx = *itRayX;
        auto correct = *itRayUI;

        // TODO: make more properties of the rays work the same!
        CHECK_EQ(rayx.m_position, correct.m_position, t);
        CHECK_EQ(rayx.m_direction, correct.m_direction, t);
        CHECK_EQ(rayx.m_energy, correct.m_energy, t);

        ++itRayX;
        ++itRayUI;
    }
}

void compareAgainstCorrect(std::string filename, double tolerance) {
    auto a = traceRML(filename);

    std::string f = canonicalizeRepositoryPath("Tests/input/" + filename + ".correct.csv").string();
    auto b = loadCSV(f);

    writeToOutputCSV(a, filename + ".rayx");
    compareRays(a, b, tolerance);
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
