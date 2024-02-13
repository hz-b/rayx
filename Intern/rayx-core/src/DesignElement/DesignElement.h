#pragma once

#include "Value.h"
#include "Shader/Element.h"

namespace RAYX{

struct DesignElement {
    Value v;
    Element compile() const;
};
}


