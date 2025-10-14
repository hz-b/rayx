# Light Sources

Light sources are the starting point for each ray. Here, the properties of the light are set. Depending on user input, the direction, photon energy, and light polarization are determined. This section provides a description of how the light sources are implemented and how to use them for different beamlines. Depending on your needs, it can be useful to understand which light source is most suitable.  
The key part of light source implementation in RAYX is the overall distribution of values. Most light sources produce a spectrum of light rays. 

## Implemented Sources

Currently, there are six light sources implemented in RAYX. They are fundamentally different and serve various purposes:

- Dipole Source
- Matrix Source
- Point Source
- Circle Source
- Pixel Source
- Simple Undulator Source

The Matrix and Point Sources are conceptual sources, whereas the Dipole Source aims to replicate real-world behavior as accurately as possible.

## Light Properties

In RAYX, every ray is described by four properties. Each light source has a different approach to determining these, depending on which aspects should be realistic and which should be synthetically generated. The values are almost always in a given distribution window and are randomly generated. This ensures minimal systematic errors impact the ray generation. The user can choose a distribution window and a distribution type.

Parameters:
- Origin
- Direction
- Photon Energy
- Polarization

### Origin

The user sets the following parameters:
* Source width (x-dir)
* Source height (y-dir)
* Source depth (z-dir)
* Distribution types (up to 3 different ones)

The origin for each ray is described by x, y, and z values.

With repeatability in mind, the Matrix Source has a fully deterministic approach to setting the origin of each ray. All rays are positioned in a uniform grid within the given width and height. Depending on the number of rays, this grid can appear sparse or fully filled.

The Point Source uses preset distribution types for the positions. Each ray has a random origin, but when considered collectively, they represent the chosen distribution type. Common choices are either hard-edge or soft-edge distributions.

The Dipole Source additionally considers the horizontal divergence and bending radius. The origin positions are in a 3-dimensional curve, representing the trajectory of the electrons in the synchrotron. The positions have a natural distribution that depends on the horizontal divergence of the electron beam.

### Direction

The user sets the following parameters:
* Horizontal divergence (x-dir)
* Vertical divergence (y-dir)
* Distribution type

The direction for each ray is calculated using the phi and psi values, which are the horizontal and vertical angles of the direction. A direction vector is then calculated from these angles and used in the tracing process.

Each source has different directional behaviors, which are detailed in their respective chapters.

### Energy

The user sets the following parameters:
* Photon Energy (mean)
* Energy Spread
* Distribution type
* (File path)

Photon energy is understood as the mean energy in a given distribution. If the energy spread is defined as 0, all rays have the same energy.

The energy is equivalent to the wavelength of the light ray. Different distribution types are available for observing the behavior.

### Polarization

Light polarization impacts the reflective properties of the ray. Depending on the light source, it is either calculated or provided by the user.

For more information on how to implement and use light sources, please refer to the documentation for the specific light source (updated soon).
