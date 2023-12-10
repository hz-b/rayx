#pragma once

namespace RAYX {

#define DECLARE_STR(x, s) struct x { static std::string str() { return s; } }

DECLARE_STR(RectStr, "rect");
DECLARE_STR(EllipticalStr, "elliptical");
DECLARE_STR(TrapezoidStr, "trapezoid");
DECLARE_STR(UnlimitedStr, "unlimited");
DECLARE_STR(WidthStr, "width");
DECLARE_STR(DiameterXStr, "diameter_x");
DECLARE_STR(DiameterZStr, "diameter_z");
DECLARE_STR(WidthAStr, "widthA");
DECLARE_STR(WidthBStr, "widthB");
DECLARE_STR(LengthStr, "length");
DECLARE_STR(SourceStr, "source");
DECLARE_STR(ElementStr, "element");
DECLARE_STR(NameStr, "name");
DECLARE_STR(CutoutStr, "cutout");
DECLARE_STR(NumberOfRaysStr, "numberOfRays");
DECLARE_STR(EnergyDistributionStr, "energyDistribution");

DECLARE_STR(SourceTypeStr, "sourceType");
DECLARE_STR(HardEdgeStr, "hardEdge");
DECLARE_STR(SoftEdgeStr, "softEdge");
DECLARE_STR(SeparateEnergiesStr, "separateEnergies");
DECLARE_STR(DatFileStr, "datFile");
DECLARE_STR(MatrixSourceStr, "matrixSource");

DECLARE_STR(LinearPol0Str, "linearPol0");
DECLARE_STR(LinearPol45Str, "linearPol45");
DECLARE_STR(CircularPolStr, "circularPol");
DECLARE_STR(SourceDepthStr, "sourceDepth");

DECLARE_STR(SourceHeightStr, "sourceHeight");
DECLARE_STR(SourceWidthStr, "sourceWidth");
DECLARE_STR(HorDivergenceStr, "horDivergence");
DECLARE_STR(VerDivergenceStr, "verDivergence");
DECLARE_STR(TransformStr, "transform");

DECLARE_STR(ByMatrixStr, "byMatrix");

DECLARE_STR(MisalignmentStr, "misalignment");
DECLARE_STR(TranslationXErrorStr, "translationXError");
DECLARE_STR(TranslationYErrorStr, "translationYError");
DECLARE_STR(TranslationZErrorStr, "translationZError");
DECLARE_STR(RotationXErrorStr, "rotationXError");
DECLARE_STR(RotationYErrorStr, "rotationYError");
DECLARE_STR(RotationZErrorStr, "rotationZError");

}
