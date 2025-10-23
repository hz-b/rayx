#include "TerminalApp.h"

#ifndef NO_H5
#include <highfive/H5DataSet.hpp>
#include <highfive/H5File.hpp>
#endif

#include <algorithm>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Beamline/StringConversion.h"
#include "Debug/Debug.h"
#include "Random.h"
#include "Rml/Importer.h"
#include "Rml/Locate.h"
#include "Tracer/Tracer.h"
#include "Writer/CsvWriter.h"
#include "Writer/H5Writer.h"

namespace fs = std::filesystem;

namespace {

void dumpBeamlineObjects(const RAYX::Beamline* beamline) {
    const auto sources = beamline->getSources();
    std::cout << "\tsources (" << sources.size() << "):" << std::endl;

    int objectIndex = 0;
    for (const auto* source : sources) {
        std::cout << "\t- [" << objectIndex << "] '" << source->getName() << "' \t(type: " << RAYX::ElementTypeToString.at(source->getType())
                  << ", number of rays: " << source->getNumberOfRays() << ")" << std::endl;
        ++objectIndex;
    }

    const auto elements = beamline->getElements();
    std::cout << "\telements (" << elements.size() << "):" << std::endl;

    for (const auto& element : elements) {
        const auto curvature = RAYX::CurvatureTypeToString.at(element->getCurvatureType());
        const auto behaviour = RAYX::BehaviourTypeToString.at(element->getBehaviourType());
        std::cout << "\t- [" << objectIndex << "] '" << element->getName() << "' \t(curvature: " << curvature << ", behviour: " << behaviour << ")"
                  << std::endl;
        ++objectIndex;
    }
}

void dumpBeamline(const fs::path& filepath) {
    std::cout << "dumping beamline meta data from: " << filepath << std::endl;
    const auto beamline = std::make_unique<RAYX::Beamline>(RAYX::importBeamline(filepath.string()));
    dumpBeamlineObjects(beamline.get());
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
                // TODO: dump dataset datatype and number of elements
                std::cout << full_path << " (dataset)" << std::endl;
                break;
            default:
                indent();
                std::cout << full_path << " (unknown)" << std::endl;
        }
    }
}

void dumpH5File(const fs::path& filepath) {
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

TerminalApp::TerminalApp(int argc, char** argv) {
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

    m_cliArgs = parseCliArgs(argc, argv);
}

TerminalApp::~TerminalApp() { RAYX_VERB << "TerminalApp deleted!"; }

int TerminalApp::tracePath(const fs::path& path) {
    if (!fs::exists(path)) {
        RAYX_EXIT << "Trying to access file or directory " << path << " but it was not found!";
    }

    auto rmlCounter = 0;

    if (fs::is_directory(path)) {
        for (const auto& p : fs::directory_iterator(path)) {
            rmlCounter += tracePath(p.path());
        }
    } else if (path.extension() == ".rml") {
        traceRmlAndExportRays(path);
        rmlCounter += 1;
    } else {
        RAYX_VERB << "ignoring non-rml file: '" << path << "'";
    }

    return rmlCounter;
}

void TerminalApp::traceRmlAndExportRays(const fs::path& inputFilepath) {
    using namespace std::chrono;
    const auto start_time = steady_clock::now();

    std::cout << "Processing: " << inputFilepath << std::endl;

    // record mask for attributes. determine which ray attributes should be recorded
    const auto attrRecordMask = RAYX::rayAttrStringsToRayAttrMask(m_cliArgs.attrRecordMask);

    const auto beamline = loadBeamline(inputFilepath);

    const auto rays = traceBeamline(beamline, attrRecordMask);

    const auto objectNames = beamline.getObjectNames();
    const auto outputFilepath = exportRays(inputFilepath, objectNames, rays, attrRecordMask);

    // print elapsed time and output filepath

    using namespace std::chrono_literals;
    const auto end_time = steady_clock::now();
    const auto elapsed_time = duration_cast<milliseconds>(end_time - start_time);
    const auto mins = duration_cast<minutes>(elapsed_time);
    const auto secs = duration_cast<seconds>(elapsed_time % 1min);
    const auto millis = duration_cast<milliseconds>(elapsed_time % 1s);

    std::cout << "Finished in ";
    if (mins > 0min) std::cout << mins.count() << "m ";
    if (secs > 0s) std::cout << secs.count() << "s ";
    std::cout << millis.count() << "ms.";

    if (outputFilepath.empty())
        std::cout << " No rays were exported." << std::endl;
    else
        std::cout << " Exported rays to: " << fs::absolute(outputFilepath) << std::endl;
}

RAYX::Beamline TerminalApp::loadBeamline(const fs::path& filepath) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto beamline = RAYX::importBeamline(filepath);

    // override number of rays for all sources
    if (m_cliArgs.numberOfRays) {
        beamline.traverse([n = *m_cliArgs.numberOfRays](RAYX::BeamlineNode& node) -> bool {
            if (node.isSource()) {
                auto* source = static_cast<RAYX::DesignSource*>(&node);
                source->setNumberOfRays(n);
            }
            return false;
        });
    }

    return beamline;
}

