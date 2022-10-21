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
    void plot(int plotType, const std::string& plotName,
              const std::vector<Ray>& RayList);

  private:
    static int getBinAmount(std::vector<double>& Pos);
    static void plotLikeRAYUI(const std::vector<Ray>& RayList, const std::string& plotName);
    static void plotforEach(const std::vector<Ray>& RayList, const std::string& plotName);
    enum plotTypes { LikeRAYUI, ForEach, Eachsubplot };
};
}  // namespace RAYX