#include <map>
#include <string>

#include "LightSource.h"
#include "Objects/BehaviourType.h"

namespace RAYX {

// String conversion functions

// SpreadType conversion
const std::map<SpreadType, std::string> SpreadTypeToString = {
    {SpreadType::HardEdge, "HardEdge"}, {SpreadType::SoftEdge, "SoftEdge"}, {SpreadType::SeparateEnergies, "SeparateEnergies"}};
const std::map<std::string, SpreadType> StringToSpreadType = {
    {"HardEdge", SpreadType::HardEdge}, {"SoftEdge", SpreadType::SoftEdge}, {"SeparateEnergies", SpreadType::SeparateEnergies}};

// EnergyDistributionType conversion
const std::map<EnergyDistributionType, std::string> EnergyDistributionTypeToString = {{EnergyDistributionType::File, "File"},
                                                                                      {EnergyDistributionType::Values, "Values"},
                                                                                      {EnergyDistributionType::Total, "Total"},
                                                                                      {EnergyDistributionType::Param, "Param"}};
const std::map<std::string, EnergyDistributionType> StringToEnergyDistributionType = {{"File", EnergyDistributionType::File},
                                                                                      {"Values", EnergyDistributionType::Values},
                                                                                      {"Total", EnergyDistributionType::Total},
                                                                                      {"Param", EnergyDistributionType::Param}};

// SourceDist conversion
const std::map<SourceDist, std::string> SourceDistToString = {
    {SourceDist::Uniform, "Uniform"}, {SourceDist::Gaussian, "Gaussian"}, {SourceDist::Thirds, "Thirds"}, {SourceDist::Circle, "Circle"}};
const std::map<std::string, SourceDist> StringToSourceDist = {
    {"Uniform", SourceDist::Uniform}, {"Gaussian", SourceDist::Gaussian}, {"Thirds", SourceDist::Thirds}, {"Circle", SourceDist::Circle}};

// ElectronEnergyOrientation conversion
const std::map<ElectronEnergyOrientation, std::string> ElectronEnergyOrientationToString = {
    {ElectronEnergyOrientation::Clockwise, "Clockwise"}, {ElectronEnergyOrientation::Counterclockwise, "Counterclockwise"}};
const std::map<std::string, ElectronEnergyOrientation> StringToElectronEnergyOrientation = {
    {"Clockwise", ElectronEnergyOrientation::Clockwise}, {"Counterclockwise", ElectronEnergyOrientation::Counterclockwise}};

// EnergySpreadUnit conversion
const std::map<EnergySpreadUnit, std::string> EnergySpreadUnitToString = {{EnergySpreadUnit::EU_PERCENT, "Percent"}, {EnergySpreadUnit::EU_eV, "eV"}};
const std::map<std::string, EnergySpreadUnit> StringToEnergySpreadUnit = {{"Percent", EnergySpreadUnit::EU_PERCENT}, {"eV", EnergySpreadUnit::EU_eV}};

// RZPType conversion
const std::map<RZPType, std::string> RZPTypeToString = {{RZPType::Elliptical, "Elliptical"}, {RZPType::Meriodional, "Meriodional"}};
const std::map<std::string, RZPType> StringToRZPType = {{"Elliptical", RZPType::Elliptical}, {"Meriodional", RZPType::Meriodional}};

// CentralBeamstop conversion
const std::map<CentralBeamstop, std::string> CentralBeamstopToString = {
    {CentralBeamstop::None, "None"}, {CentralBeamstop::Rectangle, "Rectangle"}, {CentralBeamstop::Elliptical, "Elliptical"}};
const std::map<std::string, CentralBeamstop> StringToCentralBeamstop = {
    {"None", CentralBeamstop::None}, {"Rectangle", CentralBeamstop::Rectangle}, {"Elliptical", CentralBeamstop::Elliptical}};

// GratingMount conversion
const std::map<GratingMount, std::string> GratingMountToString = {{GratingMount::Deviation, "Deviation"}, {GratingMount::Incidence, "Incidence"}};
const std::map<std::string, GratingMount> StringToGratingMount = {{"Deviation", GratingMount::Deviation}, {"Incidence", GratingMount::Incidence}};

// ParaboloidType conversion
const std::map<ParaboloidType, std::string> ParaboloidTypeToString = {{ParaboloidType::Focussing, "Focussing"},
                                                                      {ParaboloidType::Collimate, "Collimate"}};
const std::map<std::string, ParaboloidType> StringToParaboloidType = {{"Focussing", ParaboloidType::Focussing},
                                                                      {"Collimate", ParaboloidType::Collimate}};

// CurvatureType conversion
const std::map<CurvatureType, std::string> CurvatureTypeToString = {
    {CurvatureType::Plane, "Plane"},         {CurvatureType::Toroidal, "Toroidal"},
    {CurvatureType::Spherical, "Spherical"}, {CurvatureType::Cubic, "Cubic"},
    {CurvatureType::Cone, "Cone"},           {CurvatureType::Cylinder, "Cylinder"},
    {CurvatureType::Ellipsoid, "Ellipsoid"}, {CurvatureType::Paraboloid, "Paraboloid"},
    {CurvatureType::Quadric, "Quadric"},     {CurvatureType::RzpSphere, "RzpSphere"}};
const std::map<std::string, CurvatureType> StringToCurvatureType = {
    {"Plane", CurvatureType::Plane},         {"Toroidal", CurvatureType::Toroidal},
    {"Spherical", CurvatureType::Spherical}, {"Cubic", CurvatureType::Cubic},
    {"Cone", CurvatureType::Cone},           {"Cylinder", CurvatureType::Cylinder},
    {"Ellipsoid", CurvatureType::Ellipsoid}, {"Paraboloid", CurvatureType::Paraboloid},
    {"Quadric", CurvatureType::Quadric},     {"RzpSphere", CurvatureType::RzpSphere}};

// BehaviourType conversion
const std::map<BehaviourType, std::string> BehaviourTypeToString = {{BehaviourType::Mirror, "Mirror"},
                                                                    {BehaviourType::Grating, "Grating"},
                                                                    {BehaviourType::Slit, "Slit"},
                                                                    {BehaviourType::Rzp, "Rzp"},
                                                                    {BehaviourType::ImagePlane, "ImagePlane"}};
const std::map<std::string, BehaviourType> StringToBehaviourType = {{"Mirror", BehaviourType::Mirror},
                                                                    {"Grating", BehaviourType::Grating},
                                                                    {"Slit", BehaviourType::Slit},
                                                                    {"Rzp", BehaviourType::Rzp},
                                                                    {"ImagePlane", BehaviourType::ImagePlane}};

// FigureRotation conversion
const std::map<FigureRotation, std::string> FigureRotationToString = {
    {FigureRotation::Yes, "Yes"}, {FigureRotation::Plane, "Plane"}, {FigureRotation::A11, "A11"}};
const std::map<std::string, FigureRotation> StringToFigureRotation = {
    {"Yes", FigureRotation::Yes}, {"Plane", FigureRotation::Plane}, {"A11", FigureRotation::A11}};

// SigmaType conversion
const std::map<SigmaType, std::string> SigmaTypeToString = {{SigmaType::ST_STANDARD, "Standard"}, {SigmaType::ST_ACCURATE, "Accurate"}};
const std::map<std::string, SigmaType> StringToSigmaType = {{"Standard", SigmaType::ST_STANDARD}, {"Accurate", SigmaType::ST_ACCURATE}};

// Material conversion
const std::map<Material, std::string> MaterialToString = {
    {Material::REFLECTIVE, "REFLECTIVE"},
    {Material::VACUUM, "VACUUM"},

// Add other materials here as needed
#define X(e, z, a, rho) {Material::e, #e},
#include "Material/materials.xmacro"
#undef X
};
const std::map<std::string, Material> StringToMaterial = {
    {"REFLECTIVE", Material::REFLECTIVE},
    {"VACUUM", Material::VACUUM},
// Add other materials here as needed
#define X(e, z, a, rho) {#e, Material::e},
#include "Material/materials.xmacro"
#undef X
};

}  // namespace RAYX
