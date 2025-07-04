#include "TerminalApp.h"

#ifndef NO_H5
#include <highfive/H5DataSet.hpp>
#include <highfive/H5File.hpp>
#endif

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Debug/Debug.h"
#include "Random.h"
#include "Rml/Importer.h"
#include "Rml/Locate.h"
#include "Tracer/Tracer.h"
#include "Writer/CsvWriter.h"
#include "Writer/Format.h"
#include "Writer/H5Writer.h"

namespace {

void dumpBeamline(const std::filesystem::path& filepath) {
    std::cout << "dumping beamline meta data from: " << filepath << std::endl;

    const auto beamline = std::make_unique<RAYX::Beamline>(RAYX::importBeamline(filepath.string()));

    const auto sources = beamline->getSources();
    std::cout << "\tsources (" << sources.size() << "):" << std::endl;

    int objectIndex = 0;
    for (const auto* source : sources) {
        std::cout << "\t- [" << objectIndex << "] '" << source->getName() << "' \t(type: " << RAYX::elementTypeToString(source->getType())
                  << ", number of rays: " << source->getNumberOfRays() << ")" << std::endl;
        ++objectIndex;
    }

    objectIndex = 0;  // TODO: this line can be removed as soon as we support recording of generated rays

    const auto elements = beamline->getElements();
    std::cout << "\telements (" << elements.size() << "):" << std::endl;

    for (const auto& element : elements) {
        const auto curvature = RAYX::curvatureTypeToString(element->getCurvatureType());
        const auto behaviour = RAYX::behaviourTypeToString(element->getBehaviourType());
        std::cout << "\t- [" << objectIndex << "] '" << element->getName() << "' \t(curvature: " << curvature << ", behviour: " << behaviour << ")"
                  << std::endl;
        ++objectIndex;
    }
}

#ifndef NO_H5
void scanGroup(const HighFive::Group& group, const int depth = 0, const std::string& path = "/") {
    size_t num_objs = group.getNumberObjects();
    for (size_t i = 0; i < num_objs; ++i) {
        std::string obj_name = group.getObjectName(i);
        std::string full_path = path == "/" ? "/" + obj_name : path + "/" + obj_name;
        auto obj_type = group.getObjectType(obj_name);

        auto indent = [depth]() {
            for (int i = 0; i < depth; ++i) std::cout << "\t";
        };

        // Print object type and full path
        switch (obj_type) {
            case HighFive::ObjectType::Group:
                indent();
                std::cout << full_path << " (group)" << std::endl;
                // Recursively scan sub-group
                scanGroup(group.getGroup(obj_name), depth + 1, full_path);
                break;
            case HighFive::ObjectType::Dataset:
                indent();
                std::cout << full_path << " (dataset)" << std::endl;
                break;
            default:
                indent();
                std::cout << full_path << " (unknown)" << std::endl;
        }
    }
}

void dumpH5File(const std::filesystem::path& filepath) {
    try {
        std::cout << "reading h5 meta data from: " << filepath << std::endl;
        auto file = HighFive::File(filepath.string(), HighFive::File::ReadOnly);
        std::cout << "\tfilesize: " << file.getFileSize() << std::endl;
        scanGroup(file.getGroup("/"), 1);
    } catch (const std::exception& e) {
        RAYX_EXIT << "exception caught while attempting to read h5 file: " << e.what();
    }
}
#endif

}  // unnamed namespace

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
        if (m_CommandParser->m_args.m_recordIndices.empty()) {
            RAYX_VERB << "Passed no or empty array for which elements to record.";
        } else {
            RAYX_VERB << "First element of passed array: " << m_CommandParser->m_args.m_recordIndices[0];
        }

        // Run rayx core
        RAYX::Sequential seq = m_CommandParser->m_args.m_sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;
        const int maxEvents =
            (m_CommandParser->m_args.m_maxEvents < 1) ? RAYX::Tracer::defaultMaxEvents(m_Beamline.get()) : m_CommandParser->m_args.m_maxEvents;
        size_t numElements = m_Beamline->numElements();

        // Record mask
        auto recordIndices = m_CommandParser->m_args.m_recordIndices;
        std::vector<bool> recordMask(numElements, true);
        if (!recordIndices.empty()) {
            recordMask = std::vector<bool>(numElements, false);
            for (auto idx : recordIndices) {
                if (idx < 0) RAYX_EXIT << "Only positive indices are possible for CLI option: -R/--record-indices!";
                if (idx > numElements - 1) RAYX_EXIT << "Index {" << idx << "} provided with -R/--record-indices does not exist in the provided file.";
                recordMask[idx] = true;
            }
        }

        // Verbose log: print mask as a string of 0s and 1s
        std::string maskStr;
        maskStr.reserve(numElements);
        for (auto b : recordMask) {
            maskStr += b;
        }
        RAYX_VERB << "recordMask [size=" << numElements << "]: " << maskStr;

        const auto attr =
            m_CommandParser->m_args.m_format.empty() ? RAYX::RayAttrFlag::All : RAYX::formatStringToRayAttrFlag(m_CommandParser->m_args.m_format);

        const auto rays = m_Tracer->trace(*m_Beamline, seq, max_batch_size, maxEvents, recordMask, attr);

        if (!rays.num_events)
            std::cout << "No events were recorded!" << std::endl;
        else {
            bool isCSV = m_CommandParser->m_args.m_csvFlag;

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

            auto file = exportRays(rays, isCSV, outputPath, attr);

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

    if (!m_CommandParser->m_args.m_dump.empty()) {
        const auto filename = m_CommandParser->m_args.m_dump;
        const auto filepath = std::filesystem::path(filename);
        const auto filetype = filepath.extension();

        if (filetype == ".rml")
            dumpBeamline(filepath);
        else if (filetype == ".h5")
#ifndef NO_H5
            dumpH5File(filepath);
#else
            RAYX_EXIT << "error: unable to dump h5 file due to hdf5 was disabled during build.";
#endif
        else
            RAYX_EXIT << "error: unable to dump file '" << filename << "', unknown filetype. supported filetypes are h5 and rml";

        return;
    }

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

std::filesystem::path TerminalApp::exportRays(const RAYX::RaySoA& rays, bool isCSV, const std::filesystem::path& path, const RAYX::RayAttrFlag attr) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    if (isCSV) {
        writeCsv(RAYX::raySoAToBundleHistory(rays), path.string());
    } else {
#ifdef NO_H5
        RAYX_EXIT << "writeH5 called during NO_H5 (HDF5 disabled during build)";
#else
        writeH5RaySoA(path, rays, attr);
#endif
    }

    return path;
}
