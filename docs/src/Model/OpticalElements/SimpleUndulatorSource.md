# Simple Undulator Source

The simple Undulator source mimics the physics of a real undulator source and allows the user to trace an approximated undulator lightbeam much faster. 
This light source utilieses the Gaussian Beam Theorie which allows us to simplify the beam Properties a lot. We already treat the lightbeam as a group of individual rays. This gives us the possibility to substitute all complex wave theory laws on our light beam with approximations.[1]
Simply put the Gaussian Beam Theory says that some beam properties are distributed in gaussian patterns. And that the pattern flattens or sqishes from collisions with optical elements but generaly remains a gaussian bellcurve. 


## Light Properties

In RAYX every ray is described by four properties. The values are randomly generated for the given distirubution. This guarantees that minimal systematic errors are impacting the simulation. 

light properties:
- Origin
- Direction
- Photonenergy
- Polarisation

### Origin

The Origin for each Ray is described by x, y and z Position. 


# Literature
[1]Representation of a Gaussian beam by rays
P. P. Crooker,a兲 W. B. Colson, and J. Blau
Physics Department, Naval Postgraduate School, Monterey, California 93943
Received 3 October 2005; accepted 7 April 2006兲