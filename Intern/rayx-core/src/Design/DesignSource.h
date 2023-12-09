#pragma once

#include "TypedTable.h"
#include "DesignElement.h"
#include "Strings.h"

namespace RAYX {

struct DesignSource : public TypedTable<
    Field<std::string, NameStr>,
    Field<int, NumberOfRaysStr>
> {};

}
