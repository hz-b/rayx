#include "CommandParser.h"

#include <CLI/CLI.hpp>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "TerminalAppConfig.h"
#include "Tracer/Tracer.h"

namespace {

void getVersion() {
    std::cout << R"(

  ╔═══╗╔═══╗╔╗  ╔╗╔═╗╔═╗
  ║╔═╗║║╔═╗║║╚╗╔╝║╚╗╚╝╔╝
  ║╚═╝║║║ ║║╚╗╚╝╔╝ ╚╗╔╝
  ║╔╗╔╝║╚═╝║ ╚╗╔╝  ╔╝╚╗
  ║║║╚╗║╔═╗║  ║║  ╔╝╔╗╚╗
  ╚╝╚═╝╚╝ ╚╝  ╚╝  ╚═╝╚═╝ HZB 2023.
  )" << std::endl;
    ;
    std::cout << "\n\t rayx terminal application " << TERMINALAPP_VERSION_MAJOR << "." << TERMINALAPP_VERSION_MINOR << "."
              << TERMINALAPP_VERSION_PATCH << "\n \t GIT: " << GIT_REVISION << "\n \t BUILD: " << BUILD_TIMESTAMP << std::endl;
};

}  // unnamed namespace

CliArgs parseCliArgs(const int argc, char const* const* const argv) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    CliArgs args;
    CLI::App app{"Terminal Application for RAYX"};

    // add positional argument to input paths
    std::vector<std::string> inputPaths;
    app.add_option("inputs", inputPaths, "Input RML files or directories (recursive search for RML files). Same as --input");

    const auto groupPrograms = "Alternative programs, no tracing";

    // the order here, determines the order in the --help print

    // other programs than tracing
    app.add_flag("-v,--version", args.version, "Show version information")->group(groupPrograms);
    app.add_option("-D,--dump", args.dump, "Dump the meta data of a file (RML or H5)")->group(groupPrograms);

    // tracing related options
    app.add_option("-i,--input", args.inputPaths, "Input RML files or directories (recursive search for RML files)");
    app.add_option("-o,--output", args.outputPath,
                   "Output filepath. Can only be used if a single input is provided, that directs to an RML file. Default: put the output file "
                   "next to the RML");
    app.add_flag("-a,--append", args.append, "Append to existing output file. Default: overwrite existing output file");
    app.add_flag("-S,--sequential", args.sequential, "Trace sequentially");
    app.add_option("-s,--seed", args.seed, "Specify a seed to be used for tracing");
    app.add_flag("-f,--default-seed", args.defaultSeed, std::format("Use default seed for tracing: {}", RAYX::FIXED_SEED));
    app.add_flag("-x,--cpu", args.cpu,
                 "Enable CPU devices. Can be combined with --gpu. Affects --list-devices and --device-index. Default behaviour if neither --cpu and "
                 "--gpu are provided: Both will be enabled");
    app.add_flag("-X,--gpu", args.gpu, "Same as --cpu, but for GPU instead of CPU");
    app.add_flag("-l,--list-devices", args.listDevices, "List devices available for tracing. Affected by --cpu and --gpu")->group(groupPrograms);
    app.add_option("-d,--device-index", args.deviceId,
                   "Pick device via device index. Available devices are determined by --cpu and --gpu. Default: the best device will be picked "
                   "automatically. Use --list-devices to see the available devices");
    app.add_flag("-c,--csv", args.csv, "Output stored as csv instead of H5 file");
    app.add_flag("-V,--verbose", args.verbose, "Dump more information");
    app.add_option("-m,--maxevents", args.maxEvents,
                   "Maximum number of events per ray. Default: A multiple of the number of objects to record events for");
    app.add_option("-b,--batch-size", args.batchSize, std::format("Batch size for tracing. Default: {}", RAYX::DEFAULT_BATCH_SIZE));
    app.add_option("-n,--number-of-rays", args.numberOfRays, "Override the number of rays for all sources");
    app.add_flag("-B,--benchmark", args.benchmark, "Dump benchmark durations");
    app.add_flag("-O,--sort-by-object-id", args.sortByObjectId, "Sort rays by object_id before writing to output file");
    app.add_option("-R,--record-indices", args.objectRecordIndices,
                   "Record events only for specific sources / elements. Use --dump to list the objects of a beamline");

    auto formatAttrNames = RAYX::getRayAttrNames();
    auto formatAttrNamesStr = std::string();
    for (const auto attrName : formatAttrNames) formatAttrNamesStr += "\n\t" + attrName;
    app.add_option(
        "-A,--attributes", args.attrRecordMask,
        std::format("Record only specific Ray attributes to the output H5 file. Default: record all attributes. Attributes: {}", formatAttrNamesStr));

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        auto exit_code = app.exit(e);
        std::exit(exit_code);
        return {};
    }

    args.inputPaths.insert(args.inputPaths.end(), inputPaths.begin(), inputPaths.end());

    if (args.attrRecordMask.empty()) args.attrRecordMask = formatAttrNames;

    if (args.version) {
        getVersion();
        std::exit(0);
    }

    if (args.defaultSeed && args.seed) RAYX_EXIT << "Please do not provide '--default-seed' and '--seed' simultaneously'";

    const bool isMoreThanOnePath = args.inputPaths.size() > 1;
    const bool isFirstPathDirectory = args.inputPaths.size() == 1 && std::filesystem::is_directory(args.inputPaths[0]);
    if (args.outputPath && (isMoreThanOnePath || isFirstPathDirectory)) {
        if (std::filesystem::is_directory(*args.outputPath)) {
            std::cout << "warning: specifying multiple input files and an output directory can lead to name collisions between output filenames"
                      << std::endl;

            if (!args.objectRecordIndices.empty())
                std::cout << "warning: you specified --record-indices, but different input files may have different beamline objects, so the "
                             "recorded objects may differ between files"
                          << std::endl;
        } else {
            RAYX_EXIT << "error: the output path must be a directory, when multiple input files or an input directory is specified";
        }
    }

    if (args.append && args.csv) RAYX_EXIT << "error: appending to existing output files is not supported for csv output";

    return args;
}
