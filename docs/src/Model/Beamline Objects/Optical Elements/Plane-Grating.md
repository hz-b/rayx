# Plane Grating

A Plane Grating is amongst the simplest Elements you can include in your beamline. The Surface is flat and it reflects light like an optical grating. 

## Tracing Parameter

Mandatory:
- Reflectivity Type (see more in chapter Optical Elements)
- Slope Error
- World Position
- Misalignment

Grating:
- vls parameter
- Line Density
- Order Of Diffraction

Cutout:
- Geometrical Shape
- Total Width
- Total Length


## RML Object

To track a Plane Grating using an RML File, you'll require an XML Object to encompass all the grating details. Remember, successful tracing requires you to specify a Light Source first. We suggest adding an Imageplane at the end for clearer results.

```XML
  <object name="Plane Grating" type="Plane Grating">
    <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
    <param id="totalWidth" enabled="T">50</param>
    <param id="totalLength" enabled="T">200</param>
    <param id="gratingMount" comment="constant deviation" enabled="T">0</param>
    <param id="systemMount" comment="standalone, none" enabled="T">0</param>
    <param id="deviationAngle" enabled="T">10</param>
    <param id="halfConeAngle" enabled="F">10</param>
    <param id="pimpaleX0" enabled="F">10000</param>
    <param id="pimpaleY0" enabled="F">10</param>
    <param id="premirrorMountPsi0" enabled="F">0</param>
    <param id="designEnergyMounting" auto="T" enabled="T">100</param>
    <param id="lineDensity" enabled="T">1000</param>
    <param id="orderDiffraction" enabled="T">1</param>
    <param id="cFactor" enabled="F">2</param>
    <param id="alpha" auto="T" enabled="T">5.35655050894</param>
    <param id="beta" auto="T" enabled="T">-4.64344949106</param>
    <param id="distancePreceding" enabled="T">10000</param>
    <param id="azimuthalAngle" enabled="T">0</param>
    <param id="entranceArmLength" enabled="F">10000</param>
    <param id="lineSpacing" comment="constant" enabled="T">0</param>
    <param id="vlsParameterB2" enabled="F">0</param>
    <param id="vlsParameterB3" enabled="F">0</param>
    <param id="vlsParameterB4" enabled="F">0</param>
    <param id="vlsParameterB5" enabled="F">0</param>
    <param id="vlsParameterB6" enabled="F">0</param>
    <param id="vlsParameterB7" enabled="F">0</param>
    <param id="lineProfile" comment="unknown" enabled="F">3</param>
    <param id="gratingEfficiency" enabled="F">0.5</param>
    <param id="blazeAngle" enabled="F">4</param>
    <param id="aspectAngle" enabled="F">90</param>
    <param id="grooveDepth" enabled="F">10</param>
    <param id="grooveRatio" enabled="F">0.65</param>
    <param id="multilayerFourierCoefficients" auto="T" enabled="F">11</param>
    <param id="multilayerIntegrationSteps" auto="T" enabled="F">50</param>
    <param id="reflectivitySenkrecht" enabled="T">1</param>
    <param id="reflectivityParallel" enabled="T">1</param>
    <param id="reflectivityPhase" enabled="T">0</param>
    <param id="reflectivityType" comment="100%" enabled="T">0</param>
    <param id="materialSubstrate" enabled="F">Au</param>
    <param id="roughnessSubstrate" enabled="F">0</param>
    <param id="densitySubstrate" auto="T" enabled="F">19.3</param>
    <param id="surfaceCoating" comment="Substrate only" enabled="F">0</param>
    <param id="numberLayer" enabled="F">2</param>
    <param id="materialCoating1" enabled="F"></param>
    <param id="thicknessCoating1" enabled="F">0</param>
    <param id="densityCoating1" auto="T" enabled="F">0</param>
    <param id="materialCoating2" enabled="F"></param>
    <param id="thicknessCoating2" enabled="F">0</param>
    <param id="densityCoating2" auto="T" enabled="F">0</param>
    <param id="alignmentError" comment="No" enabled="T">1</param>
    <param id="translationXerror" enabled="F">0</param>
    <param id="translationYerror" enabled="F">0</param>
    <param id="translationZerror" enabled="F">0</param>
    <param id="rotationXerror" enabled="F">0</param>
    <param id="rotationYerror" enabled="F">0</param>
    <param id="rotationZerror" enabled="F">0</param>
    <param id="slopeError" comment="No" enabled="T">1</param>
    <param id="profileKind" comment="no Profile" enabled="F">2</param>
    <param id="profileFile" relative="" enabled="F"></param>
    <param id="slopeErrorSag" enabled="F">0</param>
    <param id="slopeErrorMer" enabled="F">0</param>
    <param id="thermalDistortionAmp" enabled="F">0</param>
    <param id="thermalDistortionSigmaX" enabled="F">0</param>
    <param id="thermalDistortionSigmaZ" enabled="F">0</param>
    <param id="cylindricalBowingAmp" enabled="F">0</param>
    <param id="cylindricalBowingRadius" enabled="F">0</param>
    <param id="worldPosition" enabled="F">
      <x>0</x>
      <y>0</y>
      <z>10000</z>
    </param>
    <param id="worldXdirection" enabled="F">
      <x>1</x>
      <y>0</y>
      <z>0</z>
    </param>
    <param id="worldYdirection" enabled="F">
      <x>0</x>
      <y>0.766044443118978</y>
      <z>-0.6427876096865393</z>
    </param>
    <param id="worldZdirection" enabled="F">
      <x>0</x>
      <y>0.6427876096865393</y>
      <z>0.766044443118978</z>
    </param>
  </object>
```