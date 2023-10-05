# Dipole Source

The Dipole Source in RAYX is implemented to generate an realistic lightbeam. To simulate a realistic representation of behaviour, the natural spectral and spatial distributions of synchrotron radiation is used as a foundation. Specificly the distributions defined by the universal function for synchrotron radiation defined by Wiedemann, Synchrotron Radiation P. 159 (). 

## Light Properties

In RAYX every ray is described by four properties. Each light source has a different approach to determine these, depending on which parts should be realisic and what should be syntheticaly generated. The determination is almost always in a given distribution-window and randomly generated. This guarantees that minimal systematic errors are impacting the simulation. The User can choose a Distribution window for the properties and sometimes a distribution type like a gaussian or hardedge distribution. 

The parameters are 
- Origin
- Direction
- Energy
- Polarisation.

### Origin

The Origin for each Ray is described by a x, y and z Position. For the dipolesource the Position is dependent on the design of the Dipole Source. In the coordinates system for the Lightsource x is on the horizontal plane with z. The y direction is "up" and "down" looking from the source down the beamline. Which is the direction to the experiment, or at least to the next optical element and is described by z.
The user has limeted invluence on the distribution on the position. It is determined by the magnetic field and the horizontal divergence window 


### Direction

The Direction is determined by the vertical and horizontal angle. Those are determind by the users input for the vertical and horizontal Divergence. And Again there are differences for each Source which are described in the respective chapters.

### Energy

The Photonenergy is dependant on the given mean-energy and the energyspread. Again the focus of the simulation lays in the distribution of the photonenergies between all generated rays. The energy can be understood as the equvalent to the wavelength of the lightray. Different Distributiontypes are available to observe the behaiviour. 

### Polarisation

The light polarisation impacts the reflective properties of the ray. Depending on the lightsource it is calculated or given by the user. 

## Struct Ray
In RAYX we transfer all the generated information from the light sources to the optical elements via the struct Ray. 