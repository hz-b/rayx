# Circle Source

## Light Properties

In RAYX, every ray is described by four properties. The values are randomly generated according to the given distribution. This ensures that minimal systematic errors impact the simulation.

Light properties include:
- Origin
- Direction
- Photon Energy
- Polarization

### Origin

The origin of each ray is described by its x, y, and z positions.

## Tracing Parameters

- Source Depth
- Source Height
- Source Width
- Linear Pol 0
- Linear Pol 45
- Circular Pol
- numOfEquidistantCircles
- maxOpeningAngle
- minOpeningAngle
- deltaOpeningAngle
- Energy
- Energy Spread
- Energy Spread Type
- Energy Distribution Type
- Energy Distribution File

## RML Object

To trace a Circle Source through an RML file, use an XML object to contain all relevant information for the light source. Below is the default configuration for the light source from RAY-UI for your convenience. It is recommended to include an Image Plane at the end for clearer results.

```xml
<object name="Circle Source" type="Circle Source">
   <param id="numberRays" enabled="T">20000</param>
   <param id="sourceWidth" enabled="T">0.065</param>
   <param id="sourceHeight" enabled="T">0.04</param>
   <param id="sourceDepth" enabled="T">1</param>
   <param id="numberCircles" enabled="T">1</param>
   <param id="maximumOpeningAngle" enabled="T">0</param>
   <param id="minimumOpeningAngle" enabled="T">0</param>
   <param id="deltaOpeningAngle" enabled="T">0</param>
   <param id="alignmentError" comment="No" enabled="T">1</param>
   <param id="translationXerror" enabled="F">0</param>
   <param id="translationYerror" enabled="F">0</param>
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
   <param id="energySpread" enabled="T">0</param>
   <param id="linearPol_0" enabled="T">1</param>
   <param id="linearPol_45" enabled="T">0</param>
   <param id="circularPol" enabled="T">0</param>
   <param id="sourcePulseType" comment="all rays start simultaneously" enabled="T">0</param>
   <param id="sourcePulseLength" enabled="F">0</param>
</object>
```