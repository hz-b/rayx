#pragma once

#include "TypedTable.h"

struct DesignMisalignment : public TypedTable<
    Field<double, TranslationXErrorStr>,
    Field<double, TranslationYErrorStr>,
    Field<double, TranslationZErrorStr>,
    Field<double, RotationXErrorStr>,
    Field<double, RotationYErrorStr>,
    Field<double, RotationZErrorStr>
> {};
