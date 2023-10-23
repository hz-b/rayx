#include "TerminalApp.h"

#include <filesystem>
#include <memory>
#include <stdexcept>

#include "CanonicalizePath.h"
#include "Data/Importer.h"
#include "Debug/Debug.h"
#include "Random.h"
#include "Tracer/CpuTracer.h"
#include "Tracer/VulkanTracer.h"
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
            RAYX_ERR << "No input file provided!";
        } else {
            RAYX_ERR << "File '" << path << "' not found!";
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
        uint64_t max_batch_size = DEFAULT_BATCH_SIZE;
        if (m_CommandParser->m_args.m_BatchSize != 0) {
            max_batch_size = m_CommandParser->m_args.m_BatchSize;
        }

        // Run rayx core
        RAYX::Sequential seq = m_CommandParser->m_args.m_sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;
        auto rays = m_Tracer->trace(*m_Beamline, seq, max_batch_size, m_CommandParser->m_args.m_setThreads);

        // Export Rays to external data.
        auto file = exportRays(rays, path.string());

        // Plot
        if (m_CommandParser->m_args.m_plotFlag) {
            if (!file.ends_with(".h5")) {
                RAYX_WARN << "You can only plot .h5 files!";
                RAYX_ERR << "Have you selected .csv exporting?";
            }

            auto cmd = std::string("python ") + RAYX::canonicalizeRepositoryPath(std::string("Scripts/plot.py")).string() + " " + file;
            auto ret = system(cmd.c_str());
            if (ret != 0) {
                RAYX_WARN << "received error code while printing";
            }
        }

#if defined(RAYX_DEBUG_MODE)
        // Export Debug Matrics.
        exportDebug();
#endif

    } else {
        RAYX_VERB << "ignoring non-rml file: '" << path << "'";
    }
}

void TerminalApp::run() {
    RAYX_PROFILE_FUNCTION();

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
        exit(1);
    }

    // Choose Hardware
    if (m_CommandParser->m_args.m_cpuFlag) {
        m_Tracer = std::make_unique<RAYX::CpuTracer>();
    } else {
#ifdef NO_VULKAN
        RAYX_ERR << "NO_VULKAN: trying to construct VulkanTracer with Vulkan disabled. Use -x to use CPU tracer or enable Vulkan when building.";
#else
        m_Tracer = std::make_unique<RAYX::VulkanTracer>();

        /// list physical devices
        if (m_CommandParser->m_args.m_listDevices) {
            dynamic_cast<RAYX::VulkanTracer*>(m_Tracer.get())->listPhysicalDevices();
            exit(0);
        }
        /// select physical device
        if (m_CommandParser->m_args.m_deviceID >= 0) {
            m_Tracer->setDevice(m_CommandParser->m_args.m_deviceID);
        }

#endif
    }

    // Trace, export and plot
    tracePath(m_CommandParser->m_args.m_providedFile);
}

std::string TerminalApp::exportRays(const RAYX::BundleHistory& hist, std::string path) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    bool csv = m_CommandParser->m_args.m_csvFlag;

    // strip .rml
    if (path.ends_with(".rml")) {
        path = path.substr(0, path.length() - 4);
    } else {
        RAYX_ERR << "Input file is not an *.rml file!";
    }

    Format fmt = formatFromString(m_CommandParser->m_args.m_format);

    if (csv) {
        path += ".csv";
        writeCSV(hist, path, fmt);
    } else {
#ifdef NO_H5
        RAYX_ERR << "writeH5 called during NO_H5 (HDF5 disabled during build))";
#else
        path += ".h5";
        writeH5(hist, path, fmt, getBeamlineOpticalElementsNames());
#endif
    }
    return path;
}

#if defined(RAYX_DEBUG_MODE)
/**
 * @brief Gets All Debug Buffers and check if they are the identity matrix.
 * This is a default function to show how the implemented Debug Buffer works.
 * You can write your own checking func.
 *
 * Debugging Matrices are only available on Vulkan Tracing In Debug mode
 *
 */
void TerminalApp::exportDebug() {
#ifndef NO_VULKAN
    if (m_CommandParser->m_args.m_cpuFlag) {
        return;
    }
    auto d = (const RAYX::VulkanTracer*)(m_Tracer.get());
    int index = 0;
    RAYX_VERB << "Debug Matrix Check...";
    for (auto m : d->getDebugList()) {
        if (isIdentMatrix(m._dMat)) {
            printDMat4(m._dMat);
            RAYX_D_ERR << "@" << index;
        }
        index += 1;
    }
#endif
}
#endif

/**
 * @brief Get all beamline optical elemet names
 *
 * @return std::vector<std::string> list of names
 */
std::vector<std::string> TerminalApp::getBeamlineOpticalElementsNames() {
    std::vector<std::string> names;
    names.reserve(m_Beamline->m_OpticalElements.size());

    for (const auto& opticalElement : m_Beamline->m_OpticalElements) {
        names.push_back(opticalElement.m_name);
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
    names.reserve(m_Beamline->m_LightSources.size());

    for (const auto& lightSources : m_Beamline->m_LightSources) {
        names.push_back(lightSources->m_name);
    }

    return names;
}
