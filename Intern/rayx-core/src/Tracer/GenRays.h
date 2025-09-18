#pragma once

#include <alpaka/alpaka.hpp>

#include "Beamline/Beamline.h"
#include "Beamline/StringConversion.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "Shader/LightSources/CircleSource.h"
#include "Shader/LightSources/DipoleSource.h"
#include "Shader/LightSources/EnergyDistributions/EnergyDistribution.h"
#include "Shader/LightSources/MatrixSource.h"
#include "Shader/LightSources/PixelSource.h"
#include "Shader/LightSources/PointSource.h"
#include "Shader/LightSources/SimpleUndulatorSource.h"
#include "Shader/RecordEvent.h"
#include "Util.h"

namespace RAYX {
namespace {

struct GenRaysKernel {
    template <typename Source>
    RAYX_FN_ACC Ray genRay(const Source& __restrict source, const int sourceId,
                           const std::optional<EnergyDistributionDataVariant>& __restrict energyDistribution, const int rayPathIndex,
                           Rand& __restrict rand) const {
        return source.genRay(rayPathIndex, sourceId, *energyDistribution, rand);
    }

    RAYX_FN_ACC Ray genRay(const DipoleSource& source, const int sourceId,
                           [[maybe_unused]] const std::optional<EnergyDistributionDataVariant>& __restrict energyDistribution, const int rayPathIndex,
                           Rand& __restrict rand) const {
        return source.genRay(rayPathIndex, sourceId, rand);
    }

    template <typename Acc, typename Source>
    RAYX_FN_ACC void operator()(const Acc& __restrict acc, RaysPtr rays, const int startRayIndexBatch, const Source source, const int sourceId,
                                const std::optional<EnergyDistributionDataVariant> energyDistribution, const int startRayIndex,
                                const int numRaysTotal, const double seed, const int n) const {
        const auto gid = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

        if (gid < n) {
            const auto rayPathIndex = startRayIndex + gid;
            auto rand               = Rand(rayPathIndex, numRaysTotal, seed);
            const auto ray          = genRay(source, sourceId, energyDistribution, rayPathIndex, rand);
            const auto dstRayIndex  = startRayIndexBatch + gid;
            storeRay(dstRayIndex, rays, ray);
        }
    }
};

}  // unnamed namespace

template <typename Acc>
struct GenRays {
    /// holds configuration state of sources
    struct SourceConfig {
        int numRaysTotal;
        int numRaysBatchAtMost;
        int numBatches;
    };

    /// holds configuration state of one batch
    struct BatchConfig {
        int numRaysBatch;
        RaysBuf<Acc> d_rays;
    };

    template <typename Queue>
    SourceConfig update(Queue q, const Group& beamline, const int maxBatchSize) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto platformHost = alpaka::PlatformCpu{};
        const auto devHost      = alpaka::getDevByIdx(platformHost, 0);

        m_startRayIndex = 0;

        const auto compileSource = [](const DesignSource& designSource) -> std::optional<SourceVariant> {
            switch (designSource.getType()) {
                case ElementType::PointSource:
                    return PointSource(designSource);
                case ElementType::MatrixSource:
                    return MatrixSource(designSource);
                case ElementType::DipoleSource:
                    return DipoleSource(designSource);
                case ElementType::PixelSource:
                    return PixelSource(designSource);
                case ElementType::CircleSource:
                    return CircleSource(designSource);
                case ElementType::SimpleUndulatorSource:
                    return SimpleUndulatorSource(designSource);
                default:
                    throw std::runtime_error(std::format("Unimplemented source type ({}) with name: \"{}\"",
                                                         ElementTypeToString.at(designSource.getType()), designSource.getName()));
                    return std::nullopt;
            }
        };

        auto energyDistributionListIndex     = 0;
        const auto compileEnergyDistribution = [&](const DesignSource& designSource) -> std::optional<EnergyDistributionDataVariant> {
            // special case: DipoleSource has no energy distribution
            if (designSource.getType() == ElementType::DipoleSource) return std::nullopt;

            return std::visit(
                [&]<typename T>(const T& value) -> std::optional<EnergyDistributionDataVariant> {
                    if constexpr (std::is_same_v<T, HardEdge>) { return value; }
                    if constexpr (std::is_same_v<T, SoftEdge>) { return value; }
                    if constexpr (std::is_same_v<T, SeparateEnergies>) { return value; }
                    if constexpr (std::is_same_v<T, DatFile>) {
                        assert(value.m_Lines.size() > 0);
                        assert(d_energyDistributionListWeights.size() == d_energyDistributionListEnergies.size());

                        // get the data
                        std::vector<double> weights;
                        std::vector<double> energies;
                        for (const auto entry : value.m_Lines) {
                            weights.push_back(entry.m_weight);
                            energies.push_back(entry.m_energy);
                        }
                        std::vector<double> prefixWeights(weights.size());
                        std::exclusive_scan(weights.begin(), weights.end(), prefixWeights.begin(), 0.0);
                        const auto weightSum = weights.back() + prefixWeights.back();

                        // alloc device buffers and transfer data
                        const auto index = energyDistributionListIndex++;
                        const auto size  = static_cast<int>(value.m_Lines.size());
                        if (static_cast<int>(d_energyDistributionListWeights.size()) <= index) {
                            d_energyDistributionListWeights.emplace_back();
                            d_energyDistributionListEnergies.emplace_back();
                        }
                        allocBuf(q, d_energyDistributionListWeights[index], size);
                        allocBuf(q, d_energyDistributionListEnergies[index], size);
                        alpaka::memcpy(q, *d_energyDistributionListWeights[index], alpaka::createView(devHost, prefixWeights, size));
                        alpaka::memcpy(q, *d_energyDistributionListEnergies[index], alpaka::createView(devHost, energies, size));

                        return EnergyDistributionList{
                            .prefixWeights = alpaka::getPtrNative(*d_energyDistributionListWeights[index]),
                            .weightSum     = weightSum,
                            .energies      = alpaka::getPtrNative(*d_energyDistributionListWeights[index]),
                            .size          = size,
                            .start         = value.m_start,
                            .end           = value.m_end,
                            .step          = value.m_step,
                            .continous     = value.m_continuous,
                        };
                    }

                    RAYX_EXIT << "error: unimplemented energy distribution type";
                    return std::nullopt;
                },
                designSource.getEnergyDistribution());
        };

