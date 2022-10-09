#pragma once
#include <string>
#include <vector>

#include "RayCore.h"
#include "matplotlibcpp.h"
namespace RAYX {
class Plotter {
  public:
    Plotter() = default;
    Plotter(int plotType, std::string plotName, std::vector<Ray>* RayList);
    ~Plotter() = default;
    void plot();

  private:
    int m_plotType = 0;
    std::string m_plotName = "";
    std::vector<Ray>* m_RayList = nullptr;
};
}  // namespace RAYX