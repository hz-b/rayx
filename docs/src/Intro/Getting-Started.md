# Getting Started 

Currently we only support a one over trace for a given beamline file. This means, 
you can either use RAY-UI to generate a beamline file or alter an existing one.

Here is an example for a simple beamline file "PlaneMirror.rml":
``` XML
<?xml version="1.0" encoding="UTF-8" ?>
<lab>
<version>1.1</version>
<beamline>

  <object name="Matrix Source" type="Matrix Source">
    <param id="numberRays" enabled="T">100</param>
    <param id="sourceWidth" enabled="T">0.065</param>
    <param id="sourceHeight" enabled="T">0.04</param>
    <param id="sourceDepth" enabled="T">0</param>
    <param id="horDiv" enabled="T">1</param>
    <param id="verDiv" enabled="T">1</param>
    <param id="energyDistributionType" comment="Values" enabled="T">1</param>
    <param id="photonEnergyDistributionFile" relative="" enabled="F"></param>
    <param id="photonEnergy" enabled="T">100</param>
    <param id="energySpreadType" comment="white band" enabled="T">0</param>
    <param id="energySpread" enabled="T">0</param>
    <param id="linearPol_0" enabled="T">1</param>
    <param id="linearPol_45" enabled="T">0</param>
    <param id="circularPol" enabled="T">0</param>
    <param id="sourcePulseType" comment="all rays start simultaneously" enabled="T">0</param>
    <param id="sourcePulseLength" enabled="F">0</param>
    <param id="worldPosition" enabled="F">
      <x>0</x>
      <y>0</y>
      <z>0</z>
    </param>
    <param id="worldXdirection" enabled="F">
      <x>1</x>
      <y>0</y>
      <z>0</z>
    </param>
    <param id="worldYdirection" enabled="F">
      <x>0</x>
      <y>1</y>
      <z>0</z>
    </param>
    <param id="worldZdirection" enabled="F">
      <x>0</x>
      <y>0</y>
      <z>1</z>
    </param>
  </object>

  <object name="Plane Mirror" type="Plane Mirror">
    <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
    <param id="totalWidth" enabled="T">50</param>
    <param id="totalLength" enabled="T">200</param>
    <param id="grazingIncAngle" auto="T" enabled="T">40</param>
    <param id="distancePreceding" enabled="T">10000</param>
    <param id="azimuthalAngle" auto="T" enabled="T">0</param>
    <param id="systemMount" comment="standalone" enabled="T">0</param>
    <param id="premirrorShiftZ" enabled="F">0</param>
    <param id="pimpaleAlpha1" enabled="F">1</param>
    <param id="pimpaleAlpha2" enabled="F">2</param>
    <param id="pimpaleAlpha3" enabled="F">3</param>
    <param id="distancePremirrorGrating" enabled="T">0</param>
    <param id="reflectivityType" comment="100%" enabled="T">0</param>
    <param id="elementSubstrate" enabled="F">Au</param>
    <param id="roughnessSubstrate" enabled="F">0</param>
    <param id="densitySubstrate" auto="T" enabled="F">19.3</param>
    <param id="surfaceCoating" comment="Substrate only" enabled="F">0</param>
    <param id="coatingFile" relative="" enabled="F"></param>
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

</beamline>

<ExtraData>
</ExtraData>
</lab>

```


In it we have a matrix source that sends rays onto a plane mirror. 
Assuming the beamline file is in the same directory as the binary
you can run the CLI of rayx as follows:
```
./TerminalApp -i PlaneMirror.rml
```

Adding a `-p` flag will output a footprint of the last element in
in the beamline, which was hit by rays. 