#pragma once

#include <string>
#include <variant>

namespace rayx::design {

using AtomicNumber    = int;
using ChemicalFormula = std::string;

namespace materials {

#define X(elementName, atomicNumber, atomicWeight, density) static constexpr AtomicNumber elementName = atomicNumber;
#include "AtomicElements.xmacro"
#undef X

#define X(chemicalFormula) static constexpr ChemicalFormula chemicalFormula = #chemicalFormula;
#include "Molecules.xmacro"
#undef X

}  // namespace materials

using Material = std::variant<AtomicNumber, ChemicalFormula>;

}  // namespace rayx::design
