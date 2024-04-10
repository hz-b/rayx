#include "RayProcessing.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <cmath>  // for std::pow and std::log
#include <numeric>
#include <unordered_set>

#include "Application.h"
#include "Colors.h"
#include "Debug/Instrumentor.h"

void displayFilterSlider(size_t& amountOfRays, size_t maxAmountOfRays, bool& displayRays, bool& renderAllRays) {
    // Checkbox for displaying rays
    // Slider should be greyed out if "Display Rays" is unchecked
    ImGui::Checkbox("Display Rays", &displayRays);
    if (!displayRays || renderAllRays) {
        ImGui::BeginDisabled();  // Grey out the slider
    }

    amountOfRays = std::min(amountOfRays, maxAmountOfRays);
    ImGui::Text("Maximum amount of Rays per optical element:");

    // Define the range for the slider
    float minLogValue = 0.0f;
    float maxLogValue = std::log(static_cast<float>(std::min<size_t>(maxAmountOfRays, MAX_RAYS)));

    // Convert the current amount of rays to logarithmic scale for the slider position
    float logValue = std::log(static_cast<float>(amountOfRays));

    // Create a slider that operates on the logarithmic scale
    if (ImGui::SliderFloat("##hidden", &logValue, minLogValue, maxLogValue, "")) {
        // Convert the logarithmic value back to the actual number of rays
        amountOfRays = static_cast<int>(std::exp(logValue));
    }
    if (displayRays && renderAllRays) {
        ImGui::EndDisabled();
    }

    // Display the actual number of rays next to the slider
    ImGui::SameLine();
    ImGui::Text("%zu", amountOfRays);
    ImGui::Checkbox("Render all rays", &renderAllRays);
    if (!displayRays) {
        ImGui::EndDisabled();  // End grey out
    }
}

size_t getMaxEvents(const RAYX::BundleHistory& bundleHist) {
    size_t maxEvents = 0;
    for (const auto& ray : bundleHist) {
        maxEvents = std::max(maxEvents, ray.size());
    }
    return maxEvents;
}

/**
 * This function processes the BundleHistory and determines the ray's path in the beamline.
 * Depending on the event type associated with the ray, the function produces visual lines that represent
 * ray segments, colored based on the event type.
 */
// Define the type of the filter function

std::vector<Line> getRays(const RAYX::BundleHistory& rayCache, const RAYX::Beamline& beamline, RayFilterFunction filterFunction,
                          uint32_t amountOfRays, int startEventID) {
    std::vector<Line> rays;

    // Apply the filter function to get the indices of the rays to be rendered
    amountOfRays = (uint32_t)std::min(amountOfRays, uint32_t(rayCache.size()));
    std::vector<size_t> rayIndices = filterFunction(rayCache, amountOfRays);
    auto maxRayIndex = rayCache.size();
    for (size_t i : rayIndices) {
        if (i >= maxRayIndex) {
            RAYX_VERB << "Ray index out of bounds: " << i;
            continue;
        }
        auto& rayHist = rayCache[i];

        if (beamline.m_LightSources.size() <= rayHist[0].m_sourceID) {
            RAYX_ERR << "Trying to access out-of-bounds index with source ID: " << rayHist[0].m_sourceID;
        }
        glm::vec4 rayLastPos = (glm::vec4)beamline.m_LightSources[(size_t)rayHist[0].m_sourceID]->getPosition();

        bool isFirstEvent = true;

        for (const auto& event : rayHist) {
            if (event.m_lastElement >= beamline.m_OpticalElements.size()) {
                RAYX_ERR << "Trying to access out-of-bounds index with element ID: " << event.m_lastElement;
            }
            glm::vec4 worldPos = beamline.m_OpticalElements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);
            const glm::vec4 WHITE = {1.0f, 1.0f, 1.0f, 0.7f};

            glm::vec4 originColor = (event.m_eventType == ETYPE_JUST_HIT_ELEM) ? YELLOW : WHITE;
            glm::vec4 pointColor = (event.m_eventType == ETYPE_JUST_HIT_ELEM) ? ORANGE : (event.m_eventType == ETYPE_ABSORBED) ? RED : WHITE;

            if (!(isFirstEvent && startEventID > 0)) {
                // Only execute if not the first event with startEventID > 0
                ColorVertex origin = {rayLastPos, originColor};
                ColorVertex point = {worldPos, pointColor};

                rays.push_back(Line(origin, point));
            }

            rayLastPos = worldPos;  // Update rayLastPos in every case for the next iteration
            isFirstEvent = false;   // Update the flag after the first iteration
        }
    }

    return rays;
}
void sortRaysByElement(const RAYX::BundleHistory& rays, std::vector<std::vector<RAYX::Ray>>& sortedRays, size_t numElements) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    sortedRays.resize(numElements);

    // Iterate over all rays in the bundle history
    for (const auto& rayBundle : rays) {
        for (const auto& ray : rayBundle) {
            if (ray.m_lastElement >= numElements) {
                continue;
            }
            sortedRays[ray.m_lastElement].push_back(ray);
        }
    }
}

std::vector<std::vector<float>> extractFeatures(const RAYX::BundleHistory& bundleHist, size_t eventIndex) {
    std::vector<std::vector<float>> features;
    for (const auto& rayHist : bundleHist) {
        if (eventIndex < rayHist.size()) {
            // Extract features from the ray event, specifically position
            const auto& event = rayHist[eventIndex];
            features.push_back({float(event.m_position.x), float(event.m_position.y), float(event.m_position.z)});
        }
    }
    return features;
}

std::pair<std::vector<size_t>, std::vector<std::vector<float>>> kMeansClustering(const std::vector<std::vector<float>>& features, size_t k);

