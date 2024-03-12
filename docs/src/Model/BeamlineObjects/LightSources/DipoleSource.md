# Dipole Source

The Dipole Source in RAYX is implemented to generate a realistic lightbeam. To simulate a realistic representation of behaviour, the natural spectral and spatial distributions of synchrotron radiation are used as a foundation. Specificly the distributions defined by the universal function for synchrotron radiation defined by Wiedemann, Synchrotron Radiation P. 159 (). 

## Backround

Dipole Sources are among the most common lightsources used to generate synchrotron radiation. In synchrotrons dipole magnets are used to bend a charged particle beam. Depending on the strength of the magnetic field and other changeble parameters the act of bending the particle beam produces X-Rays. Highly energized photons traveling with relativistic speeds. These photons than travel throuh the beamline, colliding with optical elements, and hitting an experiment at the end of the line. 

## Light Properties

In RAYX every ray is described by four properties. Each light source has a different approach to determine these, depending on which parts should be realisic and what should be syntheticaly generated. The values are randomly generated for the given distirubution. This guarantees that minimal systematic errors are impacting the simulation. The user gives a distribution window for the properties.

### Origin

The Origin for each Ray is described by a x, y and z Position. For the dipole source the position is dependent on the strength of the dipole magnet. 
In the coordinates system for the Lightsource x is on the horizontal plane with z. The y direction is "up" and "down" looking from the source down the beamline. Which is the direction to the experiment, or at least to the next optical element and is described by z.
The user has limeted influence on the distribution of the position. It is determined by the magnetic field and the horizontal divergence window.

### Direction

The Direction is determined by the vertical and horizontal angle. Those are determind by the users input for the vertical and horizontal Divergence. The Direction is calculated by two variables the Phi and Psi angle. 
Phi is determined randomly in a uniform spread on the given horizontal Divergence.
For Psi the calculation is a little more complex and dependent on the Polarisation.
Of the given vertical electron beam divergance a distribution window is calculated. 

### Energy

The photon energy is dependant on the given mean-energy and the energyspread. Again the focus of the simulation lays in the distribution of the photonenergies between all generated rays. The energy can be understood as the equvalent to the wavelength of the photon. For the dipole source the energy is randomly distributed according to the universal function for synchrotron radiation (1). 
The basis for the simulation lays in Helmut Wiedemanns description (Synchrotron Radiation P. 259 (D.21)). He gives edge cases which are usefull to simplyfy the simulation.

### Polarisation

The light polarisation impacts the reflective properties of the ray. The polarization is also important for the distiribution of the vertical direction value of the ray. As written by Helmut Wiedemanns (Synchrotron Radiation P. 155 (9.78)) the polarisation, direction and photon energy determine the distribution. 

## Struct Ray
In RAYX we transfer all the generated information from the light sources to the optical elements via the struct Ray. 

## Tracing Parameter

- Energy Distribution
- Photon Flux
- Electron Energy Orientation
- Source Pulse Type
- Bending Radius (Double)
- Electron Energy
- Photon Energy
- Vertical Ebeam Divergence
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

To trace a Dipole Source through an RML File, utilize an XML Object to encompass all pertinent information for the light source. The default configuration for the light source from RAY-UI is presented here for your ease of use. It is recommended to include an Imageplane at the end for clearer results.

```XML
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