# Point Source

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

## Tracing Parameter

- sourceWidthDistribution
- sourceHeightDistribution
- horDivDistribution
- verDivDistribution
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

To trace a Point Source through an RML File, utilize an XML Object to encompass all pertinent information for the light source. The default configuration for the light source from RAY-UI is presented here for your ease of use. It is recommended to include an Imageplane at the end for clearer results.

```XML
<object name="Point Source" type="Point Source">
    <param id="numberRays" enabled="T">200</param>
    <param id="sourceWidthDistribution" comment="gaussian (sigma)" enabled="T">1</param>
    <param id="sourceWidth" enabled="T">0.065</param>
    <param id="sourceHeightDistribution" comment="gaussian (sigma)" enabled="T">1</param>
    <param id="sourceHeight" enabled="T">0.04</param>
    <param id="sourceDepth" enabled="T">1</param>
    <param id="horDivDistribution" comment="gaussian (sigma)" enabled="T">1</param>
    <param id="horDiv" enabled="T">1</param>
    <param id="verDivDistribution" comment="gaussian (sigma)" enabled="T">1</param>
    <param id="verDiv" enabled="T">1</param>
    <param id="alignmentError" comment="Yes" enabled="T">0</param>
    <param id="translationXerror" enabled="F">5</param>
    <param id="translationYerror" enabled="F">5</param>
    <param id="rotationXerror" enabled="F">0</param>
    <param id="rotationYerror" enabled="F">0</param>
    <param id="energyDistributionType" comment="Values" enabled="T">1</param>
    <param id="photonEnergyDistributionFile" absolute="" enabled="F"></param>
    <param id="photonEnergy" enabled="T">151</param>
    <param id="energySpreadType" comment="white band" enabled="T">0</param>
    <param id="energySpreadUnit" comment="eV" enabled="T">0</param>
    <param id="energySpread" enabled="T">6</param>
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


```
