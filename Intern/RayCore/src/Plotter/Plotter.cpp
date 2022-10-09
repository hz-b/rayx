#include <Plotter/Plotter.h>

#include <algorithm>
namespace RAYX {
bool comp(Ray const& lhs, Ray const& rhs) {
    return lhs.m_extraParam < rhs.m_extraParam;
}
bool abs_comp(double const& lhs, double const& rhs) {
    return abs(lhs) < abs(rhs);
}
inline bool intclose(double x, double y) {
    return abs(x - y) < std::numeric_limits<double>::epsilon();
}

Plotter::Plotter(int plotType, std::string plotName, std::vector<Ray>* RayList)
    : m_plotType(plotType), m_plotName(plotName), m_RayList(RayList) {}

/**
 * @brief Plot like RAY UI (Only supported)
 *
 */
void Plotter::plot() {
    RAYX_LOG << "Plotting...";
    // Look for Max for correct order
    std::vector<Ray> correctOrderRayList;
    std::vector<double> Xpos, Ypos;
    correctOrderRayList.reserve(m_RayList->size());
    Xpos.reserve(m_RayList->size());
    Ypos.reserve(m_RayList->size());
    auto max = std::max_element(m_RayList->begin(), m_RayList->end(), comp);
    auto max_param = max->m_extraParam;

    // Create new RayList with right order
    for (auto r : *(m_RayList)) {
        if (intclose(r.m_extraParam, max_param)) {
            correctOrderRayList.push_back(r);
            Xpos.push_back(r.m_position.x);
            Ypos.push_back(r.m_position.y);
        }
    }

    // Start plot
    // auto f = matplotlibcpp::figure(1);
    matplotlibcpp::figure_size(1000, 1000);
    matplotlibcpp::suptitle(m_plotName);

    matplotlibcpp::subplot2grid(4, 4, 0, 0, 1, 3);
    double binwidth = 0.00125;
    double xymax =
        std::max(*(std::max_element(Xpos.begin(), Xpos.end(), abs_comp)),
                 *(std::max_element(Ypos.begin(), Ypos.end(), abs_comp)));
    double lim = ((int)(xymax / binwidth) + 1) * binwidth;
    //matplotlibcpp::hist(Xpos, lim, "b", 0.65);
    matplotlibcpp::scatter(Xpos, Ypos, 0.5, {{"color", "#62c300"}});
    // matplotlibcpp::subplots_adjust()

    matplotlibcpp::subplot2grid(4, 4, 1, 0, 3, 3);

    matplotlibcpp::scatter(Xpos, Ypos, 0.5, {{"color", "#62c300"}});
    matplotlibcpp::show();
}

}  // namespace RAYX
