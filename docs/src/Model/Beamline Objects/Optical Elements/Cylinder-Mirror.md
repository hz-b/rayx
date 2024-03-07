# Cylindrical Mirror


## Tracing Parameter

TBA

## RML Object

To track a Cylindrical Mirror using an RML File, you'll require an XML Object to encompass all the mirror details. The default mirror setup from RAY-UI is provided here for your convenience. Remember, successful tracing requires you to specify a Light Source first. We suggest adding an Imageplane at the end for clearer results.

```XML
    <object name="Cylinder" type="Cylinder">
    <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
    <param id="totalWidth" enabled="T">50</param>
    <param id="totalLength" enabled="T">200</param>
    <param id="grazingIncAngle" enabled="T">10</param>
    <param id="entranceArmLength" enabled="T">10000</param>
    <param id="exitArmLength" enabled="T">1000</param>
    <param id="bendingRadius" comment="Long Radius R" enabled="T">0</param>
    <param id="radius" auto="T" enabled="T">10470.4917875</param>
    <param id="distancePreceding" enabled="T">10000</param>
    <param id="azimuthalAngle" enabled="T">0</param>
    <param id="reflectivityType" comment="100%" enabled="T">0</param>
    <param id="materialSubstrate" enabled="F">Au</param>
    <param id="roughnessSubstrate" enabled="F">0</param>
    <param id="densitySubstrate" auto="T" enabled="F">19.3</param>
    <param id="surfaceCoating" comment="Substrate only" enabled="F">0</param>
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