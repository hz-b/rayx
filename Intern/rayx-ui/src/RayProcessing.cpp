#include "RayProcessing.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <unordered_set>

#include "Colors.h"

void displayFilterSlider(int* amountOfRays, int maxAmountOfRays) {
    ImGui::Text("Amount of Rays:");
    ImGui::SliderInt("Amount of Rays", amountOfRays, 1, maxAmountOfRays);
}

/**
 * This function processes the BundleHistory and determines the ray's path in the beamline.
 * Depending on the event type associated with the ray, the function produces visual lines that represent
 * ray segments, colored based on the event type.
 */
// Define the type of the filter function

std::vector<Line> getRays(const RAYX::BundleHistory& bundleHist, const std::vector<RAYX::OpticalElement>& elements, RayFilterFunction filterFunction,
                          int amountOfRays) {
    std::vector<Line> rays;

    // Apply the filter function to get the indices of the rays to be rendered
    std::vector<size_t> rayIndices = filterFunction(bundleHist, amountOfRays);
    auto maxRays = bundleHist.size();
    for (size_t i : rayIndices) {
        if (i >= maxRays) {
            RAYX_VERB << "Ray index out of bounds: " << i;
            continue;
        }
        auto& rayHist = bundleHist[i];
        glm::vec3 rayLastPos = {0.0f, 0.0f, 0.0f};
        for (const auto& event : rayHist) {
            // RAYX_LOG << "Start inner loop";
            if (event.m_eventType == ETYPE_JUST_HIT_ELEM || event.m_eventType == ETYPE_ABSORBED) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 worldPos = elements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);

                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z, 1.0f}, YELLOW};
                Vertex point = (event.m_eventType == ETYPE_JUST_HIT_ELEM) ? Vertex(worldPos, ORANGE) : Vertex(worldPos, RED);

                Line myline = {origin, point};
                rays.push_back(myline);
                rayLastPos = point.pos;
            } else if (event.m_eventType == ETYPE_FLY_OFF) {
                // Fly off events are in world coordinates
                // The origin here is the position of the event
                // The point is defined by the direction of the ray (default length)

                glm::vec4 eventPos = glm::vec4(event.m_position, 1.0f);
                glm::vec4 eventDir = glm::vec4(event.m_direction, 0.0f);
                glm::vec4 pointPos = eventPos + eventDir * 1000.0f;

                Vertex origin = {eventPos, GREY};
                Vertex point = {pointPos, GREY};

                rays.push_back(Line(origin, point));
            } else if (event.m_eventType == ETYPE_NOT_ENOUGH_BOUNCES) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 worldPos = elements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);

                const glm::vec4 white = {1.0f, 1.0f, 1.0f, 0.7f};
                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z, 1.0f}, white};
                Vertex point = Vertex(worldPos, white);

                rays.push_back(Line(origin, point));
                rayLastPos = point.pos;
            }
        }
    }

    return rays;
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

// Assume this is a function that performs k-means clustering
std::pair<std::vector<size_t>, std::vector<std::vector<float>>> kMeansClustering(const std::vector<std::vector<float>>& features, size_t k);

// Helper function to find the index of the most central ray in each cluster
std::vector<size_t> findMostCentralRays(const std::vector<std::vector<float>>& features, const std::vector<size_t>& clusterAssignments,
                                        const std::vector<std::vector<float>>& centroids, size_t k) {
    std::vector<size_t> centralRaysIndices(k, std::numeric_limits<size_t>::max());
    std::vector<float> minDistances(k, std::numeric_limits<float>::max());

    for (size_t i = 0; i < features.size(); ++i) {
        size_t clusterIdx = clusterAssignments[i];
        float distance = 0.0f;  // Calculate the distance between features[i] and centroids[clusterIdx]
        // Distance calculation, could be Euclidean or another metric
        for (size_t j = 0; j < features[i].size(); ++j) {
            distance += std::pow(features[i][j] - centroids[clusterIdx][j], 2);
        }
        distance = std::sqrt(distance);

        if (distance < minDistances[clusterIdx]) {
            minDistances[clusterIdx] = distance;
            centralRaysIndices[clusterIdx] = i;
        }
    }

    return centralRaysIndices;
}

// The kMeansFilter function implementation
std::vector<size_t> kMeansFilter(const RAYX::BundleHistory& bundleHist, size_t k) {
    std::vector<size_t> selectedRays;
    for (size_t j = 0; j < bundleHist[0].size(); ++j) {  // Assuming all rays have the same number of events
        auto features = extractFeatures(bundleHist, j);
        auto [clusterAssignments, centroids] = kMeansClustering(features, k);
        auto centralRaysIndices = findMostCentralRays(features, clusterAssignments, centroids, k);
        selectedRays.insert(selectedRays.end(), centralRaysIndices.begin(), centralRaysIndices.end());
    }

    // Remove duplicate indices if a ray is central in multiple events
    std::sort(selectedRays.begin(), selectedRays.end());
    auto last = std::unique(selectedRays.begin(), selectedRays.end());
    selectedRays.erase(last, selectedRays.end());

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

    bool changed = true;
    while (changed) {
        changed = false;

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
                // Consider reassigning clusters since the centroids have changed
                // Depending on the specific requirements, you may need to iterate again
                // or devise a more sophisticated method to handle empty clusters.
            } else {
                // Update centroids as normal
                for (size_t j = 0; j < centroids[i].size(); ++j) {
                    centroids[i][j] = sums[i][j] / counts[i];
                }
            }
        }
    }

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