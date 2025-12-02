# Ellipsoid Mirror


## Tracing Parameter

Mandatory:
- Reflectivity Type (see more in chapter Optical Elements)
- Slope Error
- World Position
- Misalignment

Ellipoid:
- Entrance Arm Length
- Exit Arm Length
- Short Half Axis B
- Long Half Axis A
- Design Grazing Inc Angle
- Figure Rotation
- Parameter A11

Cutout:
- Geometrical Shape
- Total Width
- Total Length


## RML Object

To track a Ellipsoid Mirror using an RML File, you'll require an XML Object to encompass all the mirror details. The default mirror setup from RAY-UI is provided here for your convenience. Remember, successful tracing requires you to specify a Light Source first. We suggest adding an Imageplane at the end for clearer results.

```XML
    <object name="Ellipsoid" type="Ellipsoid">
    <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
    <param id="totalWidth" enabled="T">50</param>
    <param id="totalLength" enabled="T">200</param>
    <param id="grazingIncAngle" enabled="T">10</param>
    <param id="entranceArmLength" enabled="T">10000</param>
    <param id="exitArmLength" enabled="T">1000</param>
    <param id="designGrazingIncAngle" auto="T" enabled="T">10</param>
    <param id="longHalfAxisA" auto="T" enabled="T">5500</param>
    <param id="shortHalfAxisB" auto="T" enabled="T">549.12375296508355</param>
    <param id="figureRotation" comment="Yes" enabled="T">0</param>
    <param id="parameter_a11" enabled="F">1</param>
    <param id="distancePreceding" enabled="T">10000</param>
    <param id="azimuthalAngle" enabled="T">0</param>
    <param id="reflectivityType" comment="Derived by Material" enabled="T">1</param>
    <param id="materialSubstrate" enabled="T">Au</param>
    <param id="roughnessSubstrate" enabled="T">0</param>
    <param id="densitySubstrate" auto="T" enabled="T">19.300000000000001</param>
    <param id="surfaceCoating" comment="Substrate only" enabled="T">0</param>
    <param id="coatingFile" absolute="" enabled="F"></param>
    <param id="numberLayer" enabled="F">2</param>
    <param id="materialCoating1" enabled="F"></param>
    <param id="thicknessCoating1" enabled="F">0</param>
    <param id="roughnessCoating1" enabled="F">0</param>
    <param id="densityCoating1" auto="T" enabled="F">0</param>
    <param id="materialCoating2" enabled="F"></param>
    <param id="thicknessCoating2" enabled="F">0</param>
    <param id="roughnessCoating2" enabled="F">0</param>
    <param id="densityCoating2" auto="T" enabled="F">0</param>
    <param id="materialTopLayer" enabled="F"></param>
    <param id="thicknessTopLayer" enabled="F">0</param>
    <param id="roughnessTopLayer" enabled="F">0</param>
    <param id="densityTopLayer" auto="T" enabled="F">0</param>
    <param id="lateralThicknessGradientCoating1" comment="No" enabled="F">0</param>
    <param id="gradientC1B1" enabled="F">0</param>
    <param id="gradientC1B2" enabled="F">0</param>
    <param id="gradientC1B3" enabled="F">0</param>
    <param id="gradientC1B4" enabled="F">0</param>
    <param id="gradientC1B5" enabled="F">0</param>
    <param id="gradientC1B6" enabled="F">0</param>
    <param id="gradientC1B7" enabled="F">0</param>
    <param id="gradientC1B8" enabled="F">0</param>
    <param id="alignmentError" comment="No" enabled="T">1</param>
    <param id="misalignmentCoordinateSystem" comment="Ellipsoid" enabled="T">0</param>
    <param id="translationXerror" enabled="F">0</param>
    <param id="translationYerror" enabled="F">0</param>
    <param id="translationZerror" enabled="F">0</param>
    <param id="rotationXerror" enabled="F">0</param>
    <param id="rotationYerror" enabled="F">0</param>
    <param id="rotationZerror" enabled="F">0</param>
    <param id="slopeError" comment="No" enabled="T">1</param>
    <param id="profileKind" comment="no Profile" enabled="F">2</param>
    <param id="profileFile" absolute="" enabled="F"></param>
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
      <y>0.98480775301220802</y>
      <z>-0.17364817766693033</z>
    </param>
    <param id="worldZdirection" enabled="F">
      <x>0</x>
      <y>0.17364817766693033</y>
      <z>0.98480775301220802</z>
    </param>
  </object>
```