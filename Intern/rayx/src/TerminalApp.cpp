#include "TerminalApp.h"

#include <filesystem>
#include <memory>
#include <stdexcept>

#include "Debug/Debug.h"
#include "Random.h"
#include "Rml/Importer.h"
#include "Rml/Locate.h"
#include "Tracer/Tracer.h"
#include "Writer/Writer.h"

TerminalApp::TerminalApp(int argc, char** argv) : m_argv(argv), m_argc(argc) {
    RAYX_VERB << "TerminalApp created!";

// This should not be a RAYX_VERB, as it helps a lot to see the git hash
// if someone opens an issue.
// This uses std::cout instead of RAYX_LOG to not add the debugging thing [TerminalApp.cpp:..],
// as this is intended to be read by a user.
#ifdef RAYX_DEBUG_MODE
    std::string mode = "debug";
#else
    std::string mode = "release";
#endif
    std::cout << "Starting rayx (" << GIT_REVISION << ", " << mode << ")" << std::endl;

    /// warn if the binary is compiled with 32-bit (i.e. sizeof(void*) == 4)
    /// or worse.
    /// 64-bit has sizeof(void*) == 8.
    if (sizeof(void*) <= 4) {
        RAYX_WARN << "This application should be compiled as 64-bit! Using 32-bit"
                     "will make this program run out of memory for larger ray "
                     "numbers!";
    }
}

TerminalApp::~TerminalApp() { RAYX_VERB << "TerminalApp deleted!"; }

void TerminalApp::tracePath(const std::filesystem::path& path) {
    namespace fs = std::filesystem;
    if (!fs::exists(path)) {
        if (path.empty()) {
            RAYX_EXIT << "No input file provided!";
        } else {
            RAYX_EXIT << "File '" << path << "' not found!";
        }
    }

    if (fs::is_directory(path)) {
        for (const auto& p : fs::directory_iterator(path)) {
            tracePath(p.path());
        }
    } else if (path.extension() == ".rml") {
        std::cout << "Tracing File: " << path << std::endl;
        // Load RML file
        m_Beamline = std::make_unique<RAYX::Beamline>(RAYX::importBeamline(path));

        // calculate max batch size
        uint64_t max_batch_size = RAYX::DEFAULT_BATCH_SIZE;
        if (m_CommandParser->m_args.m_BatchSize != 0) {
            max_batch_size = m_CommandParser->m_args.m_BatchSize;
        }

        // Run rayx core
        RAYX::Sequential seq = m_CommandParser->m_args.m_sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;
        int maxEvents =
            (m_CommandParser->m_args.m_maxEvents < 1) ? RAYX::Tracer::defaultMaxEvents(m_Beamline.get()) : m_CommandParser->m_args.m_maxEvents;
        int recordElementIndex = m_CommandParser->m_args.m_recordElementIndex;

        auto rays = m_Tracer->trace(*m_Beamline, seq, max_batch_size, maxEvents, recordElementIndex);

        if (rays.empty())
            RAYX_WARN << "No events were recorded!";
        else {
            bool isCSV = m_CommandParser->m_args.m_csvFlag;
            Format fmt = formatFromString(m_CommandParser->m_args.m_format);

            std::filesystem::path outputPath;
            if (!m_CommandParser->m_args.m_outPath.empty()) {
                outputPath = m_CommandParser->m_args.m_outPath;
            } else {
                outputPath = path;
                outputPath.replace_extension("");  // strip .rml
            }
            outputPath.replace_extension(isCSV ? ".csv" : ".h5");

            // Error handling in case provided path does not exist
            auto parent = outputPath.parent_path();
            if (!parent.empty() && !std::filesystem::exists(parent)) {
                RAYX_EXIT << "Output directory '" << parent.string() << "' does not exist. Create it first or use a different -o path.";
            }

            auto file = exportRays(rays, isCSV, outputPath, fmt);

            // Plot
            if (m_CommandParser->m_args.m_plotFlag) {
                if (isCSV) {
                    RAYX_WARN << "You can only plot .h5 files!";
                    RAYX_EXIT << "Have you selected .csv exporting?";
                }

                auto cmd =
                    std::string("python ") + RAYX::ResourceHandler::getInstance().getResourcePath("Scripts/plot.py").string() + " " + file.string();
                auto ret = system(cmd.c_str());
                if (ret != 0) {
                    RAYX_WARN << "received error code while printing";
                }
            }
        }
    } else {
        RAYX_VERB << "ignoring non-rml file: '" << path << "'";
    }
}

