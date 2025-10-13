#pragma once

#include "Core.h"
#include "Variant.h"

namespace RAYX {

// Cutout types:
// a subset of points of the XZ-plane. used to limit the potentially infinite surfaces.
// note that the first 3 need to be RECT; ELLIPTICAL; TRAPEZOID in order to be compatible with `geometricalShape` from Ray-UI.
enum class CutoutType {
    Rect,
    Elliptical,
    Trapezoid,
    Unlimited,
};

namespace detail {
struct RAYX_API CutoutTypes {
    /////////////////////////
    // Unlimited
    /////////////////////////

    // Every point (x, z) is within this cutout, it's unlimited after all.
    // `Unlimited` doesn't have any data so it doesn't need a struct.
    struct Unlimited {
        // no parameters
    };

    ///////////////////////////////////
    // Rect
    ///////////////////////////////////

    // A rectangle specified by width/length centered at (x=0, z=0).
    struct Rect {
        double m_width;
        double m_length;
    };

    ////////////////////////////
    // Elliptical
    ////////////////////////////

    // https://en.wikipedia.org/wiki/Ellipse
    //
    // An elliptical shape given by two diameters.
    // It can be understood as a circle with individual stretch-factors for both dimensions.
    // The point (x=0, z=0) lies at the center of the ellipse.

    struct Elliptical {
        double m_diameter_x;
        double m_diameter_z;
    };

    ////////////////////////
    // Trapezoid
    ////////////////////////

    // https://en.wikipedia.org/wiki/Trapezoid
    //
    // A trapezoid consists of two lines with lengths `m_widthA` and `m_widthB`, both parallel to the X-axis.
    // These lines have a distance of `m_length`.
    // The point (x=0, z=0) lies at the center of the trapezoid.

    struct Trapezoid {
        double m_widthA;
        double m_widthB;
        double m_length;
    };
};
}  // namespace detail

using Cutout = Variant<detail::CutoutTypes, detail::CutoutTypes::Rect, detail::CutoutTypes::Elliptical, detail::CutoutTypes::Trapezoid,
                       detail::CutoutTypes::Unlimited>;

}  // namespace RAYX
