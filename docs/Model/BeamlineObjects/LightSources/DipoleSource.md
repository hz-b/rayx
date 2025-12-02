# Dipole Source

The Dipole Source in RAYX is implemented to generate a realistic light beam. To simulate realistic behavior, the natural spectral and spatial distributions of synchrotron radiation are used as a foundation. Specifically, the distributions defined by the universal function for synchrotron radiation, as outlined by Wiedemann in *Synchrotron Radiation*, p. 159, are utilized.

## Background

Dipole sources are among the most common light sources used to generate synchrotron radiation. In synchrotrons, dipole magnets are used to bend a charged particle beam. Depending on the strength of the magnetic field and other adjustable parameters, bending the particle beam produces X-rays—high-energy photons traveling at relativistic speeds. These photons then travel through the beamline, interacting with optical elements, and eventually reaching an experiment at the end of the line.

## Light Properties

In RAYX, every ray is described by four properties. Each light source has a different approach to determining these, depending on which parts should be realistic and which should be synthetically generated. The values are randomly generated for the given distribution, ensuring that minimal systematic errors impact the simulation. The user provides a distribution window for these properties.

### Origin

The origin of each ray is described by its x, y, and z positions. For the dipole source, the position depends on the strength of the dipole magnet. In the coordinate system for the light source, x lies on the horizontal plane with z, while the y direction is "up" and "down" when looking from the source down the beamline (toward the experiment or the next optical element, described by z). The user has limited influence over the distribution of the position, which is primarily determined by the magnetic field and the horizontal divergence window.

### Direction

The direction is determined by the vertical and horizontal angles, based on the user's input for vertical and horizontal divergence. The direction is calculated using two variables: the Phi and Psi angles. Phi is determined randomly with a uniform distribution over the given horizontal divergence. Psi is more complex, depending on the polarization. From the given vertical electron beam divergence, a distribution window is calculated.

### Energy

The photon energy depends on the specified mean energy and the energy spread. The focus of the simulation is the distribution of photon energies among all generated rays. The energy can be understood as equivalent to the wavelength of the photon. For the dipole source, energy is randomly distributed according to the universal function for synchrotron radiation. The basis for this simulation is Helmut Wiedemann’s description in *Synchrotron Radiation*, p. 259 (D.21), where edge cases simplify the simulation.

### Polarization

Light polarization affects the reflective properties of the ray. Polarization is also important for the distribution of the vertical direction of the ray. As described by Helmut Wiedemann (*Synchrotron Radiation*, p. 155 (9.78)), polarization, direction, and photon energy jointly determine the distribution.

## Struct Ray

In RAYX, all the generated information from the light sources is transferred to the optical elements using the `Ray` structure.

## Tracing Parameters

- Energy Distribution
- Photon Flux
- Electron Energy Orientation
- Source Pulse Type
- Bending Radius
- Electron Energy
- Photon Energy
- Vertical E-beam Divergence
- Energy Spread
- Energy Spread Unit
- Horizontal Divergence
- Source Height
- Source Width
- Energy
- Energy Spread
- Energy Spread Type
- Energy Distribution Type
- Energy Distribution File

## RML Object

To trace a Dipole Source through an RML file, use an XML object to contain all relevant information for the light source. Below is the default configuration for the light source from RAY-UI for your convenience. It is recommended to include an Image Plane at the end for clearer results.

```xml
<object name="Dipole Source" type="Dipole Source">
    <param id="numberRays" enabled="T">200</param>
    <param id="sourceWidth" enabled="T">0.065</param>
    <param id="sourceHeight" enabled="T">0.04</param>
    <param id="verEbeamDiv" enabled="T">1</param>
    <param id="horDiv" enabled="T">1</param>
    <param id="electronEnergy" enabled="T">1.7</param>
    <param id="electronEnergyOrientation" comment="clockwise" enabled="T">0</param>
    <param id="bendingRadius" enabled="T">4.35</param>
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
    <param id="energySpreadUnit" comment="eV" enabled="T">0</param>
    <param id="energySpread" enabled="T">0</param>
    <param id="sourcePulseType" comment="all rays start simultaneously" enabled="T">0</param>
    <param id="sourcePulseLength" enabled="F">0</param>
    <param id="photonFlux" enabled="T">2.76089e+12</param>
</object>
```