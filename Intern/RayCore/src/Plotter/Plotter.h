#pragma once
#include <string>
#include <vector>

#include "RayCore.h"
#include "matplotlibcpp.h"
namespace RAYX {
class Plotter {
  public:
    Plotter() = default;
    ~Plotter() = default;
    void plot(int plotType, std::string plotName,
              const std::vector<Ray>& RayList);

  private:
    int getBinAmount(std::vector<double>& Pos);
    enum plotTypes {LikeRAYUI, ForEach, Eachsubplot };
};
}  // namespace RAYX