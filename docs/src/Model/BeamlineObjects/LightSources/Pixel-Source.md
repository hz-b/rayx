# Pixel Source

## Light Properties

In RAYX, every ray is described by four properties. The values are randomly generated for the given distribution. This guarantees that minimal systematic errors impact the simulation.

Light properties:
- Origin
- Direction
- Photon Energy
- Polarization

### Origin

The origin for each ray is described by its x, y, and z position.

## Tracing Parameters

- Ver Div
- Hor Div
- Source Depth
- Source Height
- Source Width
- Linear Pol 0
- Linear Pol 45
- Circular Pol
- Energy
- Energy Spread
- Energy Spread Type
- Energy Distribution Type
- Energy Distribution File

## RML Object

To trace a **Pixel Source** through an RML file, use an XML object to encompass all pertinent information for the light source. The default configuration for the light source from RAY-UI is presented here for ease of use. It is recommended to include an Imageplane at the end for clearer results.

```xml
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