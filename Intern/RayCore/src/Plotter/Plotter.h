#pragma once
#include <string>
#include <vector>

#include "Core.h"
#include "RayCore.h"
#include "matplotlibcpp.h"
namespace RAYX {
class RAYX_API Plotter {
  public:
    Plotter() = default;
    ~Plotter() = default;
    void plot(int plotType, std::string plotName,
              const std::vector<Ray>& RayList);

  private:
    int getBinAmount(std::vector<double>& Pos);
    void plotLikeRAYUI(const std::vector<Ray>& RayList, std::string plotName);
    void plotforEach(const std::vector<Ray>& RayList, std::string plotName);
    enum plotTypes { LikeRAYUI, ForEach, Eachsubplot };
};
}  // namespace RAYX