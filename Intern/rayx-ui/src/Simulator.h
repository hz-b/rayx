
#include <Tracer/CpuTracer.h>
#include <Tracer/Tracer.h>

class Simulator {
  public:
    Simulator();
    void runSimulation();
    void setSimulationParameters(std::filesystem::path RMLPath, RAYX::Beamline beamline, uint64_t m_max_batch_size = 1000000, int tracer = 0,
                                 bool sequential = false, unsigned int deviceIndex = 0);
    std::vector<std::string> getAvailableDevices();

  private:
    int m_maxEvents = 0;

    std::filesystem::path m_RMLPath;  ///< Path to the RML file
    RAYX::Beamline m_Beamline;        ///< Beamline
    RAYX::BundleHistory m_rays;       ///< Ray cache
    uint64_t m_max_batch_size = 1000000;
    std::unique_ptr<RAYX::Tracer> m_Tracer = std::make_unique<RAYX::CpuTracer>();
    RAYX::Sequential m_seq = RAYX::Sequential::No;
    std::vector<std::string> m_availableDevices;  ///< List of available devices
    unsigned int m_deviceIndex = 0;               ///< Index of the selected device
};