void TerminalApp::run() {
    RAYX_VERB << "TerminalApp running...";

    /////////////////// Argument Parser
    m_CommandParser = std::make_unique<CommandParser>(m_argc, m_argv);
    // Check correct use (This will exit if error)
    m_CommandParser->analyzeCommands();
    if (m_CommandParser->m_args.m_verbose) {
        RAYX::setDebugVerbose(true);
    }

    if (m_CommandParser->m_args.m_isFixSeed) {
        if (m_CommandParser->m_args.m_seed != -1) {
            RAYX::fixSeed(m_CommandParser->m_args.m_seed);
        } else
            RAYX::fixSeed(RAYX::FIXED_SEED);

    } else {
        RAYX::randomSeed();
    }

    if (m_CommandParser->m_args.m_benchmark) {
        RAYX_VERB << "Starting in Benchmark Mode.\n";
        RAYX::BENCH_FLAG = true;
    }
    /////////////////// Argument treatement
    if (m_CommandParser->m_args.m_version) {
        m_CommandParser->getVersion();
        exit(0);
    }

    auto argToDeviceType = [&] {
        using DeviceType = RAYX::DeviceConfig::DeviceType;
        if (m_CommandParser->m_args.m_cpuFlag == m_CommandParser->m_args.m_gpuFlag) return DeviceType::All;
        return m_CommandParser->m_args.m_cpuFlag ? DeviceType::Cpu : DeviceType::Gpu;
    };

    auto deviceType = argToDeviceType();

    if (m_CommandParser->m_args.m_listDevices) {
        RAYX::DeviceConfig(deviceType).dumpDevices();
        exit(0);
    }

    // Choose Hardware
    auto getDevice = [&] {
        if (m_CommandParser->m_args.m_deviceID != -1) {
            return RAYX::DeviceConfig(deviceType).enableDeviceByIndex(m_CommandParser->m_args.m_deviceID);
        } else {
            return RAYX::DeviceConfig(deviceType).enableBestDevice();
        }
    };
    m_Tracer = std::make_unique<RAYX::Tracer>(getDevice());

    // Trace, export and plot
    tracePath(m_CommandParser->m_args.m_providedFile);
}

std::filesystem::path TerminalApp::exportRays(const RAYX::BundleHistory& hist, bool isCSV, const std::filesystem::path& path, const Format& fmt) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    if (isCSV) {
        writeCSV(hist, path.string(), fmt);
    } else {
#ifdef NO_H5
        RAYX_EXIT << "writeH5 called during NO_H5 (HDF5 disabled during build)";
#else
        writeH5(hist, path.string(), fmt, getBeamlineOpticalElementsNames());
#endif
    }

    return path;
}

/**
 * @brief Get all beamline optical element names
 *
 * @return std::vector<std::string> list of names
 */
std::vector<std::string> TerminalApp::getBeamlineOpticalElementsNames() {
    std::vector<std::string> names;
    // getElements() returns a vector of pointers to DesignElement, traversing the entire tree.
    auto elements = m_Beamline->getElements();
    names.reserve(elements.size());
    for (const auto elem : elements) {
        // Assuming DesignElement provides a getName() method.
        names.push_back(elem->getName());
    }
    return names;
}

/**
 * @brief Get all beamline light sources names
 *
 * @return std::vector<std::string> list of names
 */
std::vector<std::string> TerminalApp::getBeamlineLightSourcesNames() {
    std::vector<std::string> names;
    // getSources() returns a vector of pointers to DesignSource.
    auto sources = m_Beamline->getSources();
    names.reserve(sources.size());
    for (const auto src : sources) {
        names.push_back(src->getName());
    }
    return names;
}
