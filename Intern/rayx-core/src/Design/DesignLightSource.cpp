#include "DesignLightSource.h"

namespace RAYX {

std::vector<Ray>  DesignLightSource::getRays() const {
    return field<LightSourceTypeStr>().getRays(*this);
}

}
