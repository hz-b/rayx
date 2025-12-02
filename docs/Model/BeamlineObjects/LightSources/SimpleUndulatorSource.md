# Simple Undulator Source

The Simple Undulator source in RAYX approximates the physics of a real undulator source, allowing for faster tracing of an undulator light beam. This light source uses **Gaussian Beam Theory**, which simplifies the beam properties significantly. By treating the light beam as a collection of individual rays, complex wave theory can be substituted with approximations.

Simply put, **Gaussian Beam Theory** states that certain beam properties are distributed in Gaussian patterns. These patterns flatten or squeeze after collisions with optical elements, but generally, the beam maintains a Gaussian bell curve shape.

## Light Properties

In RAYX, every ray is characterized by four properties, which are randomly generated to ensure minimal systematic errors in the simulation:

- **Origin**: Position in x, y, and z coordinates.
- **Direction**: Direction vector of the ray.
- **Photon Energy**: Energy of the photon carried by the ray.
- **Polarization**: Polarization state of the photon.

### Origin

The origin for each ray is described by x, y, and z positions.

## Tracing Parameters

Key parameters used for tracing an undulator source include:

- **sourceDepth**: Depth of the light source.
- **linearPol0**: Linear polarization at 0 degrees.
- **linearPol45**: Linear polarization at 45 degrees.
- **circularPol**: Circular polarization.
- **undulatorLength**: Length of the undulator.
- **photonEnergy**: Energy of the photons.
- **sigmaType**: Type of sigma (distribution type).
- **electronSigmaX**: Horizontal electron beam size.
- **electronSigmaXs**: Horizontal electron beam divergence.
- **electronSigmaY**: Vertical electron beam size.
- **electronSigmaYs**: Vertical electron beam divergence.
- **Energy**: Photon energy level.
- **Energy Spread**: Spread in energy.
- **Energy Spread Type**: Type of energy spread (e.g., white band).
- **Energy Distribution Type**: Distribution type of the photon energy.
- **Energy Distribution File**: File used to define energy distribution.

## RML Object

To trace a Simple Undulator Source using an RML file, you can define the source in an XML format. The default configuration from RAY-UI is provided here, which can be customized for your specific needs. It is advisable to include an Imageplane at the end for more accurate results.

```xml
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
    <z>0.0000000000000000</x>
   </param>
   <param id="worldYdirection" enabled="F">
    <x>0.0000000000000000</x>
    <y>1.0000000000000000</y>
    <z>0.0000000000000000</x>
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

## Literature
[1] Representation of a Gaussian Beam by Rays, P. P. Crooker, W. B. Colson, and J. Blau, Physics Department, Naval Postgraduate School, Monterey, California. Published 2006.