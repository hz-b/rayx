#pragma once

#include "TypedTable.h"
#include "DesignCutout.h"
#include "Strings.h"

namespace RAYX {

struct DesignElement : public TypedTable<
    Field<std::string, NameStr>,
    Field<DesignCutout, CutoutStr>
>{};

}