        m_numRaysTotal      = 0;
        auto numRaysSources = std::vector<int>();
        auto sourceId       = static_cast<int>(0);

        for (const auto* designSource : beamline.getSources()) {
            const auto source             = *compileSource(*designSource);
            const auto energyDistribution = compileEnergyDistribution(*designSource);
            const auto numRaysSource      = static_cast<int>(designSource->getNumberOfRays());
            m_numRaysTotal += numRaysSource;

            m_sourceStates.push_back(SourceState{
                .source                 = source,
                .sourceId               = sourceId,
                .energyDistribution     = energyDistribution,
                .numRaysSourceRemaining = numRaysSource,
                .name                   = designSource->getName(),
            });

            ++sourceId;
        }

        m_numRaysBatchAtMost = std::min(m_numRaysTotal, maxBatchSize);

#define X(type, name, flag) \
    allocBuf(q, d_rays.name, m_numRaysBatchAtMost);

        RAYX_X_MACRO_RAY_ATTR
#undef X

        const auto numBatches = ceilIntDivision(m_numRaysTotal, m_numRaysBatchAtMost);

        m_seed = randomDouble();

        return {
            .numRaysTotal       = m_numRaysTotal,
            .numRaysBatchAtMost = m_numRaysBatchAtMost,
            .numBatches         = numBatches,
        };
    }

    template <typename DevAcc, typename Queue>
    BatchConfig genRaysBatch(DevAcc devAcc, Queue q, const int batchIndex) {
        RAYX_PROFILE_FUNCTION_STDOUT();

        const auto batchStartRayIndex    = batchIndex * m_numRaysBatchAtMost;
        const auto numRaysTotalRemaining = m_numRaysTotal - batchStartRayIndex;
        const auto numRaysBatch          = std::min(numRaysTotalRemaining, m_numRaysBatchAtMost);
        auto numRaysBatchRemaining       = numRaysBatch;

        for (auto& sourceState : m_sourceStates) {
            const auto numRaysBatchSource = std::min(numRaysBatchRemaining, sourceState.numRaysSourceRemaining);

            if (numRaysBatchSource) {
                const auto startRayIndexBatch = numRaysBatch - numRaysBatchRemaining;
                numRaysBatchRemaining -= numRaysBatchSource;
                sourceState.numRaysSourceRemaining -= numRaysBatchSource;

                std::visit(
                    [&]<typename Source>(const Source& source) {
                        RAYX_VERB << "execute GenRaysKernel<Source> with Source = '" << sourceState.name << "'";
                        execWithValidWorkDiv<Acc>(devAcc, q, numRaysBatchSource, BlockSizeConstraint::None{}, GenRaysKernel{},
                                                  raysBufToRaysPtr(d_rays), startRayIndexBatch, source, sourceState.sourceId,
                                                  sourceState.energyDistribution, m_startRayIndex, m_numRaysTotal, m_seed, numRaysBatchSource);
                    },
                    sourceState.source);

                m_startRayIndex += numRaysBatchSource;

                assert(0 <= numRaysBatchRemaining);
                if (numRaysBatchRemaining == 0) break;
            }
        }

        return BatchConfig{
            .numRaysBatch       = numRaysBatch,
            .d_rays             = d_rays,
        };
    }

  private:
    // resources per batch. constant per batch
    /// generated rays
    RaysBuf<Acc> d_rays;

    // buffers for EnergyDistributionList (DatFile)
    std::vector<OptBuf<Acc, double>> d_energyDistributionListWeights;
    std::vector<OptBuf<Acc, double>> d_energyDistributionListEnergies;

    template <typename T>
    struct SizedArray {
        T* __restrict data;
        int size;
    };
    std::vector<OptBuf<Acc, SizedArray<double>>> d_energyDistributionDatFiles;

    using SourceVariant = std::variant<CircleSource, DipoleSource, MatrixSource, PixelSource, PointSource, SimpleUndulatorSource>;

    struct SourceState {
        const SourceVariant source;
        const int sourceId;
        const std::optional<EnergyDistributionDataVariant> energyDistribution;
        int numRaysSourceRemaining;
        std::string name;
    };

    std::vector<SourceState> m_sourceStates;
    int m_startRayIndex;
    int m_numRaysTotal;
    int m_numRaysBatchAtMost;
    double m_seed;
};

}  // namespace RAYX
