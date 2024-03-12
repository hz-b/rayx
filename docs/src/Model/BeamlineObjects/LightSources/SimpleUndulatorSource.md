# Simple Undulator Source

The simple Undulator source mimics the physics of a real undulator source and allows the user to trace an approximated undulator lightbeam much faster. 
This light source utilieses the Gaussian Beam Theorie which allows us to simplify the beam Properties a lot. We already treat the lightbeam as a group of individual rays. This gives us the possibility to substitute all complex wave theory laws on our light beam with approximations.[1]
Simply put the Gaussian Beam Theory says that some beam properties are distributed in gaussian patterns. And that the pattern flattens or sqishes from collisions with optical elements but generaly remains a gaussian bellcurve. 


## Light Properties

In RAYX every ray is described by four properties. The values are randomly generated for the given distirubution. This guarantees that minimal systematic errors are impacting the simulation. 

light properties:
- Origin
- Direction
- Photonenergy
- Polarisation

### Origin

The Origin for each Ray is described by x, y and z Position. 

## Tracing Parameter

- sourceDepth
- linearPol0
- linearPol45
- circularPol
- undulatorLength
- photonEnergy
- sigmaType
- electronSigmaX
- electronSigmaXs
- electronSigmaY
- electronSigmaYs
- Energy
- Energy Spread
- Energy Spread Type
- Energy Distribution Type
- Energy Distribution File

## RML Object

To trace a simple Undulator Source through an RML File, utilize an XML Object to encompass all pertinent information for the light source. The default configuration for the light source from RAY-UI is presented here for your ease of use. It is recommended to include an Imageplane at the end for clearer results.

```XML
<object name="Simple Undulator" type="Simple Undulator">
   <param id="numberRays" enabled="T">20000</param>
   <param id="sigmaType" comment="standard" enabled="T">0</param>
   <param id="undulatorLength" enabled="T">4</param>
   <param id="electronDistributionType" comment="manual" enabled="T">0</param>
   <param id="electronSigmaX" enabled="T">216</param>
   <param id="electronSigmaXs" enabled="T">24.8</param>
   <param id="electronSigmaY" enabled="T">18.7</param>
   <param id="electronSigmaYs" enabled="T">4.3</param>
   <param id="sourceDepth" enabled="T">1</param>
   <param id="alignmentError" comment="No" enabled="T">1</param>
   <param id="translationXerror" enabled="F">0</param>
   <param id="translationYerror" enabled="F">0</param>
   <param id="translationZerror" enabled="F">0</param>
   <param id="rotationXerror" enabled="F">0</param>
   <param id="rotationYerror" enabled="F">0</param>
   <param id="worldPosition" enabled="F">
    <x>0.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>0.0000000000000000</z>
   </param>
   <param id="worldXdirection" enabled="F">
    <x>1.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>0.0000000000000000</z>
   </param>
   <param id="worldYdirection" enabled="F">
    <x>0.0000000000000000</x>
    <y>1.0000000000000000</y>
    <z>0.0000000000000000</z>
   </param>
   <param id="worldZdirection" enabled="F">
    <x>0.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>1.0000000000000000</z>
   </param>
   <param id="energyDistributionType" comment="Values" enabled="T">1</param>
   <param id="photonEnergyDistributionFile" absolute="" enabled="F"></param>
   <param id="photonEnergy" enabled="T">100</param>
   <param id="energySpreadType" comment="white band" enabled="T">0</param>
   <param id="energySpreadUnit" comment="eV" enabled="T">0</param>
   <param id="energySpread" enabled="T">0</param>
   <param id="linearPol_0" enabled="T">1</param>
   <param id="linearPol_45" enabled="T">0</param>
   <param id="circularPol" enabled="T">0</param>
   <param id="sourcePulseType" comment="all rays start simultaneously" enabled="T">0</param>
   <param id="sourcePulseLength" enabled="F">0</param>
  </object>

```
# Literature
[1]Representation of a Gaussian beam by rays
P. P. Crooker,a兲 W. B. Colson, and J. Blau
Physics Department, Naval Postgraduate School, Monterey, California 93943
Received 3 October 2005; accepted 7 April 2006兲