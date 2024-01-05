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