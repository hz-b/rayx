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
    ray.m_padding = -1;
    ray.m_eventType = -1;
    ray.m_lastElement = -1;
    ray.m_order = -1;

    return ray;
}

/// will look at Intern/rayx-core/tests/input/<filename>.rml
RAYX::Beamline loadBeamline(std::string filename) {
    std::string beamline_file = canonicalizeRepositoryPath("Intern/rayx-core/tests/input/" + filename + ".rml").string();

    return RAYX::importBeamline(beamline_file);
}

/// will write to Intern/rayx-core/tests/output<filename>.csv
void writeToOutputCSV(const RAYX::BundleHistory& hist, std::string filename) {
    std::string f = canonicalizeRepositoryPath("Intern/rayx-core/tests/output/" + filename + ".csv").string();
    writeCSV(hist, f, FULL_FORMAT);
}

RAYX::BundleHistory traceRML(std::string filename) {
    auto beamline = loadBeamline(filename);
    return tracer->trace(beamline, Sequential::No, DEFAULT_BATCH_SIZE);
}

std::vector<RAYX::Event> extractLastHit(const RAYX::BundleHistory& hist) {
    std::vector<RAYX::Event> outs;
    for (auto rr : hist) {
        Ray out;
        out.m_eventType = ETYPE_UNINIT;
        for (auto r : rr) {
            if (r.m_eventType == ETYPE_JUST_HIT_ELEM) {
                out = r;
            }
        }
        if (out.m_eventType != ETYPE_UNINIT) {
            outs.push_back(out);
        }
    }
    return outs;
}

/// will look at Intern/rayx-core/tests/input/<filename>.csv
/// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
/// element coordinates of the relevant element!)
std::vector<RAYX::Ray> loadCSVRayUI(std::string filename) {
    std::string file = canonicalizeRepositoryPath("Intern/rayx-core/tests/input/" + filename + ".csv").string();

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

void compareBundleHistories(const RAYX::BundleHistory& r1, const RAYX::BundleHistory& r2, double t) {
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

            CHECK_EQ(ray1, ray2, t);

            ++itr1;
            ++itr2;
        }

        ++it1;
        ++it2;
    }
}

// If the ray from `ray_hist` went through the whole beamline sequentially, we return its last hit event.
// Otherwise we return `{}`, aka None.
std::optional<RAYX::Ray> lastSequentialHit(RayHistory ray_hist, unsigned int beamline_len) {
    // The ray should hit every element from the beamline once, plus one FLY_OFF event.
    if (ray_hist.size() != beamline_len + 1) {
        return {};
    }

    for (int i = 0; i < beamline_len; i++) {
        if (ray_hist[i].m_lastElement != i) {
            return {};
        }
        if (ray_hist[i].m_eventType != ETYPE_JUST_HIT_ELEM) {
            return {};
        }
    }

    // this returns the last 'hit' event.
    return ray_hist[ray_hist.size() - 2];
}

// returns the rayx rays converted to be ray-UI compatible.
std::vector<RAYX::Ray> rayUiCompat(std::string filename, Sequential seq=Sequential::No) {
    auto beamline = loadBeamline(filename);
    BundleHistory hist = tracer->trace(beamline, seq, DEFAULT_BATCH_SIZE);

    std::vector<RAYX::Ray> out;

    for (auto ray_hist : hist) {
        auto opt_ray = lastSequentialHit(ray_hist, beamline.m_OpticalElements.size());
        if (opt_ray) {
            out.push_back(*opt_ray);
        }
    }

    return out;
}

void compareLastAgainstRayUI(std::string filename, double tolerance, Sequential seq) {
    auto rayx_list = rayUiCompat(filename, seq);
    auto rayui_list = loadCSVRayUI(filename);

    writeToOutputCSV(convertToBundleHistory(rayx_list), filename + ".rayx");
    writeToOutputCSV(convertToBundleHistory(rayui_list), filename + ".rayui");

    CHECK_EQ(rayx_list.size(), rayui_list.size());

    auto itRayX = rayx_list.begin();
    auto itRayXEnd = rayx_list.end();

    auto itRayUI = rayui_list.begin();

    while (itRayX != itRayXEnd) {
        auto rayx = *itRayX;
        auto correct = *itRayUI;

        // TODO: make more properties of the rays work the same!
        CHECK_EQ(rayx.m_position, correct.m_position, tolerance);
        CHECK_EQ(rayx.m_direction, correct.m_direction, tolerance);
        CHECK_EQ(rayx.m_energy, correct.m_energy, tolerance);

        ++itRayX;
        ++itRayUI;
    }
}

void compareAgainstCorrect(std::string filename, double tolerance) {
    auto a = traceRML(filename);

    std::string f = canonicalizeRepositoryPath("Intern/rayx-core/tests/input/" + filename + ".correct.csv").string();
    auto b = loadCSV(f);

    writeToOutputCSV(a, filename + ".rayx");
    compareBundleHistories(a, b, tolerance);
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
