# Light Sources

Light Sources are the starting point for each ray. Here the Properties of the light are set. Depending on the user input the direction, photon energy and light polarization are determined. In this section, you can find a description on
how the light sources are implemented and how to employ them for different beamlines. Depending on the needs it can be usefull to know which light source suits best. 
The important part of the implementation of light sources in RAYX are the overall distributions of the values. Most of the Lightsources produce a spectrum of light rays. 

## Implemented Sources

Currently there are six light sources implemented in RAYX. They are fundamentaly different and serve different purpuses. 

- Dipole Source
- Matrix Source
- Point Source
- Circle Source
- Pixle Source
- Simple Undulator Source

The Matrix and Point Sources are conceptual sources, wheras the implementation of the Dipole Source is aiming to be as close to the reality as possible. 

## Light Properties

In RAYX every ray is described by four properties. Each light source has a different approach to determine these, depending on which parts should be realisic and what should be syntheticaly generated. The values are almost always in a given distribution-window and randomly generated. This guarantees that minimal systematic errors are impacting the ray generation. The user can choose a distribution window and a distribution type.

Parameters:
- Origin
- Direction
- Photonenergy
- Polarisation.

### Origin

The user sets the following parameters:
* source width(x-dir)
* source height(y-dir) 
* source depth(z-dir) 
* distribution types (up tp 3 different ones)

The origin for each ray is described by a x, y and z value. 

With repeatability in mind the Maxtrix Source has a completely deterministic aproach to set the origin of each ray. All rays are positioned in a uniform grid inside the given width and height. Depending on the number of rays this grid can look scarsly or completely filled. 

The Point Source has set distribution types for the positions. Every ray has a random origin but looking at all rays combined, they represent the chosen distribution type. The common choises are either hard-edge or soft-edge distributions. 

The Dipole Source additionaly takes the horizontal divergence and bending radius into account. The origin positions are in a 3 dimensual bend, representing the trajectory of the electrons in the synchrotron. The position has a natural distribution which is dependant on the horizontal divergence of the electron beam. 

### Direction

The user sets the following parameters:
* horizontal divergence (x-dir)
* vertical divergence (y-dir)
* distribution type

The direction for each ray is calculated for phi and psi values. They are the horizontal and vertical angles of the direction.
A direction vector is calculated from these angles and used for the tracing process. 

And again there are differences for each source which are described in the respective chapters.

### Energy

The user sets the following parameters:
* Photonenergy (mean)
* Energy spread
* distribution type
* (File path)

The Photonenergy is understood as the mean energy in a given distribution. If the energyspread is defined as 0 all rays have the same energy. 
All the iformation can also be 

The energy can be understood as the equivalent to the wavelength of the lightray. Different Distributiontypes are available to observe the behaiviour. 

### Polarisation

The light polarisation impacts the reflective properties of the ray. Depending on the lightsource it is calculated or given by the user. 

For more information on how to implement and use light sources please read the dokumentation for the spesific light source (updated soon).

##