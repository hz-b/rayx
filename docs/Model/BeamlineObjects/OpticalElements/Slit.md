# Slit

The **Slit** is an optical element that allows users to define a cutout through which light rays pass. You can configure the shape and size of the slit opening, as well as the size and shape of a central beamstop to block part of the light. The Slit element also accounts for Fraunhofer diffraction, producing realistic diffraction patterns when light interacts with the slit, especially in the case of single-slit diffraction.


## Tracing Parameter

Mandatory:
- Reflectivity Type (see more in chapter Optical Elements)
- Slope Error
- World Position
- Misalignment

Cutout:
- Opening Shape
- Opening Width
- Opening Height
- Central Beamstop
- Total Width Stop
- Total Height Stop


## Fraunhofer Diffraction (Rectangular Slits)

The RAYX simulation software calculates **Fraunhofer diffraction** for rectangular slits, which impacts the diffraction angle of rays passing through the slit. The diffraction effect is dependent on the slit dimensions and the wavelength of the light. The diffraction pattern produced follows the well-known Fraunhofer single-slit diffraction model, where the intensity of the light depends on the angle and the size of the slit.

For a rectangular slit of dimension `b`, the diffraction angle `dAngle` is calculated based on the equation:

\[
u = \frac{\pi b \sin(\theta)}{\lambda}
\]

where:
- \( b \) is the width or height of the slit
- \( \theta \) is the diffraction angle
- \( \lambda \) is the wavelength of the light

The intensity distribution is proportional to \( \left( \frac{\sin(u)}{u} \right)^2 \).

The algorithm ensures that rays with different angles are assigned based on a random distribution, simulating the diffraction pattern as light passes through the slit.

### Circular Apertures and Zone Plates

In addition to rectangular slit diffraction, the software also supports **Bessel diffraction** for circular slits and zone plates. For a circular aperture of radius `r`, the diffraction is modeled using Bessel functions, producing characteristic ring-like diffraction patterns.

In this case, the diffraction angle `dphi` and `dpsi` are calculated based on the aperture radius and wavelength, simulating the radial symmetry of diffraction patterns from circular openings.



## RML Object

To track a Slit using an RML File, you'll require an XML Object to encompass all the mirror details.

```XML
  <object name="Slit" type="Slit">
   <param id="geometricalShape" comment="rectangle" enabled="T">0</param>
   <param id="totalWidth" enabled="T">20</param>
   <param id="totalHeight" enabled="T">2</param>
   <param id="centralBeamstop" comment="none" enabled="T">0</param>
   <param id="totalWidthStop" enabled="F">20</param>
   <param id="totalHeightStop" enabled="F">1</param>
   <param id="distancePreceding" enabled="T">10000</param>
   <param id="azimuthalAngle" enabled="T">0</param>
   <param id="alignmentError" comment="No" enabled="T">1</param>
   <param id="translationXerror" enabled="F">0</param>
   <param id="translationYerror" enabled="F">0</param>
   <param id="translationZerror" enabled="F">0</param>
   <param id="rotationXerror" enabled="F">0</param>
   <param id="rotationYerror" enabled="F">0</param>
   <param id="rotationZerror" enabled="F">0</param>
   <param id="worldPosition" enabled="F">
    <x>0.0000000000000000</x>
    <y>0.0000000000000000</y>
    <z>10000.0000000000000000</z>
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
  </object>
```


### References
For further reading on Fraunhofer diffraction, please refer to the [Wikipedia page on Fraunhofer Diffraction](https://en.wikipedia.org/wiki/Fraunhofer_diffraction) or standard optics textbooks.