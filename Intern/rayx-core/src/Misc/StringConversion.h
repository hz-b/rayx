#pragma once

#include <iostream>
#include <map>
#include <string>

#include "Shader/LightSources/LightSource.h"

namespace rayx {


SurfaceElementType stringToSurfaceElementType(const std::string& str);
std::string surfaceElementTypeToString(SurfaceElementType type);

// no sources in SurfaceElementType, as they are not SurfaceElements
enum class SurfaceElementType {
    Crystal,
    CylinderMirror,
    ImagePlane,
    ParaboloidMirror,
    PlaneGrating,
    ReflectionZoneplate,
    Slit,
    EllipsoidMirror,
    ConeMirror,
    ExpertsMirror,
    PlaneMirror,
    CylinderMirror,
    ImagePlane,
    Paraboloid,
    RzpSphere,
    SphereMirror,
    SphereGrating,
    ToroidMirror,
    ToroidGrating,
    Foil,
    Aperature,
};

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

// ElementType conversion
const std::map<SurfaceElementType, std::string> SurfaceElementTypeToString = {
                                                                {SurfaceElementType::CylinderMirror, "Cylinder"},
                                                                {SurfaceElementType::ImagePlane, "ImagePlane"},
                                                                {SurfaceElementType::ParaboloidMirror, "Paraboloid"},
                                                                {SurfaceElementType::PlaneGrating, "Plane Grating"},
                                                                {SurfaceElementType::ReflectionZoneplate, "Reflection Zoneplate"},
                                                                {SurfaceElementType::Slit, "Slit"},
                                                                {SurfaceElementType::SphereMirror, "Sphere"},
                                                                {SurfaceElementType::ConeMirror, "Cone"},
                                                                {SurfaceElementType::ExpertsMirror, "Experts Optics"},
                                                                {SurfaceElementType::PlaneMirror, "Plane Mirror"},
                                                                {SurfaceElementType::SphereGrating, "Spherical Grating"},
                                                                {SurfaceElementType::SphereMirror, "Sphere Mirror"},
                                                                {SurfaceElementType::ToroidMirror, "Toroid"},
                                                                {SurfaceElementType::ToroidGrating, "Toroidal Grating"},
                                                                {SurfaceElementType::EllipsoidMirror, "Ellipsoid"},
                                                                {SurfaceElementType::Crystal, "Crystal"},
                                                                {SurfaceElementType::Foil, "Foil"},
                                                                {SurfaceElementType::Aperature, "Aperature"}};

const std::map<Object, std::string> ObjectToString = {{CircleSource, "Circle Source"},
                                                                {CylinderMirror, "Cylinder"},
                                                                {ImagePlane, "ImagePlane"},
                                                                {MatrixSource, "Matrix Source"},
                                                                {ParaboloidMirror, "Paraboloid"},
                                                                {PlaneGrating, "Plane Grating"},
                                                                {PointSource, "Point Source"},
                                                                {ReflectionZoneplate, "Reflection Zoneplate"},
                                                                {SimpleUndulatorSource, "Simple Undulator"},
                                                                {Slit, "Slit"},
                                                                {Sphere, "Sphere"},
                                                                {ConeMirror, "Cone"},
                                                                {ExpertsMirror, "Experts Optics"},
                                                                {PlaneMirror, "Plane Mirror"},
                                                                {SphereGrating, "Spherical Grating"},
                                                                {SphereMirror, "Sphere Mirror"},
                                                                {ToroidMirror, "Toroid"},
                                                                {ToroidGrating, "Toroidal Grating"},
                                                                {DipoleSource, "Dipole Source"},
                                                                {PixelSource, "Pixel Source"},
                                                                {EllipsoidMirror, "Ellipsoid"},
                                                                {Crystal, "Crystal"},
                                                                {Foil, "Foil"}};

const std::map<std::string, SurfaceElementType> StringToSurfaceElementType = {
                                                                {"Cylinder", SurfaceElementType::CylinderMirror},
                                                                {"ImagePlane", SurfaceElementType::ImagePlane},
                                                                {"Paraboloid", SurfaceElementType::ParaboloidMirror},
                                                                {"Plane Grating", SurfaceElementType::PlaneGrating},
                                                                {"Reflection Zoneplate", SurfaceElementType::ReflectionZoneplate},
                                                                {"Slit", SurfaceElementType::Slit},
                                                                {"Sphere", SurfaceElementType::SphereMirror},
                                                                {"Cone", SurfaceElementType::ConeMirror},
                                                                {"Experts Optics", SurfaceElementType::ExpertsMirror},
                                                                {"Plane Mirror", SurfaceElementType::PlaneMirror},
                                                                {"Spherical Grating", SurfaceElementType::SphereGrating},
                                                                {"Sphere Mirror", SurfaceElementType::SphereMirror},
                                                                {"Toroid", SurfaceElementType::ToroidMirror},
                                                                {"Toroidal Grating", SurfaceElementType::ToroidGrating},
                                                                {"Ellipsoid", SurfaceElementType::EllipsoidMirror},
                                                                {"Crystal", SurfaceElementType::Crystal},
                                                                {"Foil", SurfaceElementType::Foil}
                                                                {"Aperature", SurfaceElementType::Aperature}};
                                                                


const std::map<std::string, Object> StringToObjectType = {{"Circle Source", CircleSource},
                                                                {"Cylinder", SurfaceElement},
                                                                {"ImagePlane", SurfaceElement},
                                                                {"Matrix Source", MatrixSource},
                                                                {"Paraboloid", SurfaceElement},
                                                                {"Plane Grating", SurfaceElement},
                                                                {"Point Source", PointSource},
                                                                {"Reflection Zoneplate", SurfaceElement},
                                                                {"Simple Undulator", SimpleUndulatorSource},
                                                                {"Slit", SurfaceElement},
                                                                {"Sphere", SurfaceElement},
                                                                {"Cone", SurfaceElement},
                                                                {"Experts Optics", SurfaceElement},
                                                                {"Plane Mirror", SurfaceElement},
                                                                {"Spherical Grating", SurfaceElement},
                                                                {"Sphere Mirror", SurfaceElement},
                                                                {"Toroid", SurfaceElement},
                                                                {"Toroidal Grating", SurfaceElement},
                                                                {"Dipole Source", DipoleSource},
                                                                {"Dipole", DipoleSource},  // legacy rml
                                                                {"Pixel Source", PixelSource},
                                                                {"Ellipsoid", SurfaceElement},
                                                                {"Crystal", SurfaceElement},
                                                                {"Foil", SurfaceElement}};

const std::map<std::string, EventType> StringToEventType = {
    {"Uninitialized", EventType::Uninitialized}, {"Emitted", EventType::Emitted},   {"HitElement", EventType::HitElement},
    {"FatalError", EventType::FatalError},       {"Absorbed", EventType::Absorbed}, {"BeyondHorizon", EventType::BeyondHorizon},
    {"TooManyEvents", EventType::TooManyEvents},
};
const std::map<EventType, std::string> EventTypeToString = {
    {EventType::Uninitialized, "Uninitialized"}, {EventType::Emitted, "Emitted"},   {EventType::HitElement, "HitElement"},
    {EventType::FatalError, "FatalError"},       {EventType::Absorbed, "Absorbed"}, {EventType::BeyondHorizon, "BeyondHorizon"},
    {EventType::TooManyEvents, "TooManyEvents"},
};

const std::map<CutoutType, std::string> CutoutTypeToString = {
    {CutoutType::Unlimited, "Unlimited"}, {CutoutType::Rect, "Rect"}, {CutoutType::Elliptical, "Elliptical"}, {CutoutType::Trapezoid, "Trapezoid"}};
const std::map<std::string, CutoutType> StringToCutoutType = {
    {"Unlimited", CutoutType::Unlimited}, {"Rect", CutoutType::Rect}, {"Elliptical", CutoutType::Elliptical}, {"Trapezoid", CutoutType::Trapezoid}};

// Generic helper for all enums
template <typename Enum>
std::ostream& streamEnum(std::ostream& os, const Enum value, const std::map<Enum, std::string>& toStringMap) {
    if (auto it = toStringMap.find(value); it != toStringMap.end()) {
        os << it->second;
    } else {
        os << "<unknown>";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const SpreadType v) { return streamEnum(os, v, SpreadTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const EnergyDistributionType v) { return streamEnum(os, v, EnergyDistributionTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const SourceDist v) { return streamEnum(os, v, SourceDistToString); }
inline std::ostream& operator<<(std::ostream& os, const ElectronEnergyOrientation v) { return streamEnum(os, v, ElectronEnergyOrientationToString); }
inline std::ostream& operator<<(std::ostream& os, const EnergySpreadUnit v) { return streamEnum(os, v, EnergySpreadUnitToString); }
inline std::ostream& operator<<(std::ostream& os, const RZPType v) { return streamEnum(os, v, RZPTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const CentralBeamstop v) { return streamEnum(os, v, CentralBeamstopToString); }
inline std::ostream& operator<<(std::ostream& os, const GratingMount v) { return streamEnum(os, v, GratingMountToString); }
inline std::ostream& operator<<(std::ostream& os, const ParaboloidType v) { return streamEnum(os, v, ParaboloidTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const CurvatureType v) { return streamEnum(os, v, CurvatureTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const BehaviourType v) { return streamEnum(os, v, BehaviourTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const FigureRotation v) { return streamEnum(os, v, FigureRotationToString); }
inline std::ostream& operator<<(std::ostream& os, const SigmaType v) { return streamEnum(os, v, SigmaTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const Material v) { return streamEnum(os, v, MaterialToString); }
inline std::ostream& operator<<(std::ostream& os, const ElementType v) { return streamEnum(os, v, ElementTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const EventType v) { return streamEnum(os, v, EventTypeToString); }
inline std::ostream& operator<<(std::ostream& os, const CutoutType v) { return streamEnum(os, v, CutoutTypeToString); }

}  // namespace rayx