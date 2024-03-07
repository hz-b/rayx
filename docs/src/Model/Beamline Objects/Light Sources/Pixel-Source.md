# Pixel Source

TBA

## Light Properties

In RAYX every ray is described by four properties. The values are randomly generated for the given distirubution. This guarantees that minimal systematic errors are impacting the simulation. 

light properties:
- Origin
- Direction
- Photonenergy
- Polarisation

### Origin

The Origin for each Ray is described by x, y and z Position. 

## RML Object

To trace a Pixel Source through an RML File, utilize an XML Object to encompass all pertinent information for the light source. The default configuration for the light source from RAY-UI is presented here for your ease of use. It is recommended to include an Imageplane at the end for clearer results.

```XML
<object name="Pixel Source" type="Pixel Source">
   <param id="numberRays" enabled="T">20000</param>
   <param id="sourceWidth" enabled="T">0.065</param>
   <param id="sourceHeight" enabled="T">0.04</param>
   <param id="sourceDepth" enabled="T">1</param>
   <param id="horDiv" enabled="T">1</param>
   <param id="verDiv" enabled="T">1</param>
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