// Helper function to find the index of the most central ray in each cluster
std::vector<size_t> findMostCentralRays(const std::vector<std::vector<float>>& features, const std::vector<size_t>& clusterAssignments,
                                        const std::vector<std::vector<float>>& centroids, size_t k) {
    std::vector<size_t> centralRaysIndices(k, std::numeric_limits<size_t>::max());
    std::vector<float> minDistances(k, std::numeric_limits<float>::max());

    for (size_t i = 0; i < features.size(); ++i) {
        size_t clusterIdx = clusterAssignments[i];
        float distance = 0.0f;  // Calculate the distance between features[i] and centroids[clusterIdx]
        for (size_t j = 0; j < features[i].size(); ++j) {
            distance += (float)std::pow(features[i][j] - centroids[clusterIdx][j], 2);
        }
        distance = std::sqrt(distance);

        if (distance < minDistances[clusterIdx]) {
            minDistances[clusterIdx] = distance;
            centralRaysIndices[clusterIdx] = i;
        }
    }

    return centralRaysIndices;
}

std::vector<size_t> kMeansFilter(const RAYX::BundleHistory& rayCache, size_t k) {
    const size_t m = getMaxEvents(rayCache);
    std::vector<size_t> selectedRays;
    std::unordered_map<size_t, size_t> indexMap;  // Map filtered indices to original indices

    for (size_t j = 0; j < m; ++j) {
        RAYX::BundleHistory filteredRays;
        indexMap.clear();

        for (size_t i = 0; i < rayCache.size(); ++i) {
            if (rayCache[i].size() > j) {
                indexMap[filteredRays.size()] = i;  // Map the new index to the original index
                filteredRays.push_back(rayCache[i]);
            }
        }

        if (!filteredRays.empty()) {
            auto features = extractFeatures(filteredRays, j);
            k = std::min(k, features.size());
            auto [clusterAssignments, centroids] = kMeansClustering(features, k);
            auto centralRaysIndices = findMostCentralRays(features, clusterAssignments, centroids, k);
            for (auto& idx : centralRaysIndices) {
                selectedRays.push_back(indexMap[idx]);  // Map back to original index
            }
        }
    }

    // Remove duplicate indices if a ray is central in multiple events
    std::sort(selectedRays.begin(), selectedRays.end());
    auto last = std::unique(selectedRays.begin(), selectedRays.end());
    selectedRays.erase(last, selectedRays.end());

    return selectedRays;
}

std::vector<size_t> noFilter(const RAYX::BundleHistory& bundleHist, [[maybe_unused]] size_t k) {
    std::vector<size_t> selectedRays;
    for (size_t i = 0; i < bundleHist.size(); ++i) {
        selectedRays.push_back(i);
    }

    return selectedRays;
}

// Function to calculate Euclidean distance between two feature vectors
float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float distance = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        distance += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return std::sqrt(distance);
}

// Function to perform the k-means clustering
std::pair<std::vector<size_t>, std::vector<std::vector<float>>> kMeansClustering(const std::vector<std::vector<float>>& features, size_t k) {
    // Initialize centroids
    std::vector<std::vector<float>> centroids(k, std::vector<float>(features[0].size(), 0));
    // Call to the new initializeCentroids function
    initializeCentroids(centroids, features, k);
    std::vector<size_t> clusterAssignments(features.size(), 0);
    std::random_device rd;
    std::mt19937 rng(rd());

    // Initialize centroids to random features
    std::uniform_int_distribution<size_t> uni(0, features.size() - 1);
    for (auto& centroid : centroids) {
        centroid = features[uni(rng)];
    }

    int maxIterations = 25;
    int iteration = 0;
    bool changed = true;
    while (changed && iteration < maxIterations) {
        changed = false;
        iteration++;

        // Assign clusters
        for (size_t i = 0; i < features.size(); ++i) {
            float minDistance = std::numeric_limits<float>::max();
            size_t clusterIndex = 0;
            for (size_t j = 0; j < k; ++j) {
                float distance = euclideanDistance(features[i], centroids[j]);
                if (distance < minDistance) {
                    minDistance = distance;
                    clusterIndex = j;
                }
            }
            if (clusterAssignments[i] != clusterIndex) {
                clusterAssignments[i] = clusterIndex;
                changed = true;
            }
        }

        // Update centroids
        std::vector<size_t> counts(k, 0);
        std::vector<std::vector<float>> sums(k, std::vector<float>(features[0].size(), 0));
        for (size_t i = 0; i < features.size(); ++i) {
            size_t clusterIndex = clusterAssignments[i];
            counts[clusterIndex]++;
            for (size_t j = 0; j < features[i].size(); ++j) {
                sums[clusterIndex][j] += features[i][j];
            }
        }

        // After clustering, ensure that each centroid has at least one point
        for (size_t i = 0; i < k; ++i) {
            if (counts[i] == 0) {
                // Reinitialize centroid to a random feature to handle empty clusters
                centroids[i] = features[uni(rng)];
            } else {
                // Update centroids as normal
                for (size_t j = 0; j < centroids[i].size(); ++j) {
                    centroids[i][j] = sums[i][j] / counts[i];
                }
            }
        }
    }
    RAYX_VERB << "K-means clustering converged after " << iteration << " iterations.";
    return {clusterAssignments, centroids};
}

void initializeCentroids(std::vector<std::vector<float>>& centroids, const std::vector<std::vector<float>>& features, size_t k) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> uni(0, features.size() - 1);
    std::unordered_set<size_t> selectedIndices;

    for (size_t i = 0; i < k; ++i) {
        size_t index = uni(rng);
        // Ensure unique indices for centroid initialization
        while (selectedIndices.find(index) != selectedIndices.end()) {
            index = uni(rng);
        }
        centroids[i] = features[index];
        selectedIndices.insert(index);
    }
}
