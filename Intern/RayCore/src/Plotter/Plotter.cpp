#include <Plotter/Plotter.h>

#include <algorithm>

namespace RAYX {
bool comp(Ray const& lhs, Ray const& rhs) {
    return lhs.m_extraParam < rhs.m_extraParam;
}
/**
 * @brief Plot like RAY UI (Only supported)
 *
 */
void Plotter::plot() {
    // Look for Max for correct order
    std::vector<Ray> correctOrderRayList;
    auto max = std::max_element(m_RayList->begin(), m_RayList->end(), comp);
    auto max_param = max->m_extraParam;
    auto size = m_RayList->size();
    // Create new RayList with right order
    std::copy_if(m_RayList->begin(), m_RayList->end(),
                 std::back_inserter(correctOrderRayList),
                 [](Ray it) { return it->m_extraParam = max_param;});
    
    // Start plot
    matplotlibcpp::figure_size(10,10);
    matplotlibcpp::scatter((std::vector<double>)(correctOrderRayList.data()->m_position.x),(std::vector<double>)(correctOrderRayList.data()->m_position.y));
    matplotlibcpp::show();

}

}  // namespace RAYX
