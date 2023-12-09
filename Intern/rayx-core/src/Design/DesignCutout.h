#pragma once

#include "TypedVariant.h"
#include "TypedTable.h"
#include "Strings.h"

namespace RAYX {

struct DesignRect : public TypedTable<
    Field<double, WidthStr>,
    Field<double, LengthStr>
> {};

struct DesignElliptical : public TypedTable<
    Field<double, DiameterXStr>,
    Field<double, DiameterZStr>
> {};

struct DesignTrapezoid : public TypedTable<
    Field<double, WidthAStr>,
    Field<double, WidthBStr>,
    Field<double, LengthStr>
> {};

struct DesignUnlimited : public TypedTable<> {};

struct DesignCutout : public TypedVariant<
    Case<DesignRect, RectStr>,
    Case<DesignElliptical, EllipticalStr>,
    Case<DesignTrapezoid, TrapezoidStr>,
    Case<DesignUnlimited, UnlimitedStr>
> {};

}