RAYX::Rays TerminalApp::traceBeamline(const RAYX::Beamline& beamline, const RAYX::RayAttrMask attrRecordMask) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    // dump beamline objects
    if (RAYX::getDebugVerbose()) {
        dumpBeamlineObjects(&beamline);
    }

    const size_t numSources = beamline.numSources();
    const size_t numElements = beamline.numElements();

    // record mask for elements. determine which elements should be recorded
    auto objectRecordMask = m_cliArgs.objectRecordIndices.empty()
                                ? RAYX::ObjectIndexMask::all(numSources, numElements)
                                : RAYX::ObjectIndexMask::byIndices(numSources, numElements, m_cliArgs.objectRecordIndices);

    if (m_cliArgs.objectRecordIndices.empty()) {
        RAYX_VERB << "Record indices is empty. Defaulting to recording all elements";
    }

    if (RAYX::getDebugVerbose()) {
        const auto objectNames = beamline.getObjectNames();
        RAYX_VERB << "Recording objects:";
        for (int i = 0; i < objectRecordMask.numObjects(); ++i) {
            const auto shouldRecord = objectRecordMask.shouldRecordObject(i) ? "Yes" : "No";
            RAYX_VERB << "\t- [" << i << "] '" << objectNames[i] << "': " << shouldRecord;
        }
    }

    // sequential / non-sequential tracing
    RAYX::Sequential sequential = m_cliArgs.sequential ? RAYX::Sequential::Yes : RAYX::Sequential::No;

    // max events to record per ray path
    const auto maxEvents = m_cliArgs.maxEvents;

    // max batch size
    const auto maxBatchSize = m_cliArgs.batchSize;

    // in order to validate the events later, we always want to get the event types
    const auto attrRecordMaskTrace = attrRecordMask | RAYX::RayAttrMask::EventType;

    // do the trace
    auto rays = m_tracer->trace(beamline, sequential, objectRecordMask, attrRecordMaskTrace, maxEvents, maxBatchSize);

    if (m_cliArgs.sortByObjectId) {
        if (!(attrRecordMask & RAYX::RayAttrMask::ObjectId))
            RAYX_WARN << "Cannot sort by object_id, because object_id is not recorded. Please add object_id to the attribute record mask.";

        rays = rays.sortByObjectId();
    }

    // validate using recorded attribute: event type
    validateEvents(rays);

    // return to the user-specified attribute record mask
    rays.filterByAttrMask(attrRecordMask);

    return rays;
}

void TerminalApp::validateEvents(const RAYX::Rays& rays) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    const auto eventTypes =
        std::ranges::fold_left(rays.event_type.begin(), rays.event_type.end(), RAYX::EventTypeMask::None,
                               [](RAYX::EventTypeMask acc, const RAYX::EventType eventType) { return acc | RAYX::eventTypeToMask(eventType); });

    if (!!(eventTypes & RAYX::EventTypeMask::Uninitialized)) std::cout << "warning: one or more events in output are uninitialized" << std::endl;
    if (!!(eventTypes & RAYX::EventTypeMask::FatalError)) std::cout << "warning: fatal error detected for one or more events" << std::endl;
    if (!!(eventTypes & RAYX::EventTypeMask::BeyondHorizon))
        std::cout << "warning: one or more events in output have gone beyond the horizon while refracting" << std::endl;
    if (!!(eventTypes & RAYX::EventTypeMask::TooManyEvents))
        std::cout << "warning: capacity of events exceeded. could not record all events! consider increasing max events." << std::endl;
}

