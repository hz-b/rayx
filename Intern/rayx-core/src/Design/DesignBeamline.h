#pragma once

#include <vector>
#include "DesignLightSource.h"
#include "DesignElement.h"
#include "TypedTable.h"
#include "Beamline/Beamline.h"
#include "Strings.h"

namespace RAYX {

class DesignBeamline : public TypedTable<
        Field<std::vector<DesignLightSource>, LightSourceStr>,
        Field<std::vector<DesignElement>, ElementStr>
    > {
    public:
        Beamline compile() const;
};

}