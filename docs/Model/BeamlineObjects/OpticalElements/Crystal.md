# Plane-Crystal


## Crystal Diffraction

We simulate X-ray diffraction using the **dynamical theory** in the **Bragg-case geometry**, assuming a **perfect and thick crystal**. 

We compute the **reflection coefficient** based on this model. The reflected field is then determined from it. To run this simulation, the following user inputs are required:

### Required Inputs
- **Photon energy** (in eV)  
- **lattice spacing*2** (in nm)  
- **Unit cell volume** (in nm³)  
- **Structure factors**:  
  - F₀, F_H, F_H̄ with real/imaginary components
- **Crystal surface offset angle** α (in radians)


The following sections explain how each physical quantity and formula is derived and used in the simulation.
This section details the implementation of dynamical X-ray diffraction theory for perfect crystals, based on the foundational work of Batterman & Cole (1964). 

### Bragg Angle Calculation

\\[
\theta_B = \arcsin\left(\frac{\lambda}{2d}\right)
\\]

- **Function**: `getBraggAngle(energy, dSpacing2)`
- **Purpose**: Calculate the Bragg diffraction angle
- **Parameters**:
  - `energy`: Photon energy (eV)
  - `dSpacing2`: lattice spacing*2 (nm)
- **Returns**: Bragg angle in radians

### Asymmetry Factor

\\[
b = \frac{\sin(\theta_B - \alpha)}{\sin(\theta_B + \alpha)}
\\]

- **Function**: `getAsymmetryFactor(braggAngle, offsetAngle)`
- **Purpose**: Account for crystal surface orientation effects
- **Parameters**:
  - `braggAngle`: Calculated Bragg angle
  - `offsetAngle`: Surface tilt angle α

This definition corresponds to the asymmetry factor derived from direction cosines  
and is consistent with the definition on page 690 in [Batterman & Cole (1964)](https://doi.org/10.1103/RevModPhys.36.681).

### Diffraction Prefactor

\\[
\Gamma = \frac{r_e \lambda^2}{\pi V}
\\]

- **Function**: `getDiffractionPrefactor(wavelength, unitCellVolume)`
- **Purpose**: Calculate scaling factor for absorption/dispersion
- **Parameters**:
  - `wavelength`: X-ray wavelength (nm)
  - `unitCellVolume`: Unit cell volume (nm³)


Gamma is defined in [Batterman & Cole (1964)](https://doi.org/10.1103/RevModPhys.36.681) p. 685.

### Eta Parameter (η)

\\[
\eta = \frac{b\Delta\theta\sin 2\theta + \frac{1}{2}\Gamma F_0(1-b)}{\Gamma|P|\sqrt{|b|}\sqrt{F_H F_{\overline{H}}}}
\\]

- **Function**: `computeEta(theta, bragg, asymmetry, FH_re, FH_im, FHC_re, FHC_im, F0_re, F0_im, polFactor, gamma)`
- **Purpose**: Compute normalized angular deviation parameter
- **Parameters**:
  - Structure factors (F₀, F_H, F_H̄) with real/imaginary components
  - `polFactor`: Polarization factor (1 or |cos2θ_B|)


Eta is defined as equation (32) in [Batterman & Cole (1964)](https://doi.org/10.1103/RevModPhys.36.681) p. 690 .

### Reflection Coefficient (R)

\\[
R = \left(\eta \pm \sqrt{\eta^2 - 1}\right)\sqrt{\frac{F_H}{F_{\overline{H}}}}
\\]

- **Function**: `computeR(eta, FH_re, FH_im, FHC_re, FHC_im)`
- **Purpose**: Calculate complex reflection coefficient
- **Note**: Sign selection based on real part of η

This function is based on Equation (103) from  [Batterman & Cole (1964)](https://doi.org/10.1103/RevModPhys.36.681) p. 706.  
We applied the square to the structure factor terms in advance and omitted the leading \(|b|\) factor.


### Polarization Factors

Two states considered:
- \\(\sigma\\)-polarization: \\(P = 1\\)
- \\(\pi\\)-polarization: \\(P = |\cos 2\theta_B|\\)


### References

- Batterman, B. W., & Cole, H. (1964). *Dynamical Diffraction of X Rays by Perfect Crystals*. Reviews of Modern Physics, 36(3), 681–717. [https://doi.org/10.1103/RevModPhys.36.681](https://doi.org/10.1103/RevModPhys.36.681)
