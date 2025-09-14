#include "CommandParser.h"

#include <CLI/CLI.hpp>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "TerminalAppConfig.h"
#include "Tracer/Tracer.h"

namespace {

void getVersion() {
    RAYX_LOG << R"(

  ╔═══╗╔═══╗╔╗  ╔╗╔═╗╔═╗
  ║╔═╗║║╔═╗║║╚╗╔╝║╚╗╚╝╔╝
  ║╚═╝║║║ ║║╚╗╚╝╔╝ ╚╗╔╝
  ║╔╗╔╝║╚═╝║ ╚╗╔╝  ╔╝╚╗
  ║║║╚╗║╔═╗║  ║║  ╔╝╔╗╚╗
  ╚╝╚═╝╚╝ ╚╝  ╚╝  ╚═╝╚═╝ HZB 2023.
  )";
    RAYX_LOG << "\n\t rayx terminal application " << TERMINALAPP_VERSION_MAJOR << "." << TERMINALAPP_VERSION_MINOR << "." << TERMINALAPP_VERSION_PATCH
             << "\n \t GIT: " << GIT_REVISION << "\n \t BUILD: " << BUILD_TIMESTAMP;
};

}  // unnamed namespace

CliArgs parseCliArgs(const int argc, char const* const* const argv) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    CliArgs args;
    CLI::App app{"Terminal Application for RAYX"};

    // add positional argument to input paths
    std::vector<std::string> inputPaths;
    app.add_option("inputs", inputPaths, "Same as --input");

    const auto groupPrograms = "Alternative programs, no tracing";

    // the order here, determines the order in the --help print

    // tracing related options
    app.add_option("-i,--input", args.inputPaths, "Input RML files or directories (traversed recursively, looking for RML files)");
    app.add_option("-o,--output", args.outputPath,
                   "Output filepath. Can only be used if a single input is provided, that directs to an RML file. Default: put the output file "
                   "next to the RML");
    app.add_flag("-S,--sequential", args.sequential, "Trace sequentially");
    app.add_option("-s,--seed", args.seed, "Specify a seed to be used for tracing");
    app.add_flag("-f,--default-seed", args.defaultSeed, std::format("Use default seed for tracing: {}", RAYX::FIXED_SEED));
    app.add_flag("-x,--cpu", args.cpu,
                 "Enable CPU devices. Can be combined with --gpu. Affects --list-devices and --device-index. Default behaviour if neither --cpu and "
                 "--gpu are provided: Both will be enabled");
    app.add_flag("-X,--gpu", args.gpu, "Same as --cpu, but for GPU instead of CPU");
    app.add_option("-d,--device-index", args.deviceId,
                   "Pick device via device index. Available devices are determined by --cpu and --gpu. Default: the best device will be picked "
                   "automatically. Use --list-devices to see the available devices");
    app.add_flag("-c,--csv", args.csv, "Output stored as csv instead of hdf5 file");
    app.add_flag("-V,--verbose", args.verbose, "Dump more information");
    app.add_option("-m,--maxevents", args.maxEvents,
                   "Maximum number of events per ray. Default: A multiple of the number of objects to record events for");
    app.add_option("-b,--batch-size", args.batchSize, std::format("Batch size for tracing. Default: {}", RAYX::DEFAULT_BATCH_SIZE));
    app.add_option("-n,--number-of-rays", args.numberOfRays, "Override the number of rays for all sources");
    app.add_flag("-B,--benchmark", args.benchmark, "Dump benchmark durations");
    app.add_option("-R,--record-indices", args.recordIndices,
                   "Record events only for specific sources / elements. Use --dump to list the objects of a beamline");

    auto formatAttrNames    = RAYX::getRayAttrNames();
    auto formatAttrNamesStr = std::string();
    for (const auto attrName : formatAttrNames) formatAttrNamesStr += "\n\t" + attrName;
    app.add_option(
        "-F,--format", args.format,
        std::format("Record only specific Ray attributes to the output H5 file. Default: record all attributes. Attributes: {}", formatAttrNamesStr));

    // other programs than tracing
    app.add_flag("-v,--version", args.version, "Show version information")->group(groupPrograms);
    app.add_flag("-l,--list-devices", args.listDevices, "List devices available for tracing. Affected by --cpu and --gpu")->group(groupPrograms);
    app.add_option("-D,--dump", args.dump, "Dump the meta data of a file (h5 or rml)")->group(groupPrograms);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        auto exit_code = app.exit(e);
        std::exit(exit_code);
        return {};
    }

    args.inputPaths.insert(args.inputPaths.end(), inputPaths.begin(), inputPaths.end());

    if (args.format.empty()) args.format = formatAttrNames;

    if (args.version) {
        getVersion();
        std::exit(0);
    }

    if (args.defaultSeed && args.seed) RAYX_EXIT << "Please do not provide '--default-seed' and '--seed' simultaneously'";

    const bool isMoreThanOnePath    = args.inputPaths.size() > 1;
    const bool isFirstPathDirectory = args.inputPaths.size() == 1 && std::filesystem::is_directory(args.inputPaths[0]);
    if (args.outputPath && (isMoreThanOnePath || isFirstPathDirectory)) {
        if (std::filesystem::is_directory(*args.outputPath)) {
            std::cout << "warning: you specified an output directory and potentially multiple input files. input files with the same name can lead "
                         "to name collisions when exporting to the output directory"
                      << std::endl;
        } else {
            RAYX_EXIT << "error: the output path must be a directory, when multiple input files or an input directory is specified";
        }
    }

    return args;
}
