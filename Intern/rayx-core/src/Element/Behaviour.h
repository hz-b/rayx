#pragma once

#include "Core.h"
#include "Cutout.h"
#include "Variant.h"

namespace RAYX {

#undef m_private_serialization_params

// A behaviour decides what happens whenever a ray hits the surface of this element.
// Each behaviour type has its own `behave` function in `Behave.h`.
enum class BehaveType { Mirror, Grating, Slit, RZP, ImagePlane, Crystal, Foil };

namespace detail {
struct BehaviourTypes {
    struct Mirror {
        // no parameters
    };

    struct Grating {
        double m_vls[6];          // VLS coefficients
        double m_lineDensity;     // lines per mm
        int m_orderOfDiffraction; /* the diffraction order, usually 1 */
    };

    struct Slit {
        Cutout m_openingCutout;   // The cutout of the small opening in the slit.
        Cutout m_beamstopCutout;  // The cutout of the beamstop inside the small opening.
    };

    struct RZP {
        int m_imageType;
        int m_rzpType;
        int m_derivationMethod;
        int m_designOrderOfDiffraction;
        int m_orderOfDiffraction;
        int m_additionalOrder;
        double m_designWavelength;
        double m_fresnelZOffset;
        double m_designSagittalEntranceArmLength;
        double m_designSagittalExitArmLength;
        double m_designMeridionalEntranceArmLength;
        double m_designMeridionalExitArmLength;
        double m_designAlphaAngle;
        double m_designBetaAngle;
    };

    struct ImagePlane {
        // no parameters
    };

    struct Crystal {
        double m_dSpacing2;
        double m_unitCellVolume;
        double m_offsetAngle;

        double m_structureFactorReF0;
        double m_structureFactorImF0;
        double m_structureFactorReFH;
        double m_structureFactorImFH;
        double m_structureFactorReFHC;
        double m_structureFactorImFHC;
    };

    struct Foil {
        // Substrates
        double m_thicknessSubstrate;
        double m_roughnessSubstrate;
    };
};
}  // namespace detail

using Behaviour =
    Variant<detail::BehaviourTypes, detail::BehaviourTypes::Mirror, detail::BehaviourTypes::Grating, detail::BehaviourTypes::Slit,
            detail::BehaviourTypes::RZP, detail::BehaviourTypes::ImagePlane, detail::BehaviourTypes::Crystal, detail::BehaviourTypes::Foil>;

class DesignElement;
enum class RZPType { Elliptical, Meriodional };
enum class CentralBeamstop { None, Rectangle, Elliptical };
Behaviour makeBehaviour(const DesignElement& dele);
Behaviour makeCrystal(const DesignElement& dele);
Behaviour makeGrating(const DesignElement& dele);  //< creates a Grating Behaviour from the parameters given in `dele`.
Behaviour makeSlit(const DesignElement& dele);
Behaviour makeRZPBehaviour(const DesignElement& dele);
Behaviour makeFoil(const DesignElement& dele);

}  // namespace RAYX
