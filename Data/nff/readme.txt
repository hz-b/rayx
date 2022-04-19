
		Low-Energy X-ray Interaction Coefficients:
		Photoabsorption, Scattering, and Reflection
			E = 30-30,000 eV, Z = 1-92

	        B. L. Henke, E. M. Gullikson, and J. C. Davis
			Center for X-Ray Optics, 2-400
			Lawrence Berkeley Laboratory
			Berkeley, California 94720

These files were used to generate the tables published in reference [1].
The files contain three columns of data: Energy(eV), f_1, f_2,
where f_1 and f_2 are the atomic (forward) scattering factors.
There are 500+ points on a uniform logarithmic mesh with points
added 0.1 eV above and below "sharp" absorption edges.
(Note: below 29 eV f_1 is set equal to -9999.) 
The tabulated values of f_1 contain a relativistic, energy independent,
correction given by, Z* = Z - (Z/82.5)^(2.37).

The atomic photoabsorption cross section, mu_a, may be readily obtained
from the values of f_2 using the relation,

			mu_a = 2*r_0*lambda*f_2 

where r_0 is the classical electron radius, and lambda is the wavelength.
The index of refraction for a material with N atoms per unit volume
is calculated by,

		n = 1 - N*r_0*(lambda)^2*(f_1+if_2)/(2*pi).

These (semi-empirical) atomic scattering factors are based upon
photoabsorption measurements of elements in their elemental state.
The basic assumption is that condensed matter may be modeled as a
collection of non-interacting atoms.  This assumption is in general
a good one for energies sufficiently far from absorption thresholds.
In the threshold regions, the specific chemical state is important
and direct experimental measurements must be made.

These tables are based on a compilation of the available experimental
measurements and theoretical calculations.  For many elements there is
little or no published data and in such cases it was necessary to
rely on theoretical calculations and interpolations across Z.
In order to improve the accuracy in the future considerably more
experimental measurements are needed.

Please send any comments to EMGullikson@lbl.gov.

Note that the following elements have been updated since the publication
of Ref. [1] in July 1993.

[1]  B. L. Henke, E. M. Gullikson, and J. C. Davis,
Atomic Data and Nuclear Data Tables Vol. 54 No. 2 (July 1993).

Element		Updated		Energy Range
Mg		1994		30-50 eV
Al		1994		30-73 eV
Si		1994		30-100 eV
Au		1994		30-300 eV, 2000-6500 eV
Li		1994		2000-30000 eV
Si		1995		30-500 eV
Fe		1995		600-800 eV
Mo		1997,2004	10-930, 25-60 eV
Be		2004		40-250 eV
W		2004		35-150 eV
Ru		2004		40-1300 eV
Ti		2004		20-150 eV
Sc		2006		20-1300 eV
Gd		2007		12-450 eV
La		2007		14-440 eV
Ta		2009		20-1000 eV
Zr		2010		20-1000 eV
Mg		2011		10-1300 eV