void TerminalApp::run() {
    RAYX_VERB << "TerminalApp running...";

    if (m_cliArgs.dump) {
        const auto filename = *m_cliArgs.dump;
        const auto filepath = fs::path(filename);
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

    if (m_cliArgs.verbose) {
        RAYX::setDebugVerbose(true);
    }

    if (m_cliArgs.defaultSeed) {
        RAYX::fixSeed(RAYX::FIXED_SEED);
    } else if (m_cliArgs.seed) {
        RAYX::fixSeed(*m_cliArgs.seed);
    } else {
        RAYX::randomSeed();
    }

    if (m_cliArgs.benchmark) {
        RAYX_VERB << "Starting in Benchmark Mode.\n";
        RAYX::BENCH_FLAG = true;
    }

    auto argToDeviceType = [&] {
        using DeviceType = RAYX::DeviceConfig::DeviceType;
        if (m_cliArgs.cpu == m_cliArgs.gpu) return DeviceType::All;
        return m_cliArgs.cpu ? DeviceType::Cpu : DeviceType::Gpu;
    };

    auto deviceType = argToDeviceType();

    if (m_cliArgs.listDevices) {
        RAYX::DeviceConfig(deviceType).dumpDevices();
        exit(0);
    }

    // Choose Hardware
    auto getDevice = [&] {
        if (m_cliArgs.deviceId) {
            return RAYX::DeviceConfig(deviceType).enableDeviceByIndex(*m_cliArgs.deviceId);
        } else {
            return RAYX::DeviceConfig(deviceType).enableBestDevice();
        }
    };
    m_tracer = std::make_unique<RAYX::Tracer>(getDevice());

    if (!m_cliArgs.inputPaths.size()) RAYX_EXIT << "Please provide an input RML file or directory. Use --help for more information";

    // trace and export
    auto rmlCounter = 0;
    for (const auto path : m_cliArgs.inputPaths) rmlCounter += tracePath(path);

    std::cout << "Done. Processed " << rmlCounter << " RML file(s)" << std::endl;
}

fs::path TerminalApp::exportRays(const fs::path& inputFilepath, const std::vector<std::string>& objectNames, const RAYX::Rays& rays,
                                 const RAYX::RayAttrMask attrRecordMask) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    if (rays.empty()) return {};

    fs::path outputFilepath;
    if (m_cliArgs.outputPath) {
        outputFilepath = *m_cliArgs.outputPath;
        if (fs::is_directory(outputFilepath)) outputFilepath /= inputFilepath.filename();
    } else {
        outputFilepath = inputFilepath;
    }
    outputFilepath.replace_extension(m_cliArgs.csv ? ".csv" : ".h5");

    // Error handling in case provided path does not exist
    auto parent = outputFilepath.parent_path();
    if (!parent.empty() && !fs::exists(parent)) {
        RAYX_EXIT << "Output directory '" << parent.string() << "' does not exist. Create it first or use a different output path.";
    }

    if (m_cliArgs.csv) {
        RAYX::writeCsv(outputFilepath, rays);
        const auto rays2 = RAYX::readCsv(outputFilepath);
        std::cout << (rays == rays2) << std::endl;
    } else {
#ifdef NO_H5
        RAYX_EXIT << "writeH5 called during NO_H5 (HDF5 disabled during build)";
#else
        if (m_cliArgs.append)
            RAYX::appendH5(outputFilepath, rays, attrRecordMask);
        else
            RAYX::writeH5(outputFilepath, objectNames, rays, attrRecordMask);
#endif
    }

    return outputFilepath;
}
