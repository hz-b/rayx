#pragma once

#include "Angle.h"
#include "Distribution.h"
#include "PhotonEnergy.h"

using ScalarRangeSampler       = RangeSampler<double>;
using AngularRangeSampler      = RangeSampler<double>;
using PhotonEnergyRangeSampler = RangeSampler<double>;

using ScalarFixedValueDistribution       = FixedValueDistribution<double>;
using AngularFixedValueDistribution      = FixedValueDistribution<Angle>;
using PhotonEnergyFixedValueDistribution = FixedValueDistribution<PhotonEnergy>;

using ScalarUniformDistribution       = UniformDistribution<double>;
using AngularUniformDistribution      = UniformDistribution<Angle>;
using PhotonEnergyUniformDistribution = UniformDistribution<PhotonEnergy>;

using ScalarGaussianDistribution       = GaussianDistribution<double>;
using AngularGaussianDistribution      = GaussianDistribution<Angle>;
using PhotonEnergyGaussianDistribution = GaussianDistribution<PhotonEnergy>;

using ScalarBakedDistribution       = BakedDistribution<double>;
using AngularBakedDistribution      = BakedDistribution<Angle>;
using PhotonEnergyBakedDistribution = BakedDistribution<PhotonEnergy>;

using ScalarDistribution       = Distribution<double>;
using AngularDistribution      = Distribution<Angle>;
using PhotonEnergyDistribution = Distribution<PhotonEnergy>;

namespace defaults {
constexpr PhotonEnergyDistribution photonEnergy = PhotonEnergyFixedValueDistribution{.value = WaveLength{1.0} /* infrared */};
}

using ScalarCircularDistribution  = CircularDistribution<double>;
using AngularCircularDistribution = CircularDistribution<Angle>;

using ScalarRectangularDistribution  = RectangularDistribution<double>;
using AngularRectangularDistribution = RectangularDistribution<Angle>;

using ScalarAreaDistribution  = AreaDistribution<double>;
using AngularAreaDistribution = AreaDistribution<Angle>;

using ScalarSphericalDistribution = SphericalDistribution<double>;

using ScalarCylindricalDistribution = CylindricalDistribution<double>;

using ScalarCuboidalDistribution = CuboidalDistribution<double>;

using ScalarVolumetricDistribution = VolumetricDistribution<double>;